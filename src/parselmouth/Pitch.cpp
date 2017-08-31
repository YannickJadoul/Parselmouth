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

#include "utils/pybind11/ImplicitStringToEnumConversion.h"
#include "utils/pybind11/NumericPredicates.h"
#include "utils/pybind11/Optional.h"

#include "fon/Pitch_to_Sound.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<PitchUnit>::init() {
	value("HERTZ", kPitch_unit_HERTZ);
	value("HERTZ_LOGARITHMIC", kPitch_unit_HERTZ_LOGARITHMIC);
	value("MEL", kPitch_unit_MEL);
	value("LOG_HERTZ", kPitch_unit_LOG_HERTZ); // TODO Huh? HERTZ_LOGARITHMIC and LOG_HERTZ!?
	value("MEL", kPitch_unit_MEL);
	value("SEMITONES_1", kPitch_unit_SEMITONES_1);
	value("SEMITONES_100", kPitch_unit_SEMITONES_100);
	value("SEMITONES_200", kPitch_unit_SEMITONES_200);
	value("SEMITONES_440", kPitch_unit_SEMITONES_440);
	value("ERB", kPitch_unit_ERB);

	make_implicitly_convertible_from_string(*this, true);
}

void Binding<Pitch>::init() {
	using signature_cast_placeholder::_;

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

	def("count_voiced_frames",
		&Pitch_countVoicedFrames);

	// TODO Get value at time..., Get value in frame...
	def("get_value", // TODO Cleanup temporary
	    [] (Pitch self, double time, kPitch_unit unit, bool interpolate) { return Pitch_getValueAtTime(self, time, unit, interpolate); },
	    "time"_a, "unit"_a = kPitch_unit_HERTZ, "interpolate"_a = true);
	// TODO Minimum, Time of minimum, Maximum, Time of maximum, ...
	// TODO Get mean absolute slope (+ without octave jumps)

	// TODO Formula

	// TODO To TextGrid..., To TextTier, To IntervalTier: depends TextGrid and Tiers
	// TODO To PointProcess: depends on PointProcess

	def("interpolate",
	    &Pitch_interpolate);

	def("smooth",
		signature_cast<_ (_, Positive<double>)>(Pitch_smooth),
		"bandwidth"_a = 10.0);

	def("subtract_linear_fit",
	    [] (Pitch self, PitchUnit unit) { return Pitch_subtractLinearFit(self, static_cast<int>(unit)); },
		"unit"_a = PitchUnit::kPitch_unit_HERTZ);

	def("kill_octave_jumps",
		&Pitch_killOctaveJumps);

	// TODO To PitchTier: depends on PitchTier

	// TODO To Matrix -> .values?
}

} // namespace parselmouth
