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

#include "TimeClassAspects.h"
#include "utils/pybind11/ImplicitStringToEnumConversion.h"

#include <praat/fon/PointProcess.h>
#include <praat/fon/Pitch_to_PointProcess.h>
#include <praat/fon/VoiceAnalysis.h>
#include <praat/melder/melder_alloc.h>

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
		addTimeFunctionMixin(*this);

		// CREATIONS: Constructors & from_xxx static methods

		def(py::init([](double startTime, double endTime) {
				if (endTime <= startTime)
					Melder_throw(U"The end time should be greater than the start time.");
				return PointProcess_create(startTime, endTime, 0);
			}),
			"start_time"_a = 0.0, "end_time"_a = 1.0, "Construct a new empty PointProcess instance");

		def(py::init([](Pitch pitch) {
				return Pitch_to_PointProcess(pitch);
			}),
			"pitch"_a, "Construct a new PointProcess instance from a Pitch instance");

		def(py::init([](Sound sound, Pitch pitch, py::kwargs kwargs) { //-> py::object {
				// process the keyword arguments
				std::string method = "cc";
				bool include_maxima = true, include_minima = false;
				for (auto item : kwargs)
				{
					std::string key = std::string(py::str(item.first));
					if (key == "method")
						method = py::str(item.second);
					else if (key == "include_maxima")
						include_maxima = bool(item.second);
					else if (key == "include_minima")
						include_minima = bool(item.second);
				}

				// run the method
				if (method == "cc")
					return Sound_Pitch_to_PointProcess_cc(sound, pitch);
				else if (method == "peaks")
					return Sound_Pitch_to_PointProcess_peaks(sound, pitch, include_maxima, include_minima);

				Melder_throw(U"Unknown method specified.");
			}),
			"sound"_a, "pitch"_a, "Construct a new PointProcess instance from Sound and Pitch instance");

		def_static("create_poisson_process",
				   &PointProcess_createPoissonProcess,
				   "start_time"_a = 0.0, "end_time"_a = 1.0, "density"_a = 100.0,
				   "Create a PointProcess instance with randomly drawn time points from a Poisson distribution");

		def_static("from_pitch",
				   &Pitch_to_PointProcess,
				   "pitch"_a, "Create a PointProcess instance from a Pitch instance");

		def_static("from_sound_pitch_cc",
				   &Sound_Pitch_to_PointProcess_cc,
				   "sound"_a, "pitch"_a,
				   "Create a PointProcess instance from Sound and Pitch instances using the cross-correlation method");

		def_static("from_sound_pitch_peaks",
				   &Sound_Pitch_to_PointProcess_peaks,
				   "sound"_a, "pitch"_a, "include_maxima"_a = true, "include_minima"_a = false,
				   "Create a PointProcess instance from Sound and Pitch instances using the peak-picking method");

		// HEARING:
		// DIRECT(PLAY_PointProcess_play)
		// DIRECT(PLAY_PointProcess_hum)

		// DRAWING:
		// FORM(GRAPHICS_PointProcess_draw, U"PointProcess: Draw", nullptr)

/**
 * Standard arguments for many of the query methods
 */
#define GET_RANGE_DEFAULT_PROPERTIES \
	"from_time"_a = 0.0, "to_time"_a = 0.0, "shortest_period"_a = 0.0001, "longest_period"_a = 0.02, "maximum_period_factor"_a = 1.3
#define GET_SHIMMER_RANGE_DEFAULT_PROPERTIES \
	"sound"_a, GET_RANGE_DEFAULT_PROPERTIES, "maximum_amplitude_factor"_a = 1.6

		// QUERIES:
		// -basic info
		def(
			"get_number_of_points", [](PointProcess self) { return self->nt; }, "Get the number of defined points");

		def("get_number_of_periods", PointProcess_getNumberOfPeriods, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the number of periods within the specified time range");

		def(
			"get_time_from_index", [](PointProcess self, int pointNumber) {
				return (pointNumber <= 0 || pointNumber > self->nt) ? py::none() : py::cast(self->t[pointNumber]);
			},
			"point_number"_a, "Get time associated with the point number (1-based index)");

		def(
			"get_time_points", [](PointProcess self) {
			// NOTE: this makes a copy
			auto &t = self->t;
			return std::vector<double>(t.cells, t.cells + t.size); },
			"Get all the defined time points as numpy double array");

		// -jitters
		def("get_jitter_local", &PointProcess_getJitter_local, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute difference between consecutive periods, divided by the average period "
			"(MDVP Jitt: 1.040% as a threshold for pathology)");

		def("get_jitter_local_absolute", &PointProcess_getJitter_local_absolute, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute difference between consecutive periods, in seconds "
			"(MDVP Jita: 83.200 μs as a threshold for pathology)");

		def("get_jitter_rap", &PointProcess_getJitter_rap, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the Relative Average Perturbation, the average absolute difference between a period and "
			"the average of it and its two neighbours, divided by the average period (MDVP: 0.680% as a threshold for pathology)");

		def("get_jitter_ppq5", &PointProcess_getJitter_ppq5, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the five-point Period Perturbation Quotient, the average absolute difference "
			"between a period and the average of it and its four closest neighbours, divided by the "
			"average period (MDVP PPQ, and gives 0.840% as a threshold for pathology)");

		def("get_jitter_ddp", &PointProcess_getJitter_ddp, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute difference between consecutive differences between consecutive periods, divided by the average period");

		// -voice breaks
		def(
			"get_count_and_fraction_of_voice_breaks", [](PointProcess self, double tmin, double tmax, double maximumPeriod) {
				MelderCountAndFraction out = PointProcess_getCountAndFractionOfVoiceBreaks(self, tmin, tmax, maximumPeriod);
				return std::make_tuple(out.count, out.numerator / out.denominator);
			},
			"from_time"_a=0.0, "to_time"_a=0.0, "longest_period"_a = 0.02, "Get tuple (number of voice breaks, time fraction of voice breaks,)");

		// -shimmers
		def("get_shimmer_local", &PointProcess_Sound_getShimmer_local,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute difference between the amplitudes of consecutive periods, "
			"divided by the average amplitude (MDVP Shim: 3.810% as a threshold for pathology)");

		def("get_shimmer_local_dB", &PointProcess_Sound_getShimmer_local_dB,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute base-10 logarithm of the difference between the amplitudes of "
			"consecutive periods, multiplied by 20 (MDVP ShdB: 0.350 dB as a threshold for pathology)");

		def("get_shimmer_local_apq3", &PointProcess_Sound_getShimmer_apq3,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the three-point Amplitude Perturbation Quotient, the average absolute difference "
			"between the amplitude of a period and the average of the amplitudes of its neighbours, "
			"divided by the average amplitude");

		def("get_shimmer_local_apq5", &PointProcess_Sound_getShimmer_apq5,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the five-point Amplitude Perturbation Quotient, the average absolute difference "
			"between the amplitude of a period and the average of the amplitudes of it and its four "
			"closest neighbours, divided by the average amplitude");

		def("get_shimmer_local_apq11", &PointProcess_Sound_getShimmer_apq11,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the 11-point Amplitude Perturbation Quotient, the average absolute difference "
			"between the amplitude of a period and the average of the amplitudes of it and its ten "
			"closest neighbours, divided by the average amplitude (MDVP APQ: 3.070% as a threshold "
			"for pathology)");

		def("get_shimmer_local_dda", &PointProcess_Sound_getShimmer_dda,
			GET_SHIMMER_RANGE_DEFAULT_PROPERTIES,
			"Get the average absolute difference between consecutive differences between the "
			"amplitudes of consecutive periods (three times APQ3)");

		// -nearst point index
		def("get_low_index", PointProcess_getLowIndex, "time"_a,
			"Get the 1-base index of the nearest point before or at the specified time (0 if none found)");

		def("get_high_index", PointProcess_getHighIndex, "time"_a,
			"Get the 1-base the index of the nearest point at or after the specified time (0 if none found)");

		def("get_nearest_index", PointProcess_getNearestIndex, "time"_a,
			"Get the 1-base index of the point nearest to the specified time (0 if none found)");

		def(
			"get_window_points", [](PointProcess self, double tmin, double tmax) {
				const MelderIntegerRange points = PointProcess_getWindowPoints(self, tmin, tmax);
				return std::make_tuple(points.first, points.last);
			},
			"tmin"_a, "tmax"_a,
			"Get starting and ending (1-based) indices included in the specified time range");
			
		// -period duration
		def("get_interval", &PointProcess_getInterval, "time"_a,
			"Get the duration of the interval around a specified time");

		// -statistics
		def("get_mean_period", PointProcess_getMeanPeriod, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the average period");

		def("get_stdev_period", PointProcess_getStdevPeriod, GET_RANGE_DEFAULT_PROPERTIES,
			"Get the standard deviation of the periods");

		// SET CALCULATIONS
		def("union", &PointProcesses_union, "other"_a,
			"Create new PointProcess instance containing all the points of the two original point "
			"processes, sorted by time");

		def("intersection", &PointProcesses_intersection, "other"_a,
			"Create new PointProcess instance containing only those points that occur in both "
			"original point processes");

		def("difference", &PointProcesses_difference, "other"_a.none(false),
			"Create new PointProcess instance containing only those points of the first selected "
			"original point process that do not occur in the second");

		// MODIFICATION
		def("add_point", &PointProcess_addPoint, "time"_a, "Add a time point");

		def(
			"add_points", [](PointProcess self, std::vector<double> times) {
				PointProcess_addPoints(self, constVEC(times.data(), times.size()));
			},
			"times"_a, "Add multiple time points from a numpy double array, times");

		def("remove_point", &PointProcess_removePoint, "point_number"_a,
			"Remove a time point specified by (1-base index) point_number");

		def("remove_point_near", &PointProcess_removePointNear, "time"_a,
			"Remove a time point nearest to the specified time in seconds");

		def("remove_points", &PointProcess_removePoints, "from_point_number"_a, "to_point_number"_a,
			"Remove time points between the specified (1-based) index range, including the edge points");

		def("remove_points_between", &PointProcess_removePointsBetween, "from_time"_a, "to_time"_a,
			"Remove time points between the specified time range");

		def("fill", &PointProcess_fill, "from_time"_a, "to_time"_a, "period"_a = 0.01,
			"Add equispaced time points between the specified time range separated by the specified period");

		def("voice", &PointProcess_voice, "period"_a = 0.01, "maximum_voiced_period"_a = 0.02000000001,
			"Add equispaced time points separated by the specified period over any existing period "
			"longer than maximum_voiced_period");

		// DIRECT (MODIFY_Point_Sound_transplantDomain) {
		// 	MODIFY_FIRST_OF_TWO (PointProcess, Sound)
		// 		my xmin = your xmin;
		// 		my xmax = your xmax;
		// 	MODIFY_FIRST_OF_TWO_END
		// }

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

		// FORM (NEW1_PointProcess_Sound_to_AmplitudeTier_period, U"PointProcess & Sound: To AmplitudeTier (period)", nullptr) {
		// 	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
		// 	OK
		// DO
		// 	CONVERT_TWO (PointProcess, Sound)
		// 		autoAmplitudeTier result = PointProcess_Sound_to_AmplitudeTier_period (me, you, fromTime, toTime,
		// 			shortestPeriod, longestPeriod, maximumPeriodFactor);
		// 	CONVERT_TWO_END (your name.get(), U"_", my name.get())
		// }

		// DIRECT (NEW1_PointProcess_Sound_to_AmplitudeTier_point) {
		// 	CONVERT_TWO (PointProcess, Sound)
		// 		autoAmplitudeTier result = PointProcess_Sound_to_AmplitudeTier_point (me, you);
		// 	CONVERT_TWO_END (your name.get(), U"_", my name.get());
		// }

		// FORM (NEW1_PointProcess_Sound_to_Ltas, U"PointProcess & Sound: To Ltas", nullptr) {
		// 	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
		// 	POSITIVE (bandwidth, U"Band width (Hz)", U"100.0")
		// 	REAL (shortestPeriod, U"Shortest period (s)", U"0.0001")
		// 	REAL (longestPeriod, U"Longest period (s)", U"0.02")
		// 	POSITIVE (maximumPeriodFactor, U"Maximum period factor", U"1.3")
		// 	OK
		// DO
		// 	CONVERT_TWO (PointProcess, Sound)
		// 		autoLtas result = PointProcess_Sound_to_Ltas (me, you,
		// 			maximumFrequency, bandwidth, shortestPeriod, longestPeriod, maximumPeriodFactor);
		// 	CONVERT_TWO_END (your name.get())
		// }

		// FORM (NEW1_PointProcess_Sound_to_Ltas_harmonics, U"PointProcess & Sound: To Ltas (harmonics", nullptr) {
		// 	NATURAL (maximumHarmonic, U"Maximum harmonic", U"20")
		// 	REAL (shortestPeriod, U"Shortest period (s)", U"0.0001")
		// 	REAL (longestPeriod, U"Longest period (s)", U"0.02")
		// 	POSITIVE (maximumPeriodFactor, U"Maximum period factor", U"1.3")
		// 	OK
		// DO
		// 	CONVERT_TWO (PointProcess, Sound)
		// 		autoLtas result = PointProcess_Sound_to_Ltas_harmonics (me, you,
		// 			maximumHarmonic, shortestPeriod, longestPeriod, maximumPeriodFactor);
		// 	CONVERT_TWO_END (your name.get())
		// }

		// FORM (NEW1_Sound_PointProcess_to_SoundEnsemble_correlate, U"Sound & PointProcess: To SoundEnsemble (correlate)", nullptr) {
		// 	REAL (fromTime, U"From time (s)", U"-0.1")
		// 	REAL (toTime, U"To time (s)", U"1.0")
		// 	OK
		// DO
		// 	CONVERT_TWO (Sound, PointProcess)
		// 		autoSound result = Sound_PointProcess_to_SoundEnsemble_correlate (me, you, fromTime, toTime);
		// 	CONVERT_TWO_END (your name.get())
		// }
	}

} // namespace parselmouth
