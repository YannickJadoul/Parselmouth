/*
 * Copyright (C) 2021  Yannick Jadoul and contributors
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
#include "PointProcess_docstrings.h"

#include "TimeClassAspects.h"
#include "utils/SignatureCast.h"
#include "utils/pybind11/ImplicitStringToEnumConversion.h"
#include "utils/pybind11/NumericPredicates.h"

#include <praat/fon/Matrix_and_PointProcess.h>
#include <praat/fon/Pitch_to_PointProcess.h>
#include <praat/fon/PointProcess.h>
#include <praat/fon/PointProcess_and_Sound.h>
#include <praat/fon/Sound_PointProcess.h>
#include <praat/fon/TextGrid.h>
#include <praat/fon/VoiceAnalysis.h>

#include <algorithm>
#include <tuple>
#include <vector>

#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;
using namespace std::string_literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(PointProcess) {
	using signature_cast_placeholder::_;

	addTimeFunctionMixin(*this);

	doc() = CREATE_CLASS_DOCSTRING;

	// NEW1_PointProcess_createEmpty
	def(py::init([](double startTime, double endTime) {
		    Melder_require (endTime >= startTime, U"Your end time (", endTime, U") should not be less than your start time (", startTime, U").");
		    return PointProcess_create(startTime, endTime, 0);
	    }),
	    "start_time"_a, "end_time"_a,
	    CONSTRUCTOR_EMPTY_DOCSTRING);

	// TODO Use py::array_t
	def(py::init([](std::vector<double> times, std::optional<double> startTime, std::optional<double> endTime) {
			if (times.empty())
				throw py::value_error("Cannot create a PointProcess from an empty list of time points.");

		    double t0 = startTime ? *startTime : *std::min_element(times.cbegin(), times.cend());
		    double t1 = endTime ? *endTime : *std::max_element(times.cbegin(), times.cend());

		    Melder_require (endTime >= startTime, U"Your end time (", t0, U") should not be less than your start time (", t1, U").");
		    auto result = PointProcess_create(t0, t1, times.size());

		    PointProcess_addPoints(result.get(), constVEC(times.data(), times.size()));
		    return result;
	    }),
	    "time_points"_a, "start_time"_a = std::nullopt, "end_time"_a = std::nullopt,
	    CONSTRUCTOR_FILLED_DOCSTRING);

	// NEW1_PointProcess_createPoissonProcess
	def_static("create_poisson_process",
	           args_cast<_, _, Positive<_>>(PointProcess_createPoissonProcess),
	           "start_time"_a, "end_time"_a, "density"_a,
	           CREATE_POISSON_PROCESS_DOCSTRING);

	// TODO .values? Maybe not, if the underlying array can change through `add_point`.

	// Make PointProcess class a s sequence-like Python class
	def("__getitem__",
	    [](PointProcess self, long i) {
			if (i < 0) i += self->nt;
			if (i < 0 || i >= self->nt)
				throw py::index_error("PointProcess index out of range");
			return self->t[i + 1];
		},
		"i"_a);

	def("__len__", [](PointProcess self) { return self->nt; });

	// Iterators come for free with the sequence protocol, but this is (apparently) slightly faster
	def("__iter__",
	    [](PointProcess self) {
			return py::make_iterator(&self->t[1], &self->t[self->nt + 1]);
		},
		py::keep_alive<0, 1>());

/**
 * Standard arguments for many of the query methods
 */
#define RANGE_FUNCTION(f) \
	[](PointProcess self, std::optional<double> fromTime, std::optional<double> toTime, double periodFloor, double periodCeiling, Positive<double> maximumPeriodFactor) { \
		return f(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), periodFloor, periodCeiling, maximumPeriodFactor); \
	}
#define RANGE_ARGS \
	"from_time"_a = std::nullopt, "to_time"_a = std::nullopt, "period_floor"_a = 0.0001, "period_ceiling"_a = 0.02, "maximum_period_factor"_a = 1.3
#define SHIMMER_RANGE_FUNCTION(f) \
	[](PointProcess self, Sound sound, std::optional<double> fromTime, std::optional<double> toTime, double periodFloor, double periodCeiling, Positive<double> maximumPeriodFactor, Positive<double> maximumAmplitudeFactor) { \
		return f(self, sound, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), periodFloor, periodCeiling, maximumPeriodFactor, maximumAmplitudeFactor); \
	}
#define SHIMMER_RANGE_ARGS \
	"sound"_a.none(false), RANGE_ARGS, "maximum_amplitude_factor"_a = 1.6

	// INTEGER_PointProcess_getNumberOfPoints
	def("get_number_of_points",
	    [](PointProcess self) { return self->nt; },
	    GET_NUMBER_OF_POINTS_DOCSTRING);

	def_readonly("n_points", &structPointProcess::nt);

	// INTEGER_PointProcess_getNumberOfPeriods
	def("get_number_of_periods",
	    RANGE_FUNCTION(PointProcess_getNumberOfPeriods),
	    RANGE_ARGS,
	    GET_NUMBER_OF_PERIODS_DOCSTRING);

	// REAL_PointProcess_getMeanPeriod
	def("get_mean_period",
	    RANGE_FUNCTION(PointProcess_getMeanPeriod),
	    RANGE_ARGS,
	    GET_MEAN_PERIOD_DOCSTRING);

	// REAL_PointProcess_getStdevPeriod
	def("get_stdev_period",
	    RANGE_FUNCTION(PointProcess_getStdevPeriod),
	    RANGE_ARGS,
	    GET_STDEV_PERIOD_DOCSTRING);

	// REAL_PointProcess_getTimeFromIndex
	def("get_time_from_index",
		[](PointProcess self, integer pointNumber) {
			return (pointNumber <= 0 || pointNumber > self->nt) ? undefined : self->t[pointNumber];
		},
		"point_number"_a,
		GET_TIME_FROM_INDEX_DOCSTRING);

	// REAL_PointProcess_getJitter_local
	def("get_jitter_local",
	    RANGE_FUNCTION(PointProcess_getJitter_local),
	    RANGE_ARGS,
	    GET_JITTER_LOCAL_DOCSTRING);

	// REAL_PointProcess_getJitter_local_absolute
	def("get_jitter_local_absolute",
	    RANGE_FUNCTION(PointProcess_getJitter_local_absolute),
	    RANGE_ARGS,
	    GET_JITTER_LOCAL_ABSOLUTE_DOCSTRING);

	// REAL_PointProcess_getJitter_rap
	def("get_jitter_rap",
	    RANGE_FUNCTION(PointProcess_getJitter_rap),
	    RANGE_ARGS,
	    GET_JITTER_RAP_DOCSTRING);

	// REAL_PointProcess_getJitter_ppq5
	def("get_jitter_ppq5",
	    RANGE_FUNCTION(PointProcess_getJitter_ppq5),
	    RANGE_ARGS,
	    GET_JITTER_PPQ5_DOCSTRING);

	// REAL_PointProcess_getJitter_ddp
	def("get_jitter_ddp",
	    RANGE_FUNCTION(PointProcess_getJitter_ddp),
	    RANGE_ARGS,
	    GET_JITTER_DDP_DOCSTRING);

	// TODO get_jitter(JitterMeasure) ?

	def("get_count_and_fraction_of_voice_breaks",
	    [](PointProcess self, std::optional<double> fromTime, std::optional<double> toTime, double maximumPeriod) {
			MelderCountAndFraction out = PointProcess_getCountAndFractionOfVoiceBreaks(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), maximumPeriod);
			return std::make_tuple(out.count, out.numerator / out.denominator, out.numerator, out.denominator);
		},
	    "from_time"_a = std::nullopt, "to_time"_a = std::nullopt, "period_ceiling"_a = 0.02,
	    GET_COUNT_AND_FRACTION_OF_VOICE_BREAKS_DOCSTRING);

	// REAL_Point_Sound_getShimmer_local
	def("get_shimmer_local",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_local),
	    SHIMMER_RANGE_ARGS, GET_SHIMMER_LOCAL_DOCSTRING);

	// REAL_Point_Sound_getShimmer_local_dB
	def("get_shimmer_local_db",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_local_dB),
	    SHIMMER_RANGE_ARGS,
	    GET_SHIMMER_LOCAL_DB_DOCSTRING);

	// REAL_Point_Sound_getShimmer_apq3
	def("get_shimmer_apq3",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_apq3),
	    SHIMMER_RANGE_ARGS,
	    GET_SHIMMER_APQ3_DOCSTRING);

	// REAL_Point_Sound_getShimmer_apq5
	def("get_shimmer_apq5",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_apq5),
	    SHIMMER_RANGE_ARGS,
	    GET_SHIMMER_APQ5_DOCSTRING);

	// REAL_Point_Sound_getShimmer_apq11
	def("get_shimmer_apq11",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_apq11),
	    SHIMMER_RANGE_ARGS,
	    GET_SHIMMER_APQ11_DOCSTRING);

	// REAL_Point_Sound_getShimmer_dda
	def("get_shimmer_dda",
	    SHIMMER_RANGE_FUNCTION(PointProcess_Sound_getShimmer_dda),
	    SHIMMER_RANGE_ARGS,
	    GET_SHIMMER_DDA_DOCSTRING);

	// INTEGER_PointProcess_getLowIndex
	def("get_low_index",
	    PointProcess_getLowIndex,
	    "time"_a,
	    GET_LOW_INDEX_DOCSTRING);

	// INTEGER_PointProcess_getHighIndex
	def("get_high_index",
	    PointProcess_getHighIndex,
	    "time"_a,
	    GET_HIGH_INDEX_DOCSTRING);

	// INTEGER_PointProcess_getNearestIndex
	def("get_nearest_index",
	    PointProcess_getNearestIndex,
	    "time"_a,
	    GET_NEAREST_INDEX_DOCSTRING);

	def("get_window_points",
	    [](PointProcess self, double tmin, double tmax) {
			const MelderIntegerRange points = PointProcess_getWindowPoints(self, tmin, tmax);
			return std::make_tuple(points.first, points.last);
		},
	    "from_time"_a, "to_time"_a,
	    GET_WINDOW_POINTS_DOCSTRING);

	// REAL_PointProcess_getInterval
	def("get_interval",
	    &PointProcess_getInterval,
	    "time"_a,
	    GET_INTERVAL_DOCSTRING);

	// NEW1_PointProcesses_union
	def("union",
		PointProcesses_union,
		"other"_a.none(false),
		UNION_DOCSTRING);

	// NEW1_PointProcesses_intersection
	def("intersection",
	    PointProcesses_intersection,
	    "other"_a.none(false),
	    INTERSECTION_DOCSTRING);

	// NEW1_PointProcesses_difference
	def("difference",
	    PointProcesses_difference,
	    "other"_a.none(false),
	    DIFFERENCE_DOCSTRING);

	// MODIFY_PointProcess_addPoint
	def("add_point",
	    PointProcess_addPoint,
	    "time"_a,
	    ADD_POINT_DOCSTRING);

	// MODIFY_PointProcess_addPoints
	def("add_points",
	    // TODO py::array_t ? Caster for constVEC?
	    [](PointProcess self, std::vector<double> times) {
			PointProcess_addPoints(self, constVEC(times.data(), times.size()));
		},
	    "times"_a,
	    ADD_POINTS_DOCSTRING);

	// MODIFY_PointProcess_removePoint
	def("remove_point",
	    PointProcess_removePoint,
	    "point_number"_a,
	    REMOVE_POINT_DOCSTRING);

	// MODIFY_PointProcess_removePointNear
	def("remove_point_near",
	    PointProcess_removePointNear,
	    "time"_a,
	    REMOVE_POINT_NEAR_DOCSTRING);

	// MODIFY_PointProcess_removePoints
	def("remove_points",
	    PointProcess_removePoints,
	    "from_point_number"_a, "to_point_number"_a,
	    REMOVE_POINTS_DOCSTRING);

	// MODIFY_PointProcess_removePointsBetween
	def("remove_points_between",
	    PointProcess_removePointsBetween,
	    "from_time"_a, "to_time"_a,
	    REMOVE_POINTS_BETWEEN_DOCSTRING);

	// MODIFY_PointProcess_fill
	def("fill",
	    [](PointProcess self, std::optional<double> fromTime, std::optional<double> toTime, Positive<double> period) {
			return PointProcess_fill(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), period);
		},
		"from_time"_a, "to_time"_a, "period"_a = 0.01,
	    FILL_DOCSTRING);

	// MODIFY_PointProcess_voice
	def("voice",
	    args_cast<_, Positive<_>, Positive<_>>(PointProcess_voice),
	    "period"_a = 0.01, "maximum_voiced_period"_a = 0.02000000001,
	    VOICE_DOCSTRING);

	// MODIFY_Point_Sound_transplantDomain
	def("transplant_domain",
	    [](PointProcess self, Sound src) {
			self->xmin = src->xmin;
			self->xmax = src->xmax;
		},
	    "sound"_a.none(false),
	    TRANSPLANT_DOMAIN_DOCSTRING);

	// NEW_PointProcess_to_IntervalTier

	// NEW_PointProcess_to_Matrix
	def("to_matrix",
	    PointProcess_to_Matrix);

	// NEW_PointProcess_to_PitchTier

	// NEW_PointProcess_to_TextGrid
	// TODO `std::vector<std::string>`?
	def("to_text_grid",
	    [](PointProcess self, const std::u32string tierNames, const std::u32string pointTiers) {
			return TextGrid_create(self->xmin, self->xmax, tierNames.c_str(), pointTiers.c_str());
		},
	    "tier_names"_a, "point_tiers"_a,
	    TO_TEXT_GRID_DOCSTRING);

	// NEW_PointProcess_to_TextGrid_vuv
	def("to_text_grid_vuv",
	    PointProcess_to_TextGrid_vuv,
	    "maximum_period"_a = 0.02, "mean_period"_a = 0.01,
	    TO_TEXT_GRID_VUV_DOCSTRING);

	// NEW_PointProcess_to_TextTier

	// NEW_PointProcess_to_Sound_pulseTrain
	def("to_sound_pulse_train",
	    PointProcess_to_Sound_pulseTrain,
	    "sampling_frequency"_a = 44100.0, "adaptation_factor"_a = 1.0, "adaptation_time"_a = 0.05, "interpolation_depth"_a = 2000,
	    TO_SOUND_PULSE_TRAIN_DOCSTRING);

	// NEW_PointProcess_to_Sound_phonation
	def("to_sound_phonation",
	    PointProcess_to_Sound_phonation,
	    "sampling_frequency"_a = 44100.0, "adaptation_factor"_a = 1.0, "maximum_period"_a = 0.05, "open_phase"_a = 0.7,"collision_phase"_a = 0.03, "power1"_a = 3.0, "power2"_a = 4.0,
	    TO_SOUND_PHONATION_DOCSTRING);

	// NEW_PointProcess_to_Sound_hum
	def("to_sound_hum",
	    PointProcess_to_Sound_hum,
	    TO_SOUND_HUM_DOCSTRING);

	// NEW_PointProcess_upto_IntensityTier
	// NEW_PointProcess_upto_PitchTier
	// NEW_PointProcess_upto_TextTier
	// NEW1_PointProcess_Sound_to_AmplitudeTier_period
	// NEW1_PointProcess_Sound_to_AmplitudeTier_point
	// NEW1_PointProcess_Sound_to_Ltas
	// NEW1_PointProcess_Sound_to_Ltas_harmonics
}

}// namespace parselmouth
