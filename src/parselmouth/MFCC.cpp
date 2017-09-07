/*
 * Copyright (C) 2017  Yannick Jadoul
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

#include "Parselmouth.h"

#include <pybind11/numpy.h>

#include <praat/dwtools/Spectrogram_extensions.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<MFCC>::init() {
	//def(constructor(&Sound_to_MFCC,
	//		(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

	def("get_coefficients",
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
	    });

	def("to_mel_spectrogram",
	    &MFCC_to_MelSpectrogram,
	    "from_coefficient"_a = 0, "to_coefficient"_a = 0, "include_c0"_a = true);
}

} // namespace parselmouth
