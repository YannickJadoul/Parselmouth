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
#include "TimeClassAspects.h"

#include "utils/pybind11/NumericPredicates.h"
#include "utils/pybind11/Optional.h"

#include "fon/Pitch_to_Sound.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Pitch>::init() {
	initTimeFrameSampled(*this);

	// TODO Which constructors? From Sound?

	def("to_sound_pulses",
		[](Pitch self, optional<double> fromTime, optional<double> toTime) { return Pitch_to_Sound(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), false); },
		"from_time"_a = nullopt, "to_time"_a = nullopt);

	def("to_sound_hum",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) { return Pitch_to_Sound(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), true); },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("to_sound_sine",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime, Positive<double> samplingFrequency, double roundToNearestZeroCrossing) { return Pitch_to_Sound_sine(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), samplingFrequency, roundToNearestZeroCrossing); },
	    "from_time"_a = nullopt, "to_time"_a = nullopt, "sampling_frequency"_a = 44100.0, "round_to_nearest_zero_crossing"_a = true);
}

} // namespace parselmouth
