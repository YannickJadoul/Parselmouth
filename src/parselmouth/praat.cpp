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
#include "utils/pybind11/Optional.h"

#include <praat/sys/praat.h>
#include <praat/sys/praatP.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <cassert>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

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

class PraatEnvironment {
public:
	PraatEnvironment() : m_objects(theCurrentPraatObjects), m_interpreter(Interpreter_create(nullptr, nullptr)) {
		assert(m_objects->n == 0);
		m_objects->uniqueId = 0;
	}

	~PraatEnvironment() {
		for (auto i = m_objects->n; i > 0; --i) {
			praat_removeObject(i);
		}

		assert(m_objects->n == 0);
	}

	auto operator*() {
		return m_objects;
	}

	auto operator->() {
		return m_objects;
	}

	auto objects() const {
		return m_objects;
	}

	auto interpreter() const {
		return m_interpreter.get();
	}

private:
	// Let's not trust the combination of Praat and static initialization order to safely have a static autoInterpreter member

	PraatObjects m_objects;
	autoInterpreter m_interpreter;
};

// Workarounds since GCC (6) doesn't seem to like the brace initialization of the nested anonymous struct
structStackel stackel(double number) { auto s = structStackel{Stackel_NUMBER, {}, false}; s.number = number; return s; }
structStackel stackel(bool boolean) { auto s = structStackel{Stackel_STRING, {}, true}; s.string = Melder_dup(boolean ? U"yes" : U"no"); return s; }
structStackel stackel(const std::u32string &string) { auto s = structStackel{Stackel_STRING, {}, true}; s.string = Melder_dup(string.c_str()); return s; }
structStackel stackel(const numvec &vector, bool owned) { auto s = structStackel{Stackel_NUMERIC_VECTOR, {}, owned}; s.numericVector = vector; return s; }
structStackel stackel(const nummat &matrix, bool owned) { auto s = structStackel{Stackel_NUMERIC_MATRIX, {}, owned}; s.numericMatrix = matrix; return s; }

structStackel castPythonToPraat(const py::handle &arg) {
	if (py::isinstance<py::int_>(arg) || py::isinstance<py::float_>(arg))
		return stackel(py::cast<double>(arg));
	else if (py::isinstance<py::bool_>(arg))
		return stackel(py::cast<bool>(arg));
	else if (py::isinstance<py::str>(arg) && (PY_MAJOR_VERSION < 3 || !py::isinstance<py::bytes>(arg)))
		return stackel(py::cast<std::u32string>(arg));

	try {
		auto array = py::array_t<double, py::array::c_style>::ensure(py::cast<py::array>(arg).squeeze());
		if (array.ndim() == 1) {
			// TODO Check when we can avoid copying -- but the call to 'ensure' might have just done that copy, so can we maybe keep "array" alive until after the call into Praat?
			auto vector = numvec(array.shape(0), kTensorInitializationType::RAW);
			auto unchecked = array.unchecked<1>();
			for (ssize_t i = 0; i < array.shape(0); ++i)
				vector[i + 1] = unchecked(i);

			return stackel(vector, true);
		} else if (array.ndim() == 2) {
			// TODO Check when we can avoid copying -- not easy, as Praat NUMmatrix consists of two allocs (1 T** + 1 T*)
			auto matrix = nummat(array.shape(0), array.shape(1), kTensorInitializationType::RAW);
			auto unchecked = array.unchecked<2>();
			for (ssize_t i = 0; i < array.shape(0); ++i)
				for (ssize_t j = 0; i < array.shape(1); ++j)
					matrix[i + 1][j + 1] = unchecked(i, j);

			return stackel(matrix, true);
		}
	}
	catch (py::cast_error &) {}
	catch (py::error_already_set &) {}

	throw py::value_error("Cannot convert argument \"" + py::cast<std::string>(py::repr(arg)) + "\" to a known Praat argument type");
}

py::object castPraatResultToPython(const std::u32string &callbackName, PraatObjects praatObjects, const std::u32string &interceptedInfo, size_t nInitialObjects) {
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

	if (startsWith(callbackName, U"NUMVEC_")) {
		if (!theInterpreterNumvec.at)
			return py::none();

		auto vector = new autonumvec(theInterpreterNumvec.move());
		return py::array_t<double>(static_cast<size_t>(vector->size), &vector->at[1], py::capsule(vector, [](void *v) { delete reinterpret_cast<autonumvec *>(v); }));
	}

	if (startsWith(callbackName, U"NUMMAT_")) {
		if (!theInterpreterNummat.at)
			return py::none();

		auto matrix = new autonummat(theInterpreterNummat.move());
		return py::array_t<double, py::array::c_style>({static_cast<size_t>(matrix->nrow), static_cast<size_t>(matrix->ncol)}, &matrix->at[1][1], py::capsule(matrix, [](void *m) { delete reinterpret_cast<autonummat *>(m); }));
	}

	if (startsWith(callbackName, U"NEW_") ||
	    startsWith(callbackName, U"NEW1_") ||
	    startsWith(callbackName, U"NEW2_") ||
	    startsWith(callbackName, U"NEWMANY_") ||
	    startsWith(callbackName, U"NEWTIMES2_") ||
	    startsWith(callbackName, U"READ1_") ||
	    startsWith(callbackName, U"READMANY_")) {
		auto numSelected = static_cast<size_t>(praatObjects->totalSelection);

		std::vector<autoData> selected;
		for (auto i = 1; i <= praatObjects->n; ++i) {
			auto &praatObject = praatObjects->list[i];
			if (praatObject.isSelected) {
				assert(praatObject.id > static_cast<integer>(nInitialObjects));
				praat_deselect(i); // Hack/workaround: if this is not called, Praat will call it while removing the object from the list, and crash on accessing object -> classInfo
				selected.emplace_back(praatObject.object);
				praatObject.object = nullptr;
			}
		}

		assert(praatObjects->totalSelection == 0);
		assert(numSelected == selected.size());

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
}

auto callPraatCommand(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &command, py::args args, py::kwargs kwargs) {
	bool returnString = extractKwarg<bool, py::bool_>(kwargs, "return_string", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;

	// Add references to the passed objects to the Praat object list
	for (auto &data: objects)
		praat_newReference(&data.get()); // Since we're registering this is just a reference, running a command like "Remove" should normally be OK; through hack/workaround: a PraatObject now contains a boolean 'owned' to know if the data should be deleted
	praat_updateSelection();

	// Convert other arguments to Praat Stackels
	std::vector<structStackel> praatArgs(1); // Cause ... Praat, and 1-based indexing, and ... grmbl ... well, at least the .data() pointer of the std::vector cannot be a nullptr now
	for (auto &arg : args)
		praatArgs.emplace_back(castPythonToPraat(arg));

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
	else if (auto i = praat_doMenuCommand(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), environment.interpreter())) {
		executedCommand = praat_getMenuCommand(i);
	}
	else {
		Melder_throw(U"Command \"", command.c_str(), U"\" not available for given objects.");
	}

	// Based on the prefix of the command's callback, convert the result to a Python object
	assert(executedCommand);
	if (returnString)
		return py::cast(interceptor.get());
	else
		return castPraatResultToPython(executedCommand->nameOfCallback, environment.objects(), interceptor.get(), objects.size());
}

auto runPraatScript(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &script, py::args args, py::kwargs kwargs) {
	auto captureOutput = extractKwarg<bool, py::bool_>(kwargs, "capture_output", false, "bool");
	checkUnkownKwargs(kwargs);

	PraatEnvironment environment;

	// TODO Get rid of code duplication!
	// Add references to the passed objects to the Praat object list
	for (auto &data: objects)
		praat_newReference(&data.get()); // Since we're registering this is just a reference, running a command like "Remove" should normally be OK; through hack/workaround: a PraatObject now contains a boolean 'owned' to know if the data should be deleted
	praat_updateSelection();

	// Convert other arguments to Praat Stackels
	std::vector<structStackel> praatArgs(1); // Cause ... Praat, and 1-based indexing, and ... grmbl ... well, at least the .data() pointer of the std::vector cannot be a nullptr now
	for (auto &arg : args)
		praatArgs.emplace_back(castPythonToPraat(arg));

	// TODO Somehow does not intercept everything for praat/test/fon/fourier.praat
	// Prepare to intercept the output of the command
	auto interceptor = captureOutput ? std::make_unique<MelderInfoInterceptor>() : nullptr;

	try {
		auto fullScript = autostring32(Melder_dup(script.c_str()));
		Melder_includeIncludeFiles(&fullScript);

		Interpreter_readParameters(environment.interpreter(), fullScript.peek());
		Interpreter_getArgumentsFromArgs (environment.interpreter(), static_cast<int>(praatArgs.size() - 1), praatArgs.data());
		Interpreter_run(environment.interpreter(), fullScript.peek());
	} catch (MelderError) {
		Melder_throw(U"Script not completed.");
	}

	return captureOutput ? make_optional(interceptor->get()) : nullopt;
}

} // namespace

void initPraatModule(py::module m) {
	m.def("call",
	      &callPraatCommand,
	      "objects"_a, "command"_a,
	      "Keyword arguments:\n    - return_string: bool = False");

	m.def("call",
	      [](structData &data, const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({ std::ref(data) }, command, args, kwargs); },
	      "object"_a, "command"_a,
	      "Keyword arguments:\n    - return_string: bool = False");

	m.def("call",
	      [](const std::u32string &command, py::args args, py::kwargs kwargs) { return callPraatCommand({}, command, args, kwargs); },
	      "command"_a,
	      "Keyword arguments:\n    - return_string: bool = False");

	m.def("run",
	      &runPraatScript,
	      "objects"_a, "script"_a,
	      "Keyword arguments:\n    - capture_output: bool = False");

	m.def("run",
	      [](structData &data, const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScript({ std::ref(data) }, script, args, kwargs); },
	      "object"_a, "script"_a,
	      "Keyword arguments:\n    - capture_output: bool = False");

	m.def("run",
	      [](const std::u32string &script, py::args args, py::kwargs kwargs) { return runPraatScript({}, script, args, kwargs); },
	      "script"_a,
	      "Keyword arguments:\n    - capture_output: bool = False");

#ifndef NDEBUG // TODO Only in debug?
	auto castPraatCommand = [](const structPraat_Command &command) {
		return std::make_tuple(command.name, command.nameOfCallback);
	};

	using CastedPraatCommand = decltype(castPraatCommand(std::declval<structPraat_Command&>()));

	m.def("_get_actions",
	      [castPraatCommand]() {
		      std::vector<CastedPraatCommand> actions;
		      for (integer i = 1; i <= praat_getNumberOfActions(); ++i)
			      actions.emplace_back(castPraatCommand(*praat_getAction(i)));
		      return actions;
	      });

	m.def("_get_menu_commands",
	      [castPraatCommand]() {
		      std::vector<CastedPraatCommand> menuCommands;
		      for (integer i = 1; i <= praat_getNumberOfMenuCommands(); ++i)
			      menuCommands.emplace_back(castPraatCommand(*praat_getMenuCommand(i)));
		      return menuCommands;
	      });
#endif
}

} // namespace parselmouth
