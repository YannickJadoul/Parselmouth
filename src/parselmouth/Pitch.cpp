/*
 * Copyright (C) 2017-2019  Yannick Jadoul
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

#include "Interpolation.h"
#include "TimeClassAspects.h"

#include "praat/MelderUtils.h"
#include "utils/pybind11/ImplicitStringToEnumConversion.h"
#include "utils/pybind11/NumericPredicates.h"
#include "utils/pybind11/Optional.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <praat/fon/Matrix_and_Pitch.h>
#include <praat/fon/Pitch.h>
#include <praat/fon/Pitch_to_Sound.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_ENUM_BINDING(PitchUnit) {
	value("HERTZ", kPitch_unit::HERTZ);
	value("HERTZ_LOGARITHMIC", kPitch_unit::HERTZ_LOGARITHMIC);
	value("MEL", kPitch_unit::MEL);
	value("LOG_HERTZ", kPitch_unit::LOG_HERTZ); // TODO Huh? HERTZ_LOGARITHMIC and LOG_HERTZ!?
	value("SEMITONES_1", kPitch_unit::SEMITONES_1);
	value("SEMITONES_100", kPitch_unit::SEMITONES_100);
	value("SEMITONES_200", kPitch_unit::SEMITONES_200);
	value("SEMITONES_440", kPitch_unit::SEMITONES_440);
	value("ERB", kPitch_unit::ERB);

	make_implicitly_convertible_from_string(*this);
}

PRAAT_STRUCT_BINDING(Candidate, Pitch_Candidate) {
	def_readonly("frequency", &structPitch_Candidate::frequency); // TODO readwrite? Then we need to return references instead of copies in Pitch_Frame.
	def_readonly("strength", &structPitch_Candidate::strength);

	// TODO Reference to Pitch_Frame to have ".select()"?
}

PRAAT_STRUCT_BINDING(Frame, Pitch_Frame) {
	using PitchCandidate = structPitch_Candidate;
	PYBIND11_NUMPY_DTYPE(PitchCandidate, frequency, strength);

	def_readonly("intensity", &structPitch_Frame::intensity);

	def_property("selected",
	             [](Pitch_Frame self) { return &self->candidate[1]; },
	             [](Pitch_Frame self, Pitch_Candidate candidate) {
		             for (long j = 1; j <= self->nCandidates; j++) {
			             if (&self->candidate[j] == candidate) {
				             std::swap(self->candidate[1], self->candidate[j]);
				             return;
			             }
		             }
		             throw py::value_error("'candidate' is not a Pitch Candidate of this frame");
	             });

	def_property_readonly("candidates", [](Pitch_Frame self) { return std::vector<structPitch_Candidate>(&self->candidate[1], &self->candidate[self->nCandidates + 1]); });

	def("unvoice",
	    [](Pitch_Frame self) {
		    for (long j = 1; j <= self->nCandidates; j++) {
			    if (self->candidate[j].frequency == 0.0) {
				    std::swap(self->candidate[1], self->candidate[j]);
				    break;
			    }
		    }
	    });

	def("select",
	    [](Pitch_Frame self, Pitch_Candidate candidate) {
		    for (long j = 1; j <= self->nCandidates; j++) {
			    if (self->candidate[j].frequency == candidate->frequency && self->candidate[j].strength == candidate->strength) {
				    std::swap(self->candidate[1], self->candidate[j]);
				    return;
			    }
		    }
		    throw py::value_error("'candidate' is not a Pitch Candidate of this frame");
	    },
	    "candidate"_a.none(false));

	def("select",
	    [](Pitch_Frame self, long i) {
		    if (i < 0) i += self->nCandidates; // Python-style negative indexing
		    if (i < 0 || i >= self->nCandidates) throw py::index_error("Pitch Frame index out of range");
		    return std::swap(self->candidate[1], self->candidate[i+1]);
	    },
	    "i"_a);

	def("__getitem__",
	    [](Pitch_Frame self, long i) {
		    if (i < 0) i += self->nCandidates; // Python-style negative indexing
		    if (i < 0 || i >= self->nCandidates) throw py::index_error("Pitch Frame index out of range");
		    return self->candidate[i+1]; // Not a(n) (internal) reference, because unvoice and select would then change the value of a returned Pitch_Candidate
	    },
	    "i"_a);

	def("__len__",
	    [](Pitch_Frame self) { return self->nCandidates; });

	def("as_array", [](Pitch_Frame self) { return py::array(self->nCandidates, &self->candidate[1], py::cast(self)); });

	// TODO __setitem__ ?
	// TODO Make number of candidates changeable?
}

PRAAT_CLASS_BINDING(Pitch) {
	NESTED_BINDINGS(Pitch_Candidate,
	                Pitch_Frame)

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

	def("get_value_at_time",
	    [](Pitch self, double time, kPitch_unit unit, Interpolation interpolation) {
		    if (interpolation != Interpolation::NEAREST && interpolation != Interpolation::LINEAR)
			    Melder_throw(U"Pitch values can only be queried using NEAREST or LINEAR interpolation");
		    auto value = Sampled_getValueAtX(self, time, Pitch_LEVEL_FREQUENCY, static_cast<int>(unit), interpolation == Interpolation::LINEAR);
		    return Function_convertToNonlogarithmic(self, value, Pitch_LEVEL_FREQUENCY, static_cast<int>(unit));
	    },
	    "time"_a, "unit"_a = kPitch_unit::HERTZ, "interpolation"_a = Interpolation::LINEAR);

	// TODO get_strength_at_time ? -> Pitch strength unit enum

	def("get_value_in_frame",
	    [](Pitch self, long frameNumber, kPitch_unit unit) {
		    auto value = Sampled_getValueAtSample(self, frameNumber, Pitch_LEVEL_FREQUENCY, static_cast<int>(unit));
		    return Function_convertToNonlogarithmic(self, value, Pitch_LEVEL_FREQUENCY, static_cast<int>(unit));
	    },
	    "frame_number"_a, "unit"_a = kPitch_unit::HERTZ);

	// TODO Minimum, Time of minimum, Maximum, Time of maximum, ...

	def("get_mean_absolute_slope",
	    [](Pitch self, kPitch_unit unit) {
		    double slope;
		    long nVoiced = 0;
		    switch (unit) {
			case kPitch_unit::HERTZ:
				nVoiced = Pitch_getMeanAbsSlope_hertz(self, &slope);
				break;
			case kPitch_unit::MEL:
				nVoiced = Pitch_getMeanAbsSlope_mel(self, &slope);
				break;
			case kPitch_unit::SEMITONES_1:
			case kPitch_unit::SEMITONES_100:
			case kPitch_unit::SEMITONES_200:
			case kPitch_unit::SEMITONES_440:
				nVoiced = Pitch_getMeanAbsSlope_semitones(self, &slope);
				break;
			case kPitch_unit::ERB:
				nVoiced = Pitch_getMeanAbsSlope_erb(self, &slope);
				break;
			case kPitch_unit::HERTZ_LOGARITHMIC:
			case kPitch_unit::LOG_HERTZ:
				Melder_throw(U"The mean absolute slope of a Pitch object can only be calculated with units HERTZ, MEL, SEMITONES_1, SEMITONES_100, SEMITONES_200, SEMITONES_440, and ERB");
			case kPitch_unit::UNDEFINED:
				Melder_throw(U"ERROR: PitchUnit should never be UNDEFINED!");
		    }
		    if (nVoiced < 2)
			    return double{undefined};
		    return slope;
	    }, "unit"_a = kPitch_unit::HERTZ);

	def("get_slope_without_octave_jumps",
	    [](Pitch self) {
		    double slope;
		    Pitch_getMeanAbsSlope_noOctave(self, &slope);
		    return slope;
	    });

	def("count_differences",
	    [](Pitch self, Pitch other) {
		    MelderInfoInterceptor info;
		    Pitch_difference(self, other);
		    return info.get();
	    },
	    "other"_a.none(false));

	def("formula",
	    [](Pitch self, const std::u32string &formula) { Pitch_formula(self, formula.c_str(), nullptr); },
	    "formula"_a);

	// TODO To TextGrid..., To TextTier, To IntervalTier: depends TextGrid and Tiers
	// TODO To PointProcess: depends on PointProcess

	def("interpolate",
	    &Pitch_interpolate);

	def("smooth",
		args_cast<_, Positive<_>>(Pitch_smooth),
		"bandwidth"_a = 10.0);

	def("subtract_linear_fit",
	    &Pitch_subtractLinearFit,
		"unit"_a = kPitch_unit::HERTZ);

	def("kill_octave_jumps",
		&Pitch_killOctaveJumps);

	// TODO To PitchTier: depends on PitchTier

	def("to_matrix",
	    &Pitch_to_Matrix);

	def_readwrite("ceiling", &structPitch::ceiling);

	def_readonly("max_n_candidates", &structPitch::maxnCandidates);

	def("get_frame",
	    [](Pitch self, Positive<integer> frameNumber) {
		    if (frameNumber > self->nx) Melder_throw(U"Frame number out of range");
		    return &self->frame[frameNumber];
	    },
	    "frame_number"_a, py::return_value_policy::reference_internal);

	def("__getitem__",
	    [](Pitch self, long i) {
		    if (i < 0) i += self->nx; // Python-style negative indexing
		    if (i < 0 || i >= self->nx) throw py::index_error("Pitch index out of range");
		    return &self->frame[i+1];
	    },
	    "i"_a, py::return_value_policy::reference_internal);

	def("__getitem__",
	    [](Pitch self, std::tuple<long, long> ij) {
		    long i, j; std::tie(i, j) = ij;
		    if (i < 0) i += self->nx; // Python-style negative indexing
		    if (i < 0 || i >= self->nx) throw py::index_error("Pitch index out of range");
		    auto &frame = self->frame[i+1];
		    if (j < 0) j += frame.nCandidates; // Python-style negative indexing
		    if (j < 0 || j >= frame.nCandidates) throw py::index_error("Pitch Frame index out of range");
		    return frame.candidate[j+1];
	    },
	    "ij"_a);

	// TODO __setitem__

	def("__iter__",
	    [](Pitch self) { return py::make_iterator(&self->frame[1], &self->frame[self->nx+1]); },
	    py::keep_alive<0, 1>());

	def("to_array",
	    [](Pitch self) {
		    auto maxCandidates = Pitch_getMaxnCandidates(self);
		    py::array_t<structPitch_Candidate> array({static_cast<size_t>(maxCandidates), static_cast<size_t>(self->nx)});

		    auto unchecked = array.mutable_unchecked<2>();
		    for (auto i = 0; i < self->nx; ++i) {
			    auto &frame = self->frame[i+1];
			    for (auto j = 0; j < maxCandidates; ++j) {
				    unchecked(j, i) = (j < frame.nCandidates) ? frame.candidate[j+1] : structPitch_Candidate{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};
			    }
		    }

		    return array;
	    });

	def_property_readonly("selected",
	                      [](Pitch self) {
		                      std::vector<structPitch_Candidate> vector;
		                      vector.reserve(self->nx);
		                      std::transform(&self->frame[1], &self->frame[self->nx + 1], std::back_inserter(vector), [](auto &frame) { return frame.candidate[1]; });
		                      return vector;
	                      });

	def_property_readonly("selected_array",
	                      [](Pitch self) {
		                      py::array_t<structPitch_Candidate> array(static_cast<size_t>(self->nx));

		                      auto unchecked = array.mutable_unchecked<1>();
		                      for (auto i = 0; i < self->nx; ++i) {
			                      unchecked(i) = self->frame[i+1].candidate[1];
		                      }

		                      return array;
	                      });

	def("path_finder",
	    args_cast<_, _, _, _, _, _, Positive<_>, bool>(Pitch_pathFinder),
	    "silence_threshold"_a = 0.03, "voicing_threshold"_a = 0.45, "octave_cost"_a = 0.01, "octave_jump_cost"_a = 0.35, "voiced_unvoiced_cost"_a = 0.14, "ceiling"_a = 600.0, "pull_formants"_a = false);

	def("step",
	    [](Pitch self, double step, Positive<double> precision, optional<double> fromTime, optional<double> toTime) { Pitch_step(self, step, precision, fromTime.value_or(self->xmin), toTime.value_or(self->xmax)); },
	    "step"_a, "precision"_a = 0.1, "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("octave_up",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) {
		    Pitch_step(self, 2.0, 0.1, fromTime.value_or(self->xmin), toTime.value_or(self->xmax));
	    },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("fifth_up",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) {
		    Pitch_step(self, 1.5, 0.1, fromTime.value_or(self->xmin), toTime.value_or(self->xmax));
	    },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("fifth_down",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) {
		    Pitch_step(self, 1 / 1.5, 0.1, fromTime.value_or(self->xmin), toTime.value_or(self->xmax));
	    },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("octave_down",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) {
		    Pitch_step(self, 0.5, 0.1, fromTime.value_or(self->xmin), toTime.value_or(self->xmax));
	    },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	def("unvoice",
	    [](Pitch self, optional<double> fromTime, optional<double> toTime) {
		    long ileft = Sampled_xToHighIndex(self, fromTime.value_or(self->xmin));
		    long iright = Sampled_xToLowIndex(self, toTime.value_or(self->xmax));

		    if (ileft < 1) ileft = 1;
		    if (iright > self->nx) iright = self-> nx;

		    for (auto i = ileft; i <= iright; i ++) {
			    auto &frame = self->frame[i];
			    for (long j = 1; j <= frame.nCandidates; j++) {
				    if (frame.candidate[j].frequency == 0.0) {
					    std::swap(frame.candidate[1], frame.candidate[j]);
					    break;
				    }
			    }
		    }
	    },
	    "from_time"_a = nullopt, "to_time"_a = nullopt);

	// TODO Pitch_Intensity_getMean & Pitch_Intensity_getMeanAbsoluteSlope ? (cfr. Intensity)
}

} // namespace parselmouth
