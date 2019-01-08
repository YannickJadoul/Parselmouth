/*
 * Copyright (C) 2016-2019  Yannick Jadoul
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

#include <praat/sys/praat.h>
#include <praat/sys/praat_version.h>

#define XSTR(s) STR(s)
#define STR(s) #s

namespace py = pybind11;
using namespace py::literals;
using namespace std::string_literals;

Thing_declare(CC);
Thing_declare(Daata);
Thing_declare(Formant);
Thing_declare(Function);
Thing_declare(Harmonicity);
Thing_declare(Harmonicity);
Thing_declare(Intensity);
Thing_declare(Matrix);
Thing_declare(MFCC);
Thing_declare(Pitch);
Thing_declare(Sampled);
Thing_declare(SampledXY);
Thing_declare(Sound);
Thing_declare(Spectrum);
Thing_declare(Spectrogram);
Thing_declare(Vector);

using Data = Daata;

namespace parselmouth {

class PraatModule;
using PraatError = MelderError;
class PraatWarning {};
class PraatFatal {};

PRAAT_EXCEPTION_BINDING(PraatError, PyExc_RuntimeError) {
	// Exception translators need to be convertible to void (*) (std::exception_ptr), so we cannot capture and store *this in the lambda.
	static auto exception = *this;
	py::register_exception_translator([](std::exception_ptr p) {
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const MelderError &) {
				// Python 2: Seems exception strings should be encoded in UTF-8
				// Python 3: PyErr_SetString (in py::exception<type>::operator()) decodes from UTF-8
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1); // Remove closing newline
				Melder_clearError();
				exception(message.c_str());
			}
	});
}

PRAAT_EXCEPTION_BINDING(PraatWarning, PyExc_UserWarning) {
	static auto warning = *this;
	Melder_setWarningProc([](const char32 *message) {
			if (PyErr_WarnEx(warning.ptr(), Melder_peek32to8(message), 1) < 0)
				throw py::error_already_set();
	});
}

PRAAT_EXCEPTION_BINDING(PraatFatal, PyExc_BaseException) {
	static auto fatal = *this;
	Melder_setFatalProc([](const char32 *message) {
			auto extraMessage = "Parselmouth intercepted a fatal error in Praat:\n\n"s +
			                    Melder_peek32to8(message) + "\n"s +
			                    "To ensure correctness of Praat's calculations, it is advisable to NOT ignore this error\n"s
			                    "and to RESTART Python before using more of Praat's functionality through Parselmouth."s;
			fatal(extraMessage.c_str());
			throw py::error_already_set();
	});
}

void redirectMelderInfo() {
	Melder_setInformationProc([](const char32 *message, size_t i) {
		auto sys = py::module::import("sys");
		auto sys_stdout = sys.attr("stdout");
		sys_stdout.attr("write")(&message[i]);
		sys_stdout.attr("flush")();
	});
}

void redirectMelderError() {
	Melder_setErrorProc([](const char32 *message) {
		auto sys = py::module::import("sys");
		auto sys_stderr = sys.attr("stderr");
		sys_stderr.attr("write")(message);
		sys_stderr.attr("flush")();
	});

}

using PraatBindings = Bindings<PraatError,
                               PraatWarning,
                               PraatFatal,
                               Interpolation,
                               WindowShape,
                               AmplitudeScaling,
                               SignalOutsideTimeDomain,
                               SoundFileFormat,
                               SpectralAnalysisWindowShape,
                               FormantUnit,
                               PitchUnit,
                               Thing,
                               Data,
                               Function,
                               Sampled,
                               SampledXY,
                               Matrix,
                               Vector,
                               Sound,
                               Spectrum,
                               Spectrogram,
                               Pitch,
                               Intensity,
                               Harmonicity,
                               Formant,
                               CC,
                               MFCC,
                               PraatModule>;

}

PYBIND11_MODULE(parselmouth, m) {
	praatlib_init();
	// TODO Put in one-time initialization that is run when it's actually needed?
	INCLUDE_LIBRARY(praat_uvafon_init)
	INCLUDE_LIBRARY(praat_contrib_Ola_KNN_init)

	parselmouth::PraatBindings bindings(m);

	m.attr("__version__") = PYBIND11_STR_TYPE(XSTR(PARSELMOUTH_VERSION));
	m.attr("VERSION") = py::str(XSTR(PARSELMOUTH_VERSION));
	m.attr("PRAAT_VERSION") = py::str(XSTR(PRAAT_VERSION_STR));
	m.attr("PRAAT_VERSION_DATE") = py::str(XSTR(PRAAT_DAY) " " XSTR(PRAAT_MONTH) " " XSTR(PRAAT_YEAR));

	parselmouth::redirectMelderInfo();
	parselmouth::redirectMelderError();

	bindings.init();
}
