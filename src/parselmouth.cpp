/*
 * Copyright (C) 2016-2017  Yannick Jadoul
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
#include "version.h"

#include "praat/MelderUtils.h"
#include "utils/StringUtils.h"

#include <praat/sys/praat.h>
#include <praat/sys/praatP.h>
#include <praat/sys/praat_version.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#define XSTR(s) STR(s)
#define STR(s) #s

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

namespace {

class PraatEnvironmentGuard {
public:
	PraatEnvironmentGuard() : m_objects(theCurrentPraatObjects), m_interpreter(Interpreter_create(nullptr, nullptr)) {
		assert(m_objects->n == 0);
		m_objects->uniqueId = 0;
	}

	~PraatEnvironmentGuard() {
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

structStackel castPythonToPraat(const py::handle &arg) {
	if (py::isinstance<py::int_>(arg) || py::isinstance<py::float_>(arg)) {
		return {Stackel_NUMBER, .number=py::cast<double>(arg), false};
	} else if (py::isinstance<py::bool_>(arg)) {
		return {Stackel_STRING, .string = Melder_dup(py::cast<bool>(arg) ? U"yes" : U"no"), true};
	} else if (py::isinstance<py::str>(arg) && (PY_MAJOR_VERSION < 3 || !py::isinstance<py::bytes>(arg))) { // TODO Check for unicode/bytes / Python2/3 behaviours
		return {Stackel_STRING, .string = Melder_dup(py::cast<std::u32string>(arg).c_str()), true};
	}

	try {
		py::array_t<double> array = py::cast<py::array_t<double>>(arg).squeeze();
		if (array.ndim() == 1) {
			// TODO Check when we can avoid copying
			auto vector = numvec(array.shape(0), kTensorInitializationType::RAW);
			auto unchecked = array.unchecked<1>();
			for (ssize_t i = 0; i < array.shape(0); ++i)
				vector[i + 1] = unchecked(i); // TODO Make into copy utility function

			return {Stackel_NUMERIC_VECTOR, .numericVector = vector, true};
		} else if (array.ndim() == 2) {
			// TODO Check when we can avoid copying
			auto matrix = nummat(array.shape(0), array.shape(1), kTensorInitializationType::RAW);
			auto unchecked = array.unchecked<2>();
			for (ssize_t i = 0; i < array.shape(0); ++i)
				for (ssize_t j = 0; i < array.shape(1); ++j)
					matrix[j + 1][i + 1] = unchecked(i, j); // TODO Make into copy utility function

			return {Stackel_NUMERIC_MATRIX, .numericMatrix = matrix, true};
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
		auto vector = new autonumvec(theInterpreterNumvec.move());
		if (vector->at)
			return py::array_t<double>(static_cast<size_t>(vector->size), &vector->at[1], py::capsule(vector, [](void *v) { delete reinterpret_cast<autonumvec*>(v); }));
		else
			return py::none();
	}

	if (startsWith(callbackName, U"NUMMAT_")) {
		auto matrix = new autonummat(theInterpreterNummat.move());
		if (matrix->at) // TODO Check Row-major/column-major things
			return py::array_t<double, py::array::c_style>({ static_cast<size_t>(matrix->nrow), static_cast<size_t>(matrix->ncol) }, &matrix->at[1][1], py::capsule(matrix, [](void *m) { delete reinterpret_cast<autonummat*>(m); }));
		else
			return py::none();
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

auto callPraat(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &command, py::args args) {
	PraatEnvironmentGuard praatEnvironment;

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
	if (auto i = praat_doAction(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), praatEnvironment.interpreter())) {
		executedCommand = praat_getAction(i);
	}
	else if (auto i = praat_doMenuCommand(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), praatEnvironment.interpreter())) {
		executedCommand = praat_getMenuCommand(i);
	}
	else {
		Melder_throw(U"Command \"", command.c_str(), U"\" not available for given objects.");
	}

	// Based on the prefix of the command's callback, convert the result to a Python object
	assert(executedCommand);
	auto result = castPraatResultToPython(executedCommand->nameOfCallback, praatEnvironment.objects(), interceptor.get(), objects.size());

	return result;
}

}

void initPraatModule(py::module m) {
	m.def("call",
	      &callPraat,
	      "objects"_a, "command"_a);

	m.def("call",
	      [](structData &data, const std::u32string &command, py::args args) { return callPraat({ std::ref(data) }, command, args); },
	      "object"_a, "command"_a);

	m.def("call",
	      [](const std::u32string &command, py::args args) { return callPraat({}, command, args); },
	      "command"_a);

#ifndef NDEBUG
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


PYBIND11_MODULE(parselmouth, m) {
	praatlib_init();
	// TODO Put in one-time initialization that is run when it's actually needed?
    INCLUDE_LIBRARY(praat_uvafon_init)
    INCLUDE_LIBRARY(praat_contrib_Ola_KNN_init)

	parselmouth::PraatBindings bindings(m);

    static py::exception<MelderError> melderErrorException(m, "PraatError", PyExc_RuntimeError); // TODO Own file?
	py::register_exception_translator([](std::exception_ptr p) {
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const MelderError &) { // TODO Unicode encoding? Python 2 vs. Python 3?
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				melderErrorException(message.c_str());
			}});

	m.attr("__version__") = PYBIND11_STR_TYPE(XSTR(PARSELMOUTH_VERSION));
	m.attr("VERSION") = py::str(XSTR(PARSELMOUTH_VERSION));
	m.attr("PRAAT_VERSION") = py::str(XSTR(PRAAT_VERSION_STR));
	m.attr("PRAAT_VERSION_DATE") = py::str(XSTR(PRAAT_DAY) " " XSTR(PRAAT_MONTH) " " XSTR(PRAAT_YEAR));

	bindings.init();

	parselmouth::initPraatModule(m.def_submodule("praat")); // TODO Part of the Bindings, on the longer term?
}
