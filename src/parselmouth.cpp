#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "fon/Manipulation.h"
#include "fon/Sound.h"
#include "fon/Sound_and_Spectrogram.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"
#include "dwsys/NUMmachar.h"
#include "dwtools/Sound_to_MFCC.h"
#include "dwtools/Spectrogram_extensions.h"
#include "sys/Thing.h"

#include "praat/MelderInfoInterceptor.h"


PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);


autoSound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file);
}


void initializePraat()
{
	// TODO Look at praat initialization again, see if there's a better solution that ad-hoc copy-pasting (praatlib_init?)
	NUMmachar ();
	NUMinit ();
	Melder_alloc_init ();
	Melder_message_init ();

	Melder_batch = true;
}


namespace py = pybind11;
using namespace py::literals;

PYBIND11_PLUGIN(parselmouth) {
	initializePraat();

	py::module m("parselmouth");


    static py::exception<MelderError> melderErrorException(m, "PraatError", PyExc_RuntimeError);
	py::register_exception_translator([](std::exception_ptr p) {
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const MelderError &e) {
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				melderErrorException(message.c_str());
			}});


	enum class Interpolation
	{
		NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
		LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
		CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
		SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
		SINC700 = Vector_VALUE_INTERPOLATION_SINC700
	};

	py::enum_<Interpolation>(m, "Interpolation")
		.value("nearest", Interpolation::NEAREST)
		.value("linear", Interpolation::LINEAR)
		.value("cubic", Interpolation::CUBIC)
		.value("sinc70", Interpolation::SINC70)
		.value("sinc700", Interpolation::SINC700)
	;


	py::enum_<kSound_windowShape>(m, "WindowShape")
		.value("rectangular", kSound_windowShape_RECTANGULAR)
		.value("triangular", kSound_windowShape_TRIANGULAR)
		.value("parabolic", kSound_windowShape_PARABOLIC)
		.value("hanning", kSound_windowShape_HANNING)
		.value("hamming", kSound_windowShape_HAMMING)
		.value("gaussian1", kSound_windowShape_GAUSSIAN_1)
		.value("gaussian2", kSound_windowShape_GAUSSIAN_2)
		.value("gaussian3", kSound_windowShape_GAUSSIAN_3)
		.value("gaussian4", kSound_windowShape_GAUSSIAN_4)
		.value("gaussian5", kSound_windowShape_GAUSSIAN_5)
		.value("kaiser1", kSound_windowShape_KAISER_1)
		.value("kaiser2", kSound_windowShape_KAISER_2)
	;

	py::enum_<kSounds_convolve_scaling>(m, "AmplitudeScaling")
		.value("integral", kSounds_convolve_scaling_INTEGRAL)
		.value("sum", kSounds_convolve_scaling_SUM)
		.value("normalize", kSounds_convolve_scaling_NORMALIZE)
		.value("peak_0_99", kSounds_convolve_scaling_PEAK_099)
	;

	py::enum_<kSounds_convolve_signalOutsideTimeDomain>(m, "SignalOutsideTimeDomain")
		.value("zero", kSounds_convolve_signalOutsideTimeDomain_ZERO)
		.value("similar", kSounds_convolve_signalOutsideTimeDomain_SIMILAR)
	;

	py::enum_<kSound_to_Spectrogram_windowShape>(m, "SpectralAnalysisWindowShape")
		.value("square", kSound_to_Spectrogram_windowShape_SQUARE)
		.value("hamming", kSound_to_Spectrogram_windowShape_HAMMING)
		.value("bartlett", kSound_to_Spectrogram_windowShape_BARTLETT)
		.value("welch", kSound_to_Spectrogram_windowShape_WELCH)
		.value("hanning", kSound_to_Spectrogram_windowShape_HANNING)
		.value("gaussian", kSound_to_Spectrogram_windowShape_GAUSSIAN)
	;

	py::class_<structSound, autoSound>(m, "Sound")
		.def("__str__", // TODO Should probably be part of the Thing class?
				[] (Sound self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def_static("read_file",
				&readSound,
				"path"_a)

		.def_static("create_pure_tone",
				&Sound_createAsPureTone,
				"number_of channels"_a = 1, "start_time"_a = 0.0, "end_time"_a = 0.4, "sample_rate"_a = 44100.0, "frequency"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01, "fade_out_duration"_a = 0.01)

//		.def("create_tone_complex",
//				&Sound_createFromToneComplex,
//				"start_time"_a = 0.0, "end_time"_a = 1.0, "sample_rate"_a = 44100.0, "phase"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01, "fade_out_duration"_a = 0.01)
//				// double startingTime, double endTime, double sampleRate, int phase, double frequencyStep, double firstFrequency, double ceiling, long numberOfComponents

		.def("to_mfcc",
				&Sound_to_MFCC,
				"number_of_coefficients"_a = 12, "analysis_width"_a = 0.015, "dt"_a = 0.005, "f1_mel"_a = 100.0, "fmax_mel"_a = 0.0, "df_mel"_a = 100.0)

		.def("to_mono",
				&Sound_convertToMono)

		.def("to_stereo",
				&Sound_convertToStereo)

		.def("extract_channel",
				&Sound_extractChannel,
				"channel"_a)

		.def("extract_left_channel",
				[] (Sound self) { return Sound_extractChannel(self, 1); })

		.def("extract_right_channel",
				[] (Sound self) { return Sound_extractChannel(self, 2); })

		.def("upsample",
				&Sound_upsample)

		.def("resample",
				&Sound_resample,
				"sample_frequency"_a, "precision"_a = 50)

		.def("append",
				&Sounds_append,
				"silence"_a, "other"_a)

		.def("__add__",
				[] (Sound self, Sound other) { return Sounds_append(self, 0.0, other); },
				"other"_a)

		.def("convolve",
				&Sounds_convolve,
				"other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

		.def("cross_correlate",
				&Sounds_crossCorrelate,
				"other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

		.def("auto_correlate",
				&Sound_autoCorrelate,
				"scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

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
				[] (const std::vector<Sound> &sounds, double overlap)
					{
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
				"start_time"_a, "end_time"_a, "window"_a = kSound_windowShape_RECTANGULAR, "relative_width"_a = 1.0, "preserve_times"_a = false)

		.def("extract_part_for_overlap",
				&Sound_extractPartForOverlap,
				"start_time"_a, "end_time"_a, "overlap"_a)

		.def("as_array",
				[] (Sound self) { return py::array_t<double>({ static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); })

		.def("to_pitch",
				&Sound_to_Pitch,
				"time_step"_a = 0.0, "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0)

		.def("to_pitch_ac",
				[] (Sound self, double time_step, double minimum_pitch, double periods_per_window, int max_candidates, bool very_accurate, double silence_treshold, double voicing_threshold, double octave_cost, double octave_jump_cost, double voiced_unvoiced_cost, double pitch_ceiling) { return Sound_to_Pitch_ac(self, time_step, minimum_pitch, periods_per_window, max_candidates, very_accurate, silence_treshold, voicing_threshold, octave_cost, octave_jump_cost, voiced_unvoiced_cost, pitch_ceiling); },
				"time_step"_a = 0.0, "minimum_pitch"_a = 75.0, "periods_per_window"_a = 3.0, "max_candidates"_a = 15, "very_accurate"_a = false, "silence_treshold"_a = 0.03, "voicing_threshold"_a = 0.45, "octave_cost"_a = 0.01, "octave_jump_cost"_a = 0.35, "voiced_unvoiced_cost"_a = 0.14, "pitch_ceiling"_a = 600.0)

		.def("to_intensity", // TODO Maybe get a template thing that just changes the type of the arguments, so we won't have an integer expected by parselmouth when it should be a boolean.
				[] (Sound self, double minimum_pitch, double time_step, bool subtract_mean) { return Sound_to_Intensity(self, minimum_pitch, time_step, subtract_mean); },
				"minimum_pitch"_a = 100.0, "time_step"_a = 0.0, "subtract_mean"_a = true)

		.def("to_harmonicity_cc",
				&Sound_to_Harmonicity_cc,
				"time_step"_a = 0.01, "minimum_pitch"_a = 75.0, "silence_treshold"_a = 0.1, "periods_per_window"_a = 1.0)

		.def("to_spectrogram",
				[] (Sound self, double window_length, double maximum_frequency, double time_step, double frequency_step, kSound_to_Spectrogram_windowShape window_shape) { return Sound_to_Spectrogram(self, window_length, maximum_frequency, time_step, frequency_step, window_shape, 8.0, 8.0); },
				"window_length"_a = 0.005, "maximum_frequency"_a = 5000.0, "time_step"_a = 0.002, "frequency_step"_a = 20.0, "window_shape"_a = kSound_to_Spectrogram_windowShape_GAUSSIAN)

		.def("save_wav",
				[] (Sound self, const std::string &path)
					{
						structMelderFile file = { nullptr };
						Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
						Sound_saveAsAudioFile(self, &file, Melder_WAV, 16);
					},
				"path"_a)

		.def("to_manipulation",
				&Sound_to_Manipulation,
				"time_step"_a = 0.01, "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0)
	;

	py::class_<structMFCC, autoMFCC>(m, "MFCC")
		//.def(constructor(&Sound_to_MFCC,
		//		(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

		.def("__str__", // TODO Should probably be part of the Thing class?
				[] (MFCC self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("get_coefficients",
				[] (MFCC mfcc)
					{
						auto maxCoefficients = CC_getMaximumNumberOfCoefficients(mfcc, 1, mfcc->nx);
						py::array_t<double> array({static_cast<size_t>(mfcc->nx), static_cast<size_t>(maxCoefficients + 1)}, nullptr);

						for (auto i = 0; i < mfcc->nx; ++i) {
							*array.mutable_data(i, 0) = mfcc->frame[i+1].c0;
							for (auto j = 1; j <= maxCoefficients; ++j) {
								*array.mutable_data(i, j) = (j <= mfcc->frame[i+1].numberOfCoefficients) ? mfcc->frame[i+1].c[j] : std::numeric_limits<double>::quiet_NaN();
							}
						}

						return array;
					})
		.def("to_mel_spectrogram",
				&MFCC_to_MelSpectrogram,
				"from_coefficient"_a = 0, "to_coefficient"_a = 0, "include_c0"_a = true);
	;


	py::enum_<kPitch_unit>(m, "PitchUnit")
		.value("hertz", kPitch_unit_HERTZ)
		.value("hertz_logarithmic", kPitch_unit_HERTZ_LOGARITHMIC)
		.value("mel", kPitch_unit_MEL)
		.value("log_hertz", kPitch_unit_LOG_HERTZ)
		.value("mel", kPitch_unit_MEL)
		.value("semitones_1", kPitch_unit_SEMITONES_1)
		.value("semitones_100", kPitch_unit_SEMITONES_100)
		.value("semitones_200", kPitch_unit_SEMITONES_200)
		.value("semitones_440", kPitch_unit_SEMITONES_440)
		.value("erb", kPitch_unit_ERB)
	;

	py::class_<structPitch, autoPitch>(m, "Pitch")
		.def("__str__", // TODO Should probably be part of the Thing class?
				[] (Pitch self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("get_value",
				[] (Pitch self, double time, kPitch_unit unit, bool interpolate) { return Pitch_getValueAtTime(self, time, unit, interpolate); },
				"time"_a, "unit"_a = kPitch_unit_HERTZ, "interpolate"_a = true)
	;


	py::class_<structIntensity, autoIntensity>(m, "Intensity")
		.def("__str__", // TODO Should probably be part of the Thing class?
				[] (Intensity self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("get_value", // TODO Should be part of Vector class
				[] (Intensity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;


	py::class_<structHarmonicity, autoHarmonicity>(m, "Harmonicity")
		.def("__str__", // TODO Should probably be part of the Thing class?
				[] (Harmonicity self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("get_value", // TODO Should be part of Vector class
				[] (Harmonicity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;

	py::class_<structMelSpectrogram, autoMelSpectrogram>(m, "MelSpectrogram")
		.def("__str__",
				[] (MelSpectrogram self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("as_array",
				[] (MelSpectrogram self) { { return py::array_t<double>({ static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); } })

		.def_readonly("xmin",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::xmin))

		.def_readonly("xmax",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::xmax))

		.def_readonly("x1",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::x1))

		.def_readonly("dx",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::dx))

		.def_readonly("nx",
				static_cast<int structMelSpectrogram::*>(&structMelSpectrogram::nx))

		.def_readonly("fmin",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::ymin))

		.def_readonly("fmax",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::ymax))

		.def_readonly("f1",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::y1))

		.def_readonly("df",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::dy))

		.def_readonly("nf",
				static_cast<long structMelSpectrogram::*>(&structMelSpectrogram::ny))

		.def("frequency_to_hertz",
				&structMelSpectrogram::v_frequencyToHertz,
				"frequency"_a)
	;

	py::class_<structSpectrogram, autoSpectrogram>(m, "Spectrogram")
		.def("__str__",
				[] (Spectrogram self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.get()); }) // TODO Python 2 expects an old string for __str__ to work, while std::string is transformed into unicode. Check how Python 3 handles this and come up with a solution.

		.def("as_array",
				[] (Spectrogram self) { { return py::array_t<double>({ static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); } })

		.def_readonly("xmin",
				static_cast<double structSpectrogram::*>(&structSpectrogram::xmin))

		.def_readonly("xmax",
				static_cast<double structSpectrogram::*>(&structSpectrogram::xmax))

		.def_readonly("x1",
				static_cast<double structSpectrogram::*>(&structSpectrogram::x1))

		.def_readonly("dx",
				static_cast<double structSpectrogram::*>(&structSpectrogram::dx))

		.def_readonly("nx",
				static_cast<int structSpectrogram::*>(&structSpectrogram::nx))

		.def_readonly("fmin",
				static_cast<double structSpectrogram::*>(&structSpectrogram::ymin))

		.def_readonly("fmax",
				static_cast<double structSpectrogram::*>(&structSpectrogram::ymax))

		.def_readonly("f1",
				static_cast<double structSpectrogram::*>(&structSpectrogram::y1))

		.def_readonly("df",
				static_cast<double structSpectrogram::*>(&structSpectrogram::dy))

		.def_readonly("nf",
				static_cast<long structSpectrogram::*>(&structSpectrogram::ny))
	;

	py::class_<structPitchTier, autoPitchTier>(m, "PitchTier")
		.def("shift_frequencies",
				[] (PitchTier self, double tmin, double tmax, double shift, kPitch_unit unit) { PitchTier_shiftFrequencies(self, tmin, tmax, shift, unit); },
				"tmin"_a, "tmax"_a, "shift"_a = -20.0, "unit"_a = kPitch_unit::kPitch_unit_HERTZ)
		.def_readonly("tmin",
				static_cast<double structPitchTier::*>(&structPitchTier::xmin))
		.def_readonly("tmax",
				static_cast<double structPitchTier::*>(&structPitchTier::xmax))
	;

	py::class_<structManipulation, autoManipulation>(m, "Manipulation")
		.def("get_resynthesis_lpc",
		     [] (Manipulation self) { return Manipulation_to_Sound(self, Manipulation_PITCH_LPC); })
		.def("get_resynthesis_overlap_add",
		     [] (Manipulation self) { return Manipulation_to_Sound(self, Manipulation_OVERLAPADD); })
		.def_property_readonly("pitch",
				[] (Manipulation self) { return self->pitch.get(); })
	;

	return m.ptr();
}
