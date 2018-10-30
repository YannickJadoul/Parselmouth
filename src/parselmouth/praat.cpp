/*
 * Copyright (C) 2018  Yannick Jadoul
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

py::object autonumvecToArray(autonumvec &&vector) {
	if (!vector.at)
		return py::none();

	auto [size, at] = std::make_tuple(vector.size, vector.at); // Because of undefined order of evaluation of arguments, we need to make sure to save size and at before moving
	auto capsule = py::capsule(std::make_unique<autonumvec>(std::move(vector)).release(), [](void *v) { delete reinterpret_cast<autonumvec *>(v); });
	return py::array_t<double>(static_cast<size_t>(size), &at[1], capsule);
}

py::object autonummatToArray(autonummat &&matrix) {
	if (!matrix.at)
		return py::none();

	auto [nrow, ncol, at] = std::make_tuple(matrix.nrow, matrix.ncol, matrix.at); // Because of undefined order of evaluation of arguments, we need to make sure to save size and at before moving
	auto capsule = py::capsule(std::make_unique<autonummat>(std::move(matrix)).release(), [](void *m) { delete reinterpret_cast<autonummat *>(m); });
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

		assert(m_objects->n == 0);
	}

	auto interpreter() const {
		return m_interpreter.get();
	}

	void addObjects(const std::vector<std::reference_wrapper<structData>> &objects) {
		// Add references to the passed objects to the Praat object list
		for (auto &data: objects)
			praat_newReference(&data.get()); // Since we're registering this is just a reference, running a command like "Remove" should normally be OK; through hack/workaround: a PraatObject now contains a boolean 'owned' to know if the data should be deleted
		praat_updateSelection();
		m_lastId = m_objects->uniqueId;
	}

	std::vector<autoData> retrieveSelectedObjects(bool assertNew = false) {
		[[maybe_unused]] auto numSelected = static_cast<size_t>(m_objects->totalSelection);

		std::vector<autoData> selected;
		for (auto i = 1; i <= m_objects->n; ++i) {

			auto &praatObject = m_objects->list[i];
			if (praatObject.isSelected) {
				if (assertNew)
					assert(praatObject.id > m_lastId);
				praat_deselect(i); // Hack/workaround: if this is not called, Praat will call it while removing the object from the list, and crash on accessing object -> classInfo
				selected.emplace_back(praatObject.object);
				praatObject.object = nullptr;
			}
		}

		assert(m_objects->totalSelection == 0);
		assert(numSelected == selected.size());

		return selected;
	}

	std::unordered_map<std::u32string, py::object> getVariables() {
		std::unordered_map<std::u32string, py::object> variables;

		for (const auto &[name, value] : m_interpreter->variablesMap) {
			assert(name == value->string);

			if (name.length() > 0 && name.back() == U'$') {
				variables.emplace(name, py::cast(value->stringValue));
			}
			else if (name.length() > 1 && name.substr(name.length() - 2) == U"##") {
				// Steal matrix value, so clear out old variable to stop Praat from deleting when cleaning up the interpreter
				variables.emplace(name, autonummatToArray(autonummat(value->numericMatrixValue)));
				value->numericMatrixValue = empty_nummat;
			}
			else if (name.length() > 0 && name.back() == U'#') {
				// Steal vector value, so clear out old variable to stop Praat from deleting when cleaning up the interpreter
				variables.emplace(name, autonumvecToArray(autonumvec(value->numericVectorValue)));
				value->numericVectorValue = empty_numvec;
			}
			else {
				variables.emplace(name, py::cast(value->numericValue));
			}
		}

		return variables;
	}

	structStackel toPraatArg(const py::handle &arg);

	std::vector<structStackel> toPraatArgs(const py::args &args) {
		std::vector<structStackel> praatArgs(1); // Cause ... Praat, and 1-based indexing, and ... grmbl ... well, at least the .data() pointer of the std::vector cannot be a nullptr now
		for (auto arg : args)
			praatArgs.emplace_back(toPraatArg(arg));
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
structStackel stackel(double number) { auto s = structStackel{Stackel_NUMBER, {}, false}; s.number = number; return s; }
structStackel stackel(bool boolean) { auto s = structStackel{Stackel_STRING, {}, true}; s.string = Melder_dup(boolean ? U"yes" : U"no"); return s; }
structStackel stackel(const std::u32string &string) { auto s = structStackel{Stackel_STRING, {}, true}; s.string = Melder_dup(string.c_str()); return s; }
structStackel stackel(const numvec &vector, bool owned) { auto s = structStackel{Stackel_NUMERIC_VECTOR, {}, owned}; s.numericVector = vector; return s; }
structStackel stackel(const nummat &matrix, bool owned) { auto s = structStackel{Stackel_NUMERIC_MATRIX, {}, owned}; s.numericMatrix = matrix; return s; }

structStackel PraatEnvironment::toPraatArg(const py::handle &arg) {
	if (py::isinstance<py::int_>(arg) || py::isinstance<py::float_>(arg))
		return stackel(py::cast<double>(arg));
	else if (py::isinstance<py::bool_>(arg))
		return stackel(py::cast<bool>(arg));
	else if (py::isinstance<py::str>(arg) && (PY_MAJOR_VERSION < 3 || !py::isinstance<py::bytes>(arg)))
		return stackel(py::cast<std::u32string>(arg));

	try {
		// Let's not 'squeeze' the array, or we might interpret a 2D matrix as 1D vector!
		auto array = py::array_t<double, py::array::c_style>::ensure(arg);
		if (array.ndim() == 1) {
			// Keep the object alive until the PraatEnvironment goes out of scope, and avoid a copy
			m_keepAliveObjects.push_back(array);

			return stackel(numvec(array.mutable_data(0) - 1, array.shape(0)), false); // Remember Praat is 1-based
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

			return stackel(nummat(rows - 1, array.shape(0), array.shape(1)), false);
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
		return autonumvecToArray(std::move(theInterpreterNumvec));

	if (startsWith(callbackName, U"NUMMAT_"))
		return autonummatToArray(std::move(theInterpreterNummat));

	if (startsWith(callbackName, U"NEW_") ||
	    startsWith(callbackName, U"NEW1_") ||
	    startsWith(callbackName, U"NEW2_") ||
	    startsWith(callbackName, U"NEWMANY_") ||
	    startsWith(callbackName, U"NEWTIMES2_") ||
	    startsWith(callbackName, U"READ1_") ||
	    startsWith(callbackName, U"READMANY_")) {

		auto selected = retrieveSelectedObjects(true);

		if (selected.size() == 1 && !(startsWith(callbackName, U"NEWMANY_") || startsWith(callbackName, U"READMANY_")))
			return py::cast(std::move(selected[0]));
		else
			return py::cast(std::move(selected));
	}

	if (!interceptedInfo.empty()) // U"HINT_", U"INFO_", U"LIST_"
		return py::cast(interceptedInfo);

	// TODO U"GRAPHICS_" and U"MOVIE_" crashes

	// U"HELP_", U"MODIFY_", U"PRAAT_", U"PREFS_", U"SAVE_"
	return py::none();

	// Weird: U"BUG_", U"DANGEROUS_"
	// Impossible: U"PLAY_", U"RECORD1_", U"WINDOW_"
	// Does nothing: U"GRAPHICS_"
	// Throws exception: U"MOVIE_"
}


auto callPraatCommand(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &command, py::args args, py::kwargs kwargs) {
	auto returnString = extractKwarg<bool, py::bool_>(kwargs, "return_string", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;
	environment.addObjects(objects);
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
	auto captureOutput = extractKwarg<bool, py::bool_>(kwargs, "capture_output", false, "bool");
	auto returnVariables = extractKwarg<bool, py::bool_>(kwargs, "return_variables", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;
	environment.addObjects(objects);
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

	return runPraatScript(objects, fullScript.peek(), std::move(args), std::move(kwargs));
}

auto runPraatScriptFromFile(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &path, py::args args, py::kwargs kwargs) {
	auto file = pathToMelderFile(path);
	autostring32 script = MelderFile_readText(&file);

	{
		autoMelderFileSetDefaultDir dir(&file);
		Melder_includeIncludeFiles(&script);
	}

	return runPraatScript(objects, script.peek(), std::move(args), std::move(kwargs));
}

} // namespace

class PraatModule;

PRAAT_MODULE_BINDING(praat, PraatModule) {
	def("call",
	    [](const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({}, command, args, kwargs); },
	    "command"_a,
	    "Keyword arguments:\n    - return_string: bool = False");

	def("call",
	    [](structData &data, const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({ std::ref(data) }, command, args, kwargs); },
	    "object"_a, "command"_a,
	    "Keyword arguments:\n    - return_string: bool = False");

	def("call",
	    &callPraatCommand,
	    "objects"_a, "command"_a,
	    "Keyword arguments:\n    - return_string: bool = False");

	def("run",
	    [](const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromText({}, script, args, kwargs); },
	    "script"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run",
	    [](structData &data, const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromText({ std::ref(data) }, script, args, kwargs); },
	    "object"_a, "script"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run",
	    &runPraatScriptFromText,
	    "objects"_a, "script"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    [](const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromFile({}, script, args, kwargs); },
	    "path"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    [](structData &data, const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScriptFromFile({ std::ref(data) }, script, args, kwargs); },
	    "object"_a, "path"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

	def("run_file",
	    &runPraatScriptFromFile,
	    "objects"_a, "path"_a,
	    "Keyword arguments:\n    - capture_output: bool = False\n    - return_variables: bool = False");

#ifndef NDEBUG // TODO Only in debug?
	auto castPraatCommand = [](const structPraat_Command &command) {
		return std::make_tuple(command.name, command.nameOfCallback);
	};

	using CastedPraatCommand = decltype(castPraatCommand(std::declval<structPraat_Command&>()));

	def("_get_actions",
	    [castPraatCommand]() {
		    std::vector<CastedPraatCommand> actions;
		    for (integer i = 1; i <= praat_getNumberOfActions(); ++i)
			    actions.emplace_back(castPraatCommand(*praat_getAction(i)));
		    return actions;
	    });

	def("_get_menu_commands",
	    [castPraatCommand]() {
		    std::vector<CastedPraatCommand> menuCommands;
		    for (integer i = 1; i <= praat_getNumberOfMenuCommands(); ++i)
		     menuCommands.emplace_back(castPraatCommand(*praat_getMenuCommand(i)));
		    return menuCommands;
	    });
#endif
}

} // namespace parselmouth
