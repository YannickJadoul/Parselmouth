/*
 * Copyright (C) 2017-2022  Yannick Jadoul
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

#include "utils/SignatureCast.h"
#include "utils/pybind11/NumericPredicates.h"

#include <pybind11/numpy.h>

#include <praat/dwtools/MFCC.h>
#include <praat/dwtools/Spectrogram_extensions.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(MFCC) {
	using signature_cast_placeholder::_;

	// TODO Constructor from Sound? Other constructors?

	// TODO To MelSpectrogram..., To TableOfReal... (? -> pandas?),

	/*def("to_mel_spectrogram", // TODO Uncomment once we have a MelSpectrogram!
	    &MFCC_to_MelSpectrogram,
	    "from_coefficient"_a = 0, "to_coefficient"_a = 0, "include_c0"_a = true);*/

	def("to_matrix_features",
	    args_cast<_, Positive<_>, _>(MFCC_to_Matrix_features),
	    "window_length"_a = 0.025, "include_energy"_a = false);

	def("extract_features",
	    args_cast<_, Positive<_>, _>(MFCC_to_Matrix_features),
	    "window_length"_a = 0.025, "include_energy"_a = false);

	def("to_sound",
	    &MFCC_to_Sound);

	def("cross_correlate",
	    &MFCCs_convolve,
	    "other"_a.none(false), "scaling"_a = kSounds_convolve_scaling::PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain::ZERO);

	def("convolve",
	    &MFCCs_convolve,
	    "other"_a.none(false), "scaling"_a = kSounds_convolve_scaling::PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain::ZERO);
}

} // namespace parselmouth
