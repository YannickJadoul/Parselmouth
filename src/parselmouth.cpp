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
Thing_declare(Matrix);
Thing_declare(Sound);
Thing_declare(Spectrum);
Thing_declare(Spectrogram);
Thing_declare(Vector);

using Data = Daata;

namespace parselmouth {

class PraatModule;
using PraatError = MelderError;

PRAAT_EXCEPTION_BINDING(PraatError, PyExc_RuntimeError) {
	static auto exception = *this;
	py::register_exception_translator([](std::exception_ptr p) mutable {
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
			}});
}

using PraatBindings = Bindings<PraatError,
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

	bindings.init();
}
