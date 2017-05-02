#include "Parselmouth.h"

#include "dwtools/Sound_to_MFCC.h"
#include "fon/Sound_and_Spectrogram.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"

#include <pybind11/numpy.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

namespace {

autoSound readSound(const std::string &path) {
	structMelderFile file = {nullptr};
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file);
}

} // namespace

void initSound(parselmouth::PraatBindings &bindings) {
	bindings.get<Sound>()
			.def_static("read_file",
			            &readSound,
			            "path"_a)

			.def_static("create_pure_tone",
			            &Sound_createAsPureTone,
			            "number_of channels"_a = 1, "start_time"_a = 0.0, "end_time"_a = 0.4, "sample_rate"_a = 44100.0,
			            "frequency"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01,
			            "fade_out_duration"_a = 0.01)

//		.def("create_tone_complex",
//				&Sound_createFromToneComplex,
//				"start_time"_a = 0.0, "end_time"_a = 1.0, "sample_rate"_a = 44100.0, "phase"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01, "fade_out_duration"_a = 0.01)
//				// double startingTime, double endTime, double sampleRate, int phase, double frequencyStep, double firstFrequency, double ceiling, long numberOfComponents

			.def("to_mfcc",
			     &Sound_to_MFCC,
			     "number_of_coefficients"_a = 12, "analysis_width"_a = 0.015, "dt"_a = 0.005, "f1_mel"_a = 100.0,
			     "fmax_mel"_a = 0.0, "df_mel"_a = 100.0)

			.def("to_mono",
			     &Sound_convertToMono)

			.def("to_stereo",
			     &Sound_convertToStereo)

			.def("extract_channel",
			     &Sound_extractChannel,
			     "channel"_a)

			.def("extract_left_channel",
			     [](Sound self) { return Sound_extractChannel(self, 1); })

			.def("extract_right_channel",
			     [](Sound self) { return Sound_extractChannel(self, 2); })

			.def("upsample",
			     &Sound_upsample)

			.def("resample",
			     &Sound_resample,
			     "sample_frequency"_a, "precision"_a = 50)

			.def("append",
			     &Sounds_append,
			     "silence"_a, "other"_a)

			.def("__add__",
			     [](Sound self, Sound other) { return Sounds_append(self, 0.0, other); },
			     "other"_a)

			.def("convolve",
			     &Sounds_convolve,
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099,
			     "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			.def("cross_correlate",
			     &Sounds_crossCorrelate,
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099,
			     "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			.def("auto_correlate",
			     &Sound_autoCorrelate,
			     "scaling"_a = kSounds_convolve_scaling_PEAK_099,
			     "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			.def("get_root_mean_square",
			     &Sound_getRootMeanSquare,
			     "xmin"_a = 0.0, "xmax"_a = 0.0)

			.def("get_energy",
			     &Sound_getEnergy,
			     "xmin"_a = 0.0, "xmax"_a = 0.0)

			.def("get_power",
			     &Sound_getPower,
			     "xmin"_a = 0.0, "xmax"_a = 0.0)

			.def("get_energy_in_air",
			     &Sound_getEnergyInAir)

			.def("get_power_in_air",
			     &Sound_getPowerInAir)

			.def("get_intensity",
			     &Sound_getIntensity_dB)

			.def("get_nearest_zero_crossing",
			     &Sound_getNearestZeroCrossing,
			     "position"_a, "channel"_a = 1)

			.def("set_zero",
			     &Sound_setZero,
			     "tmin"_a = 0.0, "tmax"_a = 0.0, "nearest_zero_crossing"_a = true)

			.def_static("concatenate",
			            [](const std::vector<Sound> &sounds, double overlap) {
				            OrderedOf<structSound> soundList;
				            for (const auto &sound : sounds)
					            soundList.addItem_ref(sound);
				            return Sounds_concatenate(soundList, overlap);
			            },
			            "sounds"_a, "overlap"_a = 0.0)

			.def("multiply_by_window",
			     &Sound_multiplyByWindow,
			     "window"_a)

			.def("scale_intensity",
			     &Sound_scaleIntensity,
			     "new_average_intensity"_a)

			.def("override_sampling_frequency",
			     &Sound_overrideSamplingFrequency,
			     "sample_rate"_a)

			.def("extract_part",
			     &Sound_extractPart,
			     "start_time"_a, "end_time"_a, "window"_a = kSound_windowShape_RECTANGULAR, "relative_width"_a = 1.0,
			     "preserve_times"_a = false)

			.def("extract_part_for_overlap",
			     &Sound_extractPartForOverlap,
			     "start_time"_a, "end_time"_a, "overlap"_a)

			.def("as_array",
			     [](Sound self) {
				     return py::array_t<double>({static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)},
				                                {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)},
				                                &self->z[1][1], py::cast(self));
			     })

			.def("to_pitch",
			     &Sound_to_Pitch,
			     "time_step"_a = 0.0, "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0)

			.def("to_pitch_ac",
			     [](Sound self, double time_step, double minimum_pitch, double periods_per_window, int max_candidates,
			        bool very_accurate, double silence_treshold, double voicing_threshold, double octave_cost,
			        double octave_jump_cost, double voiced_unvoiced_cost, double pitch_ceiling) { return Sound_to_Pitch_ac(self, time_step, minimum_pitch, periods_per_window, max_candidates, very_accurate, silence_treshold, voicing_threshold, octave_cost, octave_jump_cost, voiced_unvoiced_cost, pitch_ceiling); },
			     "time_step"_a = 0.0, "minimum_pitch"_a = 75.0, "periods_per_window"_a = 3.0, "max_candidates"_a = 15, "very_accurate"_a = false, "silence_treshold"_a = 0.03, "voicing_threshold"_a = 0.45, "octave_cost"_a = 0.01, "octave_jump_cost"_a = 0.35, "voiced_unvoiced_cost"_a = 0.14, "pitch_ceiling"_a = 600.0)

			.def("to_intensity", // TODO Maybe get a template thing that just changes the type of the arguments, so we won't have an integer expected by parselmouth when it should be a boolean.
					[](Sound self, double minimum_pitch, double time_step, bool subtract_mean) {
						return Sound_to_Intensity(self, minimum_pitch, time_step, subtract_mean);
					},
					"minimum_pitch"_a = 100.0, "time_step"_a = 0.0, "subtract_mean"_a = true)

			.def("to_harmonicity_cc",
			     &Sound_to_Harmonicity_cc,
			     "time_step"_a = 0.01, "minimum_pitch"_a = 75.0, "silence_treshold"_a = 0.1,
			     "periods_per_window"_a = 1.0)

			.def("to_spectrogram",
			     [](Sound self, double window_length, double maximum_frequency, double time_step, double frequency_step,
			        kSound_to_Spectrogram_windowShape window_shape) {
				     return Sound_to_Spectrogram(self, window_length, maximum_frequency, time_step, frequency_step, window_shape, 8.0, 8.0);
			     },
			     "window_length"_a = 0.005, "maximum_frequency"_a = 5000.0, "time_step"_a = 0.002,
			     "frequency_step"_a = 20.0, "window_shape"_a = kSound_to_Spectrogram_windowShape_GAUSSIAN)

			.def("save_wav",
			     [](Sound self, const std::string &path) {
				     structMelderFile file = {nullptr};
				     Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
				     Sound_saveAsAudioFile(self, &file, Melder_WAV, 16);
			     },
			     "path"_a)

			;
}

} // namespace parselmouth
