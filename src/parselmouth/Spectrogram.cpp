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

#include "utils/SignatureCast.h"
#include "utils/pybind11/NumericPredicates.h"

#include "fon/Sound_and_Spectrogram.h"
#include "fon/Spectrum_and_Spectrogram.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Spectrogram>::init() {
	using signature_cast_placeholder::_;

	// TODO Constructor!

	def("get_power_at",
	    [](Spectrogram self, double time, double frequency) { return Matrix_getValueAtXY(self, time, frequency); },
		"time"_a, "frequency"_a);

	def("to_spectrum_slice", // TODO Pythonic alias?
	    &Spectrogram_to_Spectrum,
	    "time"_a);

	def("synthesize_sound",
	    signature_cast<_ (_, Positive<double>)>(Spectrogram_to_Sound),
		"sampling_frequency"_a = 44100.0);

	def("to_sound",
	    signature_cast<_ (_, Positive<double>)>(Spectrogram_to_Sound),
	    "sampling_frequency"_a = 44100.0);


	// TODO fmin, fmax, fn, f1 ?

	// TODO Formula (in Matrix?)

	// TODO TimeFrameSampled
	// TODO TimeFunction
}

// TODO Spectrogram_extension: BandFilterSpectrogram, BarkSpectrogram, MelSpectrogram

} // namespace parselmouth
