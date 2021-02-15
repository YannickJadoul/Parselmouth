/*
 * Copyright (C) 2019-2021  Yannick Jadoul
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

#include <praat/fon/PointProcess.h>
#include <praat/fon/Pitch_to_PointProcess.h>
#include <praat/fon/VoiceAnalysis.h>

#include <vector>
#include <tuple>

#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;
using namespace std::string_literals;

namespace parselmouth
{
	PRAAT_CLASS_BINDING(PointProcess)
	{
		// FORM(NEW1_PointProcess_createEmpty, U"Create an empty PointProcess", U"Create empty PointProcess...")
		def(py::init([](double startTime, double endTime, integer initialMaxnt) {
				if (endTime <= startTime)
					Melder_throw(U"The end time should be greater than the start time.");
				return PointProcess_create(startTime, endTime, initialMaxnt);
			}),
			"start_time"_a = 0.0, "end_time"_a = 1.0, "initialMaxnt"_a = 0);

		//FORM(NEW1_PointProcess_createPoissonProcess, U"Create Poisson process", U"Create Poisson process...")
		// WORD(name, U"Name", U"poisson")
		// REAL(startTime, U"Start time (s)", U"0.0")
		// REAL(endTime, U"End time (s)", U"1.0")
		// POSITIVE(density, U"Density (/s)", U"100.0")

		def_static("from_pitch",
				   &Pitch_to_PointProcess,
				   "pitch"_a);

		def_static("from_sound_pitch_cc",
				   &Sound_Pitch_to_PointProcess_cc,
				   "sound"_a, "pitch"_a);

		def_static("from_sound_pitch_peaks",
				   &Sound_Pitch_to_PointProcess_peaks,
				   "sound"_a, "pitch"_a, "include_maxima"_a = true, "include_minima"_a = false);

		// FORM(MODIFY_PointProcess_fill, U"PointProcess: Fill", nullptr)
		def("fill", &PointProcess_fill, "from_time"_a = 0.0, "to_time"_a = 0.0, "period"_a = 0.01);

		// FORM (REAL_PointProcess_getInterval, U"PointProcess: Get interval", U"PointProcess: Get interval...") {
		def("get_interval", &PointProcess_getInterval, "time"_a);

#define GET_RANGE_DEFAULT_PROPERTIES \
	"from_time"_a = 0.0, "to_time"_a = 0.0, "shortest_period"_a = 0.0001, "longest_period"_a = 0.02, "maximum_period_factor"_a = 1.3

		// FORM (REAL_PointProcess_getJitter_local, U"PointProcess: Get jitter (local)", U"PointProcess: Get jitter (local)...") {
		def("get_jitter_local", &PointProcess_getJitter_local, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM (REAL_PointProcess_getJitter_local_absolute, U"PointProcess: Get jitter (local, absolute)", U"PointProcess: Get jitter (local, absolute)...") {
		def("get_jitter_local_absolute", &PointProcess_getJitter_local_absolute, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM(REAL_PointProcess_getJitter_rap, U"PointProcess: Get jitter (rap)", U"PointProcess: Get jitter (rap)...")
		def("get_jitter_rap", &PointProcess_getJitter_rap, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM(REAL_PointProcess_getJitter_ppq5, U"PointProcess: Get jitter (ppq5)", U"PointProcess: Get jitter (ppq5)...")
		def("get_jitter_ppq5", &PointProcess_getJitter_ppq5, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM(REAL_PointProcess_getJitter_ddp, U"PointProcess: Get jitter (ddp)", U"PointProcess: Get jitter (ddp)...")
		def("get_jitter_ddp", &PointProcess_getJitter_ddp, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM (REAL_PointProcess_getMeanPeriod, U"PointProcess: Get mean period", U"PointProcess: Get mean period...") {
		def("get_mean_period", PointProcess_getMeanPeriod, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM (REAL_PointProcess_getStdevPeriod, U"PointProcess: Get stdev period", U"PointProcess: Get stdev period...") {
		def("get_stdev_period", PointProcess_getStdevPeriod, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM (INTEGER_PointProcess_getLowIndex, U"PointProcess: Get low index", U"PointProcess: Get low index...") {
		def("get_low_index", PointProcess_getLowIndex, "time"_a, "index of nearest point not after specified time");

		// FORM (INTEGER_PointProcess_getHighIndex, U"PointProcess: Get high index", U"PointProcess: Get high index...") {
		def("get_high_index", PointProcess_getHighIndex, "time"_a, "index of nearest point not before specified time");

		// FORM (INTEGER_PointProcess_getNearestIndex, U"PointProcess: Get nearest index", U"PointProcess: Get nearest index...") {
		def("get_nearest_index", PointProcess_getNearestIndex, "time"_a, "index of point nearest to specified time");

		// DIRECT (INTEGER_PointProcess_getNumberOfPoints) {
		def("get_number_of_points", [](PointProcess self) { return self->nt; });

		// FORM (INTEGER_PointProcess_getNumberOfPeriods, U"PointProcess: Get number of periods", U"PointProcess: Get number of periods...") {
		def("get_number_of_periods", PointProcess_getNumberOfPeriods, GET_RANGE_DEFAULT_PROPERTIES);

		// FORM (REAL_PointProcess_getTimeFromIndex, U"Get time", 0 /*"PointProcess: Get time from index..."*/) {
		def(
			"get_time_from_index", [](PointProcess self, int pointNumber) {
				return (pointNumber <= 0 || pointNumber > self->nt) ? py::none() : py::cast(self->t[pointNumber]);
			},
			"point_number"_a, "get time associated with the point number (1-based index)");

		// DIRECT(HELP_PointProcess_help){
		// DIRECT(PLAY_PointProcess_hum)

		// FORM (MODIFY_PointProcess_addPoint, U"PointProcess: Add point", U"PointProcess: Add point...") {
		def("add_point", &PointProcess_addPoint, "time"_a = 0.5);

		// FORM (MODIFY_PointProcess_addPoints, U"PointProcess: Add points", U"PointProcess: Add point...") {
		// def("add_points", &PointProcess_addPoints, "times"_a = std::vector<double>({0.1, 0.2, 0.5}));

		// DIRECT(NEW1_PointProcesses_intersection)
		def("intersection", &PointProcesses_intersection, "other"_a);

		// DIRECT(PLAY_PointProcess_play)

		// FORM(MODIFY_PointProcess_removePoint, U"PointProcess: Remove point", U"PointProcess: Remove point...")
		def("remove_point", &PointProcess_removePoint, "point_number"_a = 1);

		// FORM(MODIFY_PointProcess_removePointNear, U"PointProcess: Remove point near", U"PointProcess: Remove point near...")
		def("remove_point_near", &PointProcess_removePointNear, "time"_a = 0.5);

		// FORM(MODIFY_PointProcess_removePoints, U"PointProcess: Remove points", U"PointProcess: Remove points...")
		def("remove_points", &PointProcess_removePoints, "from_point_number"_a = 1, "to_point_number"_a = 10);

		// FORM(MODIFY_PointProcess_removePointsBetween, U"PointProcess: Remove points between", U"PointProcess: Remove points between...")
		def("remove_points_between", &PointProcess_removePointsBetween, "from_time"_a = 0.3, "to_time"_a = 0.7);

		// FORM(MODIFY_PointProcess_voice, U"PointProcess: Fill unvoiced parts", nullptr)
		def("voice", &PointProcess_voice, "period"_a = 0.01, "maximum_voiced_period"_a = 0.02000000001);

		// non-Script PointProcess APIs
		def(
			"get_window_points", [](PointProcess self, double tmin, double tmax) {
				const MelderIntegerRange points = PointProcess_getWindowPoints(self, tmin, tmax);
				return std::make_tuple(points.first, points.last);
			},
			"tmin"_a, "tmax"_a);

		// DIRECT(NEW1_PointProcesses_difference)
		def("difference", &PointProcesses_difference, "other"_a.none(false));

		// DIRECT(NEW1_PointProcesses_union)
		def("union", &PointProcesses_union, "other"_a);

		// Skip? FORM(GRAPHICS_PointProcess_draw, U"PointProcess: Draw", nullptr)
		// void PointProcess_draw (PointProcess me, Graphics g, double fromTime, double toTime, bool garnish);

		// DIRECT(NEW_PointProcess_to_IntervalTier)
		// {
		// 	CONVERT_EACH(PointProcess)
		// 	autoIntervalTier result = IntervalTier_create(my xmin, my xmax);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// DIRECT(NEW_PointProcess_to_Matrix)
		// {
		// 	CONVERT_EACH(PointProcess)
		// 	autoMatrix result = PointProcess_to_Matrix(me);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_to_PitchTier, U"PointProcess: To PitchTier", U"PointProcess: To PitchTier...")
		// {
		// 	POSITIVE(maximumInterval, U"Maximum interval (s)", U"0.02")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoPitchTier result = PointProcess_to_PitchTier(me, maximumInterval);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_to_TextGrid, U"PointProcess: To TextGrid...", U"PointProcess: To TextGrid...")
		// {
		// 	SENTENCE(tierNames, U"Tier names", U"Mary John bell")
		// 	SENTENCE(pointTiers, U"Point tiers", U"bell")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoTextGrid result = TextGrid_create(my xmin, my xmax, tierNames, pointTiers);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_to_TextGrid_vuv, U"PointProcess: To TextGrid (vuv)...", U"PointProcess: To TextGrid (vuv)...")
		// {
		// 	POSITIVE(maximumPeriod, U"Maximum period (s)", U"0.02")
		// 	REAL(meanPeriod, U"Mean period (s)", U"0.01")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoTextGrid result = PointProcess_to_TextGrid_vuv(me, maximumPeriod, meanPeriod);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// DIRECT(NEW_PointProcess_to_TextTier)
		// {
		// 	CONVERT_EACH(PointProcess)
		// 	autoTextTier result = TextTier_create(my xmin, my xmax);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_to_Sound_phonation, U"PointProcess: To Sound (phonation)", U"PointProcess: To Sound (phonation)...")
		// {
		// 	POSITIVE(samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
		// 	POSITIVE(adaptationFactor, U"Adaptation factor", U"1.0")
		// 	POSITIVE(maximumPeriod, U"Maximum period (s)", U"0.05")
		// 	POSITIVE(openPhase, U"Open phase", U"0.7")
		// 	REAL(collisionPhase, U"Collision phase", U"0.03")
		// 	POSITIVE(power1, U"Power 1", U"3.0")
		// 	POSITIVE(power2, U"Power 2", U"4.0")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoSound result = PointProcess_to_Sound_phonation(me, samplingFrequency,
		// 																   adaptationFactor, maximumPeriod, openPhase, collisionPhase, power1, power2);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_to_Sound_pulseTrain, U"PointProcess: To Sound (pulse train)", U"PointProcess: To Sound (pulse train)...")
		// {
		// 	POSITIVE(samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
		// 	POSITIVE(adaptationFactor, U"Adaptation factor", U"1.0")
		// 	POSITIVE(adaptationTime, U"Adaptation time (s)", U"0.05")
		// 	NATURAL(interpolationDepth, U"Interpolation depth (samples)", U"2000")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoSound result = PointProcess_to_Sound_pulseTrain(me, samplingFrequency,
		// 																	adaptationFactor, adaptationTime, interpolationDepth);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// DIRECT(NEW_PointProcess_to_Sound_hum)
		// {
		// 	CONVERT_EACH(PointProcess)
		// 	autoSound result = PointProcess_to_Sound_hum(me);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_upto_IntensityTier, U"PointProcess: Up to IntensityTier", U"PointProcess: Up to IntensityTier...")
		// {
		// 	POSITIVE(intensity, U"Intensity (dB)", U"70.0")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoIntensityTier result = PointProcess_upto_IntensityTier(me, intensity);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_upto_PitchTier, U"PointProcess: Up to PitchTier", U"PointProcess: Up to PitchTier...")
		// {
		// 	POSITIVE(frequency, U"Frequency (Hz)", U"190.0")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoPitchTier result = PointProcess_upto_PitchTier(me, frequency);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// FORM(NEW_PointProcess_upto_TextTier, U"PointProcess: Up to TextTier", U"PointProcess: Up to TextTier...")
		// {
		// 	SENTENCE(text, U"Text", U"")
		// 	OK
		// 		DO
		// 			CONVERT_EACH(PointProcess)
		// 				autoTextTier result = PointProcess_upto_TextTier(me, text);
		// 	CONVERT_EACH_END(my name.get())
		// }

		// PointProcess functions not exposed in Praat Script

		def_readonly("_nt", &structPointProcess::nt);
		def_property_readonly("_t", [](PointProcess self) { 
			// NOTE: this makes a copy
			auto &t = self->t;
			return std::vector<double>(t.cells, t.cells+t.size); });

		def(
			"get_count_and_fraction_of_voice_breaks", [](PointProcess self, double tmin, double tmax, double maximumPeriod) {
				MelderCountAndFraction out = PointProcess_getCountAndFractionOfVoiceBreaks(self, tmin, tmax, maximumPeriod);
				return std::make_tuple(out.count, out.numerator / out.denominator);
			},
			"from_time"_a, "to_time"_a, "longest_period"_a = 0.02);

		// integer PointProcess_findPoint (PointProcess me, double t);
		// double PointProcess_getInterval (PointProcess me, double t);
	}

} // namespace parselmouth
