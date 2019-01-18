/*
 * Copyright (C) 2018-2019  Yannick Jadoul
 *
 * This file is part of Parselmouth.
 *
 * Parselmouth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parselmouth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>
 */

#include "parselmouth/Parselmouth.h"

#include "praat/MelderUtils.h"
#include "utils/StringUtils.h"

#include <praat/sys/praat.h>
#include <praat/sys/praatP.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <cassert>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

using structData = structDaata;
using Data = Daata;
using autoData = autoDaata;

namespace {

template <typename T, typename PyT>
inline T extractKwarg(py::kwargs &kwargs, const std::string &name, T defaultValue, const std::string &pyName) {
	auto throwNotConvertible = [&]() { throw py::type_error("Keyword argument '" + name + "' should be convertible to " + pyName); };

	try {
		return py::cast<T>(PyT(kwargs.attr("pop")(name, defaultValue)));
	}
	catch (py::cast_error &) {
		throwNotConvertible();
	}
	catch (py::error_already_set &) {
		throwNotConvertible();
	}
	return defaultValue;
}

inline void checkUnkownKwargs(const py::kwargs &kwargs) {
	if (kwargs.size() > 0) {
		throw py::type_error("Unknown keyword argument '" + py::cast<std::string>(kwargs.begin()->first) + "'");
	}
}

py::object autoVECToArray(autoVEC &&vector) {
	if (!vector.at)
		return py::none();

	auto [size, at] = std::tuple(vector.size, vector.at); // Because of undefined order of evaluation of arguments, we need to make sure to save size and at before moving
	auto capsule = py::capsule(new autoVEC(std::move(vector)), [](void *v) { delete reinterpret_cast<autoVEC *>(v); });
	return py::array_t<double>(static_cast<size_t>(size), &at[1], capsule);
}

py::object autoMATToArray(autoMAT &&matrix) {
	if (!matrix.at)
		return py::none();

	auto [nrow, ncol, at] = std::tuple(matrix.nrow, matrix.ncol, matrix.at); // Because of undefined order of evaluation of arguments, we need to make sure to save nrow, ncol, and at before moving
	auto capsule = py::capsule(new autoMAT(std::move(matrix)), [](void *m) { delete reinterpret_cast<autoMAT *>(m); });
	return py::array_t<double, py::array::c_style>({static_cast<size_t>(nrow), static_cast<size_t>(ncol)}, &at[1][1], capsule);
}

class PraatEnvironment {
public:
	PraatEnvironment() : m_objects(theCurrentPraatObjects), m_interpreter(Interpreter_create(nullptr, nullptr)), m_lastId(0) {
		assert(m_objects->n == 0);
		m_objects->uniqueId = 0;
	}

	~PraatEnvironment() {
		for (auto i = m_objects->n; i > 0; --i) {
			praat_removeObject(i);
		}

		assert(m_objects->totalSelection == 0);
		assert(m_objects->n == 0);
	}

	auto interpreter() const {
		return m_interpreter.get();
	}

	void addObjects(const std::vector<std::reference_wrapper<structData>> &objects, bool select) {
		// Add references to the passed objects to the Praat object list
		for (auto &data: objects) {
			praat_newReference(&data.get()); // Since we're registering this is just a reference, running a command like "Remove" should normally be OK; through hack/workaround: a PraatObject now contains a boolean 'owned' to know if the data should be deleted
			m_objects->list[m_objects->n].isBeingCreated = false;
			if (select)
				praat_select(m_objects->n);
		}
		// praat_updateSelection will change which objects are selected, and we don't want that
		m_objects->totalBeingCreated = 0;
		praat_show();
		m_lastId = m_objects->uniqueId;
	}

	auto retrieveSelectedObjects(bool onlyNew = false) {
		[[maybe_unused]] auto numSelected = static_cast<size_t>(m_objects->totalSelection);

		std::vector<py::object> selected;
		for (auto i = 1; i <= m_objects->n; ++i) {

			auto &praatObject = m_objects->list[i];
			if (praatObject.isSelected) {
				// Sometimes we're only interested in returning new objects (i.e. to implement 'call'), but Praat did not add new ones and did not deselect the old ones (e.g. when a warning is shown).
				if (onlyNew && praatObject.id <= m_lastId)
					continue;

				praat_deselect(i); // Hack/workaround: if this is not called, Praat will call it while removing the object from the list, and crash on accessing object -> classInfo

				// We cannot just wrap the object as autoData, because it might not be newly created, and so a pybind11 holder in a Python object owns it rather than Praat (see praatObject.owned)
				// Luckily, pybind11 also provides the solution: py::cast will return a py::object with the existing Python instance for a C++ object that already has a Python counterpart
				auto object = praatObject.object;
				praatObject.object = nullptr; // Just in case py::cast goes wrong and throws, let's clean out praatObject.object to make sure we don't segfault with a double delete in ~PraatEnvironment
				selected.emplace_back(py::cast(object)); // If not owned/not new, this will return the existing instance; if owned/new this will take ownership and create a autoData holder inside the py::object
			}
		}

		assert(numSelected == selected.size() + m_objects->totalSelection);

		return selected;
	}

	std::unordered_map<std::u32string, py::object> getVariables() {
		std::unordered_map<std::u32string, py::object> variables;

		for (const auto &[name, value] : m_interpreter->variablesMap) {
			assert(name == value->string.get());

			if (name.length() > 0 && name.back() == U'$') {
				variables.emplace(name, py::cast(value->stringValue.get()));
			}
			else if (name.length() > 1 && name.substr(name.length() - 2) == U"##") {
				// Steal matrix value, so clear out old variable to stop Praat from deleting when cleaning up the interpreter
				autoMAT matrix;
				matrix.adoptFromAmbiguousOwner(value->numericMatrixValue);
				value->numericMatrixValue = emptyMAT;
				variables.emplace(name, autoMATToArray(std::move(matrix)));
			}
			else if (name.length() > 0 && name.back() == U'#') {
				// Steal vector value, so clear out old variable to stop Praat from deleting when cleaning up the interpreter
				autoVEC vector;
				vector.adoptFromAmbiguousOwner(value->numericVectorValue);
				value->numericVectorValue = emptyVEC;
				variables.emplace(name, autoVECToArray(std::move(vector)));
			}
			else {
				variables.emplace(name, py::cast(value->numericValue));
			}
		}

		return variables;
	}

	void toPraatArg(structStackel &stackel, const py::handle &arg);

	std::vector<structStackel> toPraatArgs(const py::args &args) {
		std::vector<structStackel> praatArgs(1 + args.size()); // Cause ... Praat, and 1-based indexing, and ... grmbl ... well, at least the .data() pointer of the std::vector cannot be a nullptr now
		for (size_t i = 1; i < praatArgs.size(); ++i) {
			toPraatArg(praatArgs[i], args[i-1]);
		}
		return praatArgs;
	}

	py::object fromPraatResult(const std::u32string &callbackName, const std::u32string &interceptedInfo);

private:
	// Let's not trust the combination of Praat and static initialization order to safely have a static autoInterpreter member

	PraatObjects m_objects;
	autoInterpreter m_interpreter;

	std::vector<py::object> m_keepAliveObjects;

	integer m_lastId;
};

// Workarounds since GCC (6) doesn't seem to like the brace initialization of the nested anonymous struct
void fillStackel(structStackel &s, double number) { s.which = Stackel_NUMBER; s.number = number; }
void fillStackel(structStackel &s, bool boolean) { s.which = Stackel_STRING; s._string = Melder_dup(boolean ? U"yes" : U"no"); }
void fillStackel(structStackel &s, const std::u32string &string) { s.which = Stackel_STRING; s._string = Melder_dup(string.c_str()); }
void fillStackel(structStackel &s, const VEC &vector, bool owned) { s.which = Stackel_NUMERIC_VECTOR; s.owned = owned; s.numericVector = vector; }
void fillStackel(structStackel &s, const MAT &matrix, bool owned) { s.which = Stackel_NUMERIC_MATRIX, s.owned = owned; s.numericMatrix = matrix; }

void PraatEnvironment::toPraatArg(structStackel &stackel, const py::handle &arg) {
	if (py::isinstance<py::int_>(arg) || py::isinstance<py::float_>(arg))
		return fillStackel(stackel, py::cast<double>(arg));
	else if (py::isinstance<py::bool_>(arg))
		return fillStackel(stackel, py::cast<bool>(arg));
	else if (py::isinstance<py::str>(arg) && (PY_MAJOR_VERSION < 3 || !py::isinstance<py::bytes>(arg)))
		return fillStackel(stackel, py::cast<std::u32string>(arg));

	try {
		// Let's not 'squeeze' the array, or we might interpret a 2D matrix as 1D vector!
		auto array = py::array_t<double, py::array::c_style>::ensure(arg);
		if (array.ndim() == 1) {
			// Keep the object alive until the PraatEnvironment goes out of scope, and avoid a copy
			m_keepAliveObjects.push_back(array);

			return fillStackel(stackel, VEC(array.mutable_data(0) - 1, array.shape(0)), false); // Remember Praat is 1-based
			// NOTE: If Praat ever contains commands/actions that modify the vector,
			//       we're in trouble since this will not consistently be reflected in the original NumPy array.
			//       However, since we're indicating the vector is not owned, at least the interpreter can know.
		} else if (array.ndim() == 2) {
			auto rows = new double*[array.shape(0)];
			for (ssize_t i = 0; i < array.shape(0); ++i) {
				rows[i] = array.mutable_data(i, 0) - 1;
			}

			m_keepAliveObjects.push_back(array);
			m_keepAliveObjects.push_back(py::capsule(rows, [](void *r) { delete reinterpret_cast<double**>(r); }));

			return fillStackel(stackel, MAT(rows - 1, array.shape(0), array.shape(1)), false);
		}

		throw py::value_error("Cannot convert " + std::to_string(array.ndim()) + "-dimensional NumPy array argument\"" + py::cast<std::string>(py::repr(arg)) + "\" to a Praat vector or matrix");
	}
	catch (py::cast_error &) {}
	catch (py::error_already_set &) {}

	throw py::value_error("Cannot convert argument \"" + py::cast<std::string>(py::repr(arg)) + "\" to a known Praat argument type");
}

py::object PraatEnvironment::fromPraatResult(const std::u32string &callbackName, const std::u32string &interceptedInfo) {
	if (startsWith(callbackName, U"REAL_"))
		return py::cast(Melder_atof(interceptedInfo.c_str()));

	if (startsWith(callbackName, U"INTEGER_"))
		return py::cast(Melder_atoi(interceptedInfo.c_str()));

	if (startsWith(callbackName, U"BOOLEAN_"))
		return py::cast(Melder_atoi(interceptedInfo.c_str()) != 0);

	if (startsWith(callbackName, U"COMPLEX_")) {
		// This is ugly, but as a tradeoff between keeping things simple and making them nice, the best we can do
		auto re = Melder_atof(interceptedInfo.c_str());
		auto reDigit = interceptedInfo.find_first_of(U"0123456789");
		auto imStart = reDigit != std::u32string::npos ? interceptedInfo.find_first_of(U"+-", reDigit) : std::u32string::npos;
		auto im = Melder_atof(imStart != std::u32string::npos ? interceptedInfo.c_str() + imStart + (interceptedInfo[imStart] == U'+') : U"");
		if (isundef(re) || isundef(im))
			return py::cast(undefined);
		return py::cast(std::complex<decltype(re)>(re, im));
	}

	if (startsWith(callbackName, U"NUMVEC_"))
		return autoVECToArray(std::move(theInterpreterNumvec));

	if (startsWith(callbackName, U"NUMMAT_"))
		return autoMATToArray(std::move(theInterpreterNummat));

	if (startsWith(callbackName, U"NEW_") ||
	    startsWith(callbackName, U"NEW1_") ||
	    startsWith(callbackName, U"NEW2_") ||
	    startsWith(callbackName, U"NEWMANY_") ||
	    startsWith(callbackName, U"NEWTIMES2_") ||
	    startsWith(callbackName, U"READ1_") ||
	    startsWith(callbackName, U"READMANY_")) {

		auto selected = retrieveSelectedObjects(true);

		if (selected.size() == 1 && !(startsWith(callbackName, U"NEWMANY_") || startsWith(callbackName, U"READMANY_")))
			return std::move(selected[0]);
		else
			return py::cast(std::move(selected));
	}

	if (!interceptedInfo.empty()) // U"STRING_", U"HINT_", U"INFO_", U"LIST_"
		return py::cast(interceptedInfo);

	// U"HELP_", U"MODIFY_", U"PRAAT_", U"PREFS_", U"SAVE_"
	return py::none();

	// Weird: U"BUG_", U"DANGEROUS_"
	// Impossible: U"PLAY_", U"RECORD1_", U"WINDOW_"
	// Does nothing: U"GRAPHICS_"
	// Throws exception: U"MOVIE_"
}


auto callPraatCommand(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &command, py::args args, py::kwargs kwargs) {
	auto extraObjects = extractKwarg<std::vector<std::reference_wrapper<structData>>, py::list>(kwargs, "extra_objects", {}, "List[parselmouth.Data]");
	auto returnString = extractKwarg<bool, py::bool_>(kwargs, "return_string", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;
	environment.addObjects(objects, true);
	environment.addObjects(extraObjects, false);
	auto praatArgs = environment.toPraatArgs(args);

	// If there are arguments, let's help the user and append "..." to the command, if not yet there
	auto completedCommand = command;
	if (args.size() > 0 && !endsWith(command, U"..."))
		completedCommand += U"...";

	// Prepare to intercept the output of the command
	MelderInfoInterceptor interceptor;

	// Actually find the command and execute it
	// We need to pass a non-nullptr Interpreter to praat_doAction and praat_doMenuCommand if we want 'theInterpreterNumvec' and 'theInterpreterNummat' to be used
	Praat_Command executedCommand = nullptr;
	if (auto i = praat_doAction(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), environment.interpreter())) {
		executedCommand = praat_getAction(i);
	}
	else if (auto j = praat_doMenuCommand(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), environment.interpreter())) {
		executedCommand = praat_getMenuCommand(j);
	}
	else {
		Melder_throw(U"Command \"", command.c_str(), U"\" not available for given objects.");
	}

	// Based on the prefix of the command's callback, convert the result to a Python object
	assert(executedCommand);
	if (returnString)
		return py::cast(interceptor.get());
	else
		return environment.fromPraatResult(executedCommand->nameOfCallback, interceptor.get());
}

auto runPraatScript(const std::vector<std::reference_wrapper<structData>> &objects, char32 *script, py::args args, py::kwargs kwargs) {
	auto extraObjects = extractKwarg<std::vector<std::reference_wrapper<structData>>, py::list>(kwargs, "extra_objects", {}, "List[parselmouth.Data]");
	auto captureOutput = extractKwarg<bool, py::bool_>(kwargs, "capture_output", false, "bool");
	auto returnVariables = extractKwarg<bool, py::bool_>(kwargs, "return_variables", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;
	environment.addObjects(objects, true);
	environment.addObjects(extraObjects, false);
	auto praatArgs = environment.toPraatArgs(args);

	// Prepare to maybe intercept the output of the script
	auto interceptor = captureOutput ? std::make_unique<MelderInfoInterceptor>() : nullptr;

	try {
		Interpreter_readParameters(environment.interpreter(), script);
		Interpreter_getArgumentsFromArgs (environment.interpreter(), static_cast<int>(praatArgs.size() - 1), praatArgs.data());
		Interpreter_run(environment.interpreter(), script);
	} catch (MelderError) {
		Melder_throw(U"Script not completed.");
	}

	auto selectedObjects = environment.retrieveSelectedObjects();

	if (!captureOutput && !returnVariables) {
		return py::cast(std::move(selectedObjects));
	}
	else {
		std::vector<py::object> results;
		results.push_back(py::cast(std::move(selectedObjects)));
		if (captureOutput)
			results.push_back(py::cast(interceptor->get()));
		if (returnVariables)
			results.push_back(py::cast(environment.getVariables()));
		return py::object(py::tuple(py::cast(std::move(results))));
	}
}

auto runPraatScriptFromText(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &script, py::args args, py::kwargs kwargs) {
	auto fullScript = autostring32(Melder_dup(script.c_str()));
	Melder_includeIncludeFiles(&fullScript);

	return runPraatScript(objects, fullScript.get(), std::move(args), std::move(kwargs));
}

auto runPraatScriptFromFile(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &path, py::args args, py::kwargs kwargs) {
	auto file = pathToMelderFile(path);
	autostring32 script = MelderFile_readText(&file);

	{
		autoMelderFileSetDefaultDir dir(&file);
		Melder_includeIncludeFiles(&script);
	}

	return runPraatScript(objects, script.get(), std::move(args), std::move(kwargs));
}

auto castPraatCommand(const structPraat_Command &command) {
	return std::tuple(command.title.get(), command.nameOfCallback);
}

using CastedPraatCommand = decltype(castPraatCommand(std::declval<structPraat_Command&>()));

} // namespace

class PraatModule;

PRAAT_MODULE_BINDING(praat, PraatModule) {
	def("call",
	    [](const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({}, command, args, kwargs); },
	    "command"_a);

	def("call",
	    [](structData &data, const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({ std::ref(data) }, command, args, kwargs); },
	    "object"_a, "command"_a);

	def("call",
	    &callPraatCommand,
	    "objects"_a, "command"_a, R"(Call a Praat command.

This function provides a Python interface to call available Praat commands
based on the label in the Praat user interface and documentation, similar
to the Praat scripting language.

Calling a Praat command through this function roughly corresponds to the
following scenario in the Praat user interface or scripting language:

1. Zero, one, or multiple `parselmouth.Data` objects are put into Praat's
   global object list and are 'selected'.
2. The argument values are converted into Praat values.
   E.g., `True` and `False` are converted into ``"yes"`` and ``"no"``.
3. The Praat command is executed on the selected objects with the
   converted arguments.
4. The result of the command is returned. The type of the result depends
   on the result of the Praat command.
5. Praat's object list is emptied again, such that the following
   execution of this function is independent from the current one.

Parameters
----------
object : parselmouth.Data
    A single object to add to the Praat object list, which will be
    selected when the Praat command is called.
objects : List[parselmouth.Data]
    Multiple objects to be added to the Praat object list, which will be
    selected when the Praat command is called.
command : str
	The Praat action to call. This is the same command name as one would
    use in a Praat script and corresponds to the label on the button in
    the Praat user interface.
*args
    The list of values to be passed as arguments to the Praat command.
    Allowed types for these arguments are:

    - :class:`int` or :class:`float`: passed as a Praat numeric value
    - :class:`bool`: converted into ``"yes"``/``"no"``
    - :class:`str`: passed as Praat string value
    - :class:`numpy.ndarray`: passed as Praat vector or matrix, if the
      array contains numeric values and is 1D or 2D, respectively.

Keyword Arguments
-----------------
extra_objects: List[parselmouth.Data]
    Extra objects added to the Praat object list that will not be selected
    when the command is called (default value: ``[]``).
return_string: bool
    Return the rew string written in the Praat info window instead of the
    converted Python object (default value: ``False``).

Returns
-------
object
    The result of the Praat command. The actual value returned depends on
    what the Praat command does. The following types can be returned:

    - If ``return_string=True`` was passed, a `str` value is returned,
      which contains the text that would have been written to the Praat
      info window.
    - A :class:`float`, :class:`int`, :class:`bool`, or :class:`complex`
      value is returned when the Praat command would write such a value to
      the Praat info window.
    - A :class:`numpy.ndarray` value is returned if the command returns a
      Praat vector or matrix.
    - A :class:`parselmouth.Data` object is returned if the command always
      creates exactly one object. If the actual type of the Praat object is
      known as a class in Parselmouth, an object of a subtype of
      :class:`parselmouth.Data` is returned.
    - A list of :class:`parselmouth.Data` is returned if the command can
      create multiple new objects (even if this particular execution of the
      command only added one object to the Praat object list).
    - A :class:`str` is returned when a string or info text would be
      written to the Praat info window.
)");

	def("run",
	    [](const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromText({}, script, args, kwargs); },
	    "script"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run",
	    [](structData &data, const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromText({ std::ref(data) }, script, args, kwargs); },
	    "object"_a, "script"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run",
	    &runPraatScriptFromText,
	    "objects"_a, "script"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    [](const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromFile({}, script, args, kwargs); },
	    "path"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    [](structData &data, const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromFile({ std::ref(data) }, script, args, kwargs); },
	    "object"_a, "path"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    &runPraatScriptFromFile,
	    "objects"_a, "path"_a,
	    "Keyword arguments:\n    - extra_objects: List[parselmouth.Data] = []\n    - capture_output: bool = False\n    - return_variables: bool = False");


	def("_get_actions",
	    []() {
		    std::vector<CastedPraatCommand> actions;
		    for (integer i = 1; i <= praat_getNumberOfActions(); ++i)
			    actions.emplace_back(castPraatCommand(*praat_getAction(i)));
		    return actions;
	    });

	def("_get_menu_commands",
	    []() {
		    std::vector<CastedPraatCommand> menuCommands;
		    for (integer i = 1; i <= praat_getNumberOfMenuCommands(); ++i)
				menuCommands.emplace_back(castPraatCommand(*praat_getMenuCommand(i)));
		    return menuCommands;
	    });
}

} // namespace parselmouth
