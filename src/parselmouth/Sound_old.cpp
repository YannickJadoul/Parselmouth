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

void initSound(parselmouth::PraatBindings &bindings)
{
	bindings.get<Sound>()
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
