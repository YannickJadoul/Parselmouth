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

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "fon/Formant.h" // TODO "" vs <> for Praat imports?
#include "fon/Manipulation.h"
#include "dwsys/NUMmachar.h"
#include "dwtools/Spectrogram_extensions.h"
#include "sys/praat.h"
#include "sys/praat_version.h"

#define XSTR(s) STR(s)
#define STR(s) #s

// TODO What to do with NUMundefined?

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

enum class Interpolation // TODO Remove/move to header
{
	NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
	LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
	CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
	SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
	SINC700 = Vector_VALUE_INTERPOLATION_SINC700
};

}

using namespace parselmouth;


PYBIND11_MODULE(parselmouth, m) {
	praatlib_init();

	parselmouth::PraatBindings bindings(m);

    static py::exception<MelderError> melderErrorException(m, "PraatError", PyExc_RuntimeError);
	py::register_exception_translator([](std::exception_ptr p) {
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const MelderError &) {
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

	bindings.get<MFCC>()
		//.def(constructor(&Sound_to_MFCC,
		//		(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

		.def("get_coefficients",
				[] (MFCC mfcc)
					{
						auto maxCoefficients = CC_getMaximumNumberOfCoefficients(mfcc, 1, mfcc->nx);
						py::array_t<double> array({static_cast<size_t>(mfcc->nx), static_cast<size_t>(maxCoefficients + 1)}, nullptr);

						for (auto i = 0; i < mfcc->nx; ++i) {
							*array.mutable_data(i, 0) = mfcc->frame[i+1].c0;
							for (auto j = 1; j <= maxCoefficients; ++j) {
								*array.mutable_data(i, j) = (j <= mfcc->frame[i+1].numberOfCoefficients) ? mfcc->frame[i+1].c[j] : std::numeric_limits<double>::quiet_NaN();
							}
						}

						return array;
					})
		.def("to_mel_spectrogram",
				&MFCC_to_MelSpectrogram,
				"from_coefficient"_a = 0, "to_coefficient"_a = 0, "include_c0"_a = true);
	;


	py::enum_<kPitch_unit>(m, "PitchUnit")
		.value("hertz", kPitch_unit_HERTZ)
		.value("hertz_logarithmic", kPitch_unit_HERTZ_LOGARITHMIC)
		.value("mel", kPitch_unit_MEL)
		.value("log_hertz", kPitch_unit_LOG_HERTZ)
		.value("mel", kPitch_unit_MEL)
		.value("semitones_1", kPitch_unit_SEMITONES_1)
		.value("semitones_100", kPitch_unit_SEMITONES_100)
		.value("semitones_200", kPitch_unit_SEMITONES_200)
		.value("semitones_440", kPitch_unit_SEMITONES_440)
		.value("erb", kPitch_unit_ERB)
	;

	bindings.get<Pitch>()
		.def("get_value",
				[] (Pitch self, double time, kPitch_unit unit, bool interpolate) { return Pitch_getValueAtTime(self, time, unit, interpolate); },
				"time"_a, "unit"_a = kPitch_unit_HERTZ, "interpolate"_a = true)
	;


	bindings.get<Intensity>()
		.def("get_value", // TODO Should be part of Vector class
				[] (Intensity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;


	bindings.get<Harmonicity>()
		.def("get_value", // TODO Should be part of Vector class
				[] (Harmonicity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;
}
