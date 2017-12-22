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
	if (praatObjects->totalSelection == 1) {
		for (auto i = praatObjects->n; i > static_cast<int>(nInitialObjects); --i) {
			auto &praatObject = praatObjects->list[i];
			if (praatObject.isSelected)
				return py::cast(Data_copy(praatObject.object)); // TODO Can we steal this instead of copying?
		}
	}

	return py::none();
}

auto callPraat(const std::vector<std::reference_wrapper<structData>> &objects, const std::u32string &command, py::args args) {
	auto praatObjects = theCurrentPraatObjects;

	assert(praatObjects->n == 0);
	praatObjects->uniqueId = 0;

	for (auto &data: objects)
		praat_new(Data_copy(&data.get())); // TODO Copy? What about modifications to the original object??
	praat_updateSelection();
	praat_show();

	std::vector<structStackel> praatArgs(1); // Cause ... Praat, and 1-based indexing, and ... grmbl ... well, at least the .data() pointer of the std::vector cannot be a nullptr now
	for (auto &arg : args)
		praatArgs.emplace_back(castPythonToPraat(arg));

	auto completedCommand = command;
	if (args.size() > 0 && (command.size() < 3 || command.substr(command.size() - 3, 3) != U"..."))
		completedCommand += U"...";

	MelderInfoInterceptor interceptor;

	if (!praat_doAction(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), nullptr) &&
	    !praat_doMenuCommand(completedCommand.c_str(), static_cast<int>(praatArgs.size() - 1), praatArgs.data(), nullptr))
		Melder_throw(U"Command \"", command.c_str(), U"\" not available for given objects.");

	for (auto i = 1; i <= praatObjects->n; ++i) {
		auto &praatObject = praatObjects->list[i];
		if (static_cast<size_t>(praatObject.id) <= objects.size()) {
			auto oldData = &objects[praatObject.id - 1].get();
			if (!Data_equal(oldData, praatObject.object)) {
				oldData->v_destroy();
				praatObject.object->v_copy(oldData);
			}
		}
	}

	auto result = castPraatResultToPython(U"", praatObjects, interceptor.get(), objects.size());

	for (auto i = praatObjects->n; i > 0; --i) {
		praat_removeObject(i);
	}

	assert(praatObjects->n == 0);

	return result;

	/*objects->n = 1; // TODO praat_MAXNUM_OBJECTS
	objects->totalSelection = 1;
	objects->numberOfSelected[data->classInfo->sequentialUniqueIdOfReadableClass] = 1;
	objects->totalBeingCreated = 0;
	++objects->uniqueId;

	auto &object = objects->list[1];
	object.klas = data->classInfo;
	object.object = Data_copy(data).releaseToAmbiguousOwner(); // TODO What about "Remove", otherwise? And what about ugly stuff like "Quit"?
	autoMelderString name;
	MelderString_append(&name, data->classInfo->className, U" ", data->name && data->name[0] ? data->name : U"untitled");
	object.name = Melder_dup_f(name.string);
	MelderFile_setToNull(&object.file);
	object.id = objects->uniqueId;
	object.isSelected = true;
	for (auto &editor : object.editors)
		editor = nullptr;
	object.isBeingCreated = false;

	auto previousObjects = theCurrentPraatObjects;
	theCurrentPraatObjects = objects.get();

	praat_show();*/
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
