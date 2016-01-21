#include "fon/Sound.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"
#include "dwsys/NUMmachar.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/melder.h"
#include "sys/Thing.h"
#include "praat/UndefPraatMacros.h"

#include "boost_python/functor_signature.h" // Before anything else (from Boost.Python) is included, such that the function templates for boost::python::get_signature are taken into account.

#include "boost_python/buffer_protocol.h"
#include "boost_python/constructor.h"
#include "common/MovingCopyable.h"
#include "praat/CopyableAutoThing.h"
#include "praat/MelderInfoInterceptor.h"

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/numpy.hpp>

autoSound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file);
}

boost::numpy::ndarray getCoefficients(MFCC cc)
{
	auto maxCoefficients = CC_getMaximumNumberOfCoefficients(cc, 1, cc->nx);
	auto array = boost::numpy::empty(2, std::vector<Py_intptr_t>({cc->nx, maxCoefficients + 1}).data(), boost::numpy::dtype::get_builtin<double>());

	auto data = array.get_data();
	auto strides = array.get_strides();
	for (auto i = 0; i < cc->nx; ++i) {
		*reinterpret_cast<double*>(data + i * strides[0]) = cc->frame[i+1].c0;
		for (auto j = 1; j <= maxCoefficients; ++j) {
			*reinterpret_cast<double*>(data + i * strides[0] + j * strides[1]) = (j <= cc->frame[i+1].numberOfCoefficients) ? cc->frame[i+1].c[j] : std::numeric_limits<double>::quiet_NaN();
		}
	}

	return array;
}

BOOST_PYTHON_MODULE(parselmouth)
{
	// TODO Look at praat initialization again, see if there's a better solution that ad-hoc copy-pasting
	NUMmachar ();
	NUMinit ();
	Melder_alloc_init ();
	Melder_message_init ();

	Melder_batch = true;
	
	using namespace boost::python;
	using namespace boost::numpy;

	boost::numpy::initialize(false);

	docstring_options docstringOptions(true, true, false);

	register_exception_translator<MelderError>(
			[] (const MelderError &) {
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				throw std::runtime_error(message);
	        });


	enum class Interpolation
	{
		NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
		LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
		CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
		SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
		SINC700 = Vector_VALUE_INTERPOLATION_SINC700
	};

	enum_<Interpolation>("Interpolation")
		.value("nearest", Interpolation::NEAREST)
		.value("linear", Interpolation::LINEAR)
		.value("cubic", Interpolation::CUBIC)
		.value("sinc70", Interpolation::SINC70)
		.value("sinc700", Interpolation::SINC700)
		.export_values()
	;


	enum_<kSound_windowShape>("WindowShape")
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
		.export_values()
	;

	enum_<kSounds_convolve_scaling>("AmplitudeScaling")
		.value("integral", kSounds_convolve_scaling_INTEGRAL)
		.value("sum", kSounds_convolve_scaling_SUM)
		.value("normalize", kSounds_convolve_scaling_NORMALIZE)
		.value("peak_0_99", kSounds_convolve_scaling_PEAK_099)
		.export_values()
	;

	enum_<kSounds_convolve_signalOutsideTimeDomain>("SignalOutsideTimeDomain")
		.value("zero", kSounds_convolve_signalOutsideTimeDomain_ZERO)
		.value("similar", kSounds_convolve_signalOutsideTimeDomain_SIMILAR)
		.export_values()
	;

	class_<structSound, CopyableAutoThing<structSound>, boost::noncopyable>("Sound", no_init)
		.def(constructor(&readSound,
				(arg("self"), arg("path"))))

		.def("__str__",
				[] (Sound self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("read_file",
				&readSound,
				arg("path"))
				.staticmethod("read_file")

		/*.def("create_pure_tone",
				returnsAutoThing(&Sound_createAsPureTone),
				(arg("number_of channels") = 1, arg("start_time") = 0.0, arg("end_time") = 0.4, arg("sample_rate") = 44100.0, arg("frequency") = 440.0, arg("amplitude") = 0.2, arg("fade_in_duration") = 0.01, arg("fade_out_duration") = 0.01))
				.staticmethod("create_pure_tone")

		.def("create_tone_complex",
				returnsAutoThing(&Sound_createFromToneComplex),
				return_value_policy<manage_new_object>(),
				(arg("start_time") = 0.0, arg("end_time") = 1.0, arg("sample_rate") = 44100.0, arg("frequency") = 440.0, arg("amplitude") = 0.2, arg("fade_in_duration") = 0.01, arg("fade_out_duration") = 0.01))
				.staticmethod("create_tone_complex")*/
				// double startingTime, double endTime,	double sampleRate, int phase, double frequencyStep,	double firstFrequency, double ceiling, long numberOfComponents

		.def("to_mfcc",
				&Sound_to_MFCC,
				(arg("self"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))

		.def("to_mono",
				&Sound_convertToMono,
				arg("self"))

		.def("to_stereo",
				&Sound_convertToStereo,
				arg("self"))

		.def("extract_channel",
				&Sound_extractChannel,
				(arg("self"), arg("channel")))

		.def("extract_left_channel",
				[] (Sound self) { return Sound_extractChannel(self, 1); },
				(arg("self")))

		.def("extract_right_channel",
				[] (Sound self) { return Sound_extractChannel(self, 2); },
				(arg("self")))

		.def("upsample",
				&Sound_upsample,
				arg("self"))

		.def("resample",
				&Sound_resample,
				(arg("self"), arg("sample_frequency"), arg("precision") = 50))

		.def("append",
				&Sounds_append,
				(arg("self"), arg("silence"), arg("other")))

		.def("__add__",
				[] (Sound self, Sound other) { return Sounds_append(self, 0.0, other); },
				(arg("self"), arg("other")))

		.def("convolve",
				&Sounds_convolve,
				(arg("self"), arg("other"), arg("scaling") = kSounds_convolve_scaling_PEAK_099, arg("signal_outside_time_domain") = kSounds_convolve_signalOutsideTimeDomain_ZERO))

		.def("cross_correlate",
				&Sounds_crossCorrelate,
				(arg("self"), arg("other"), arg("scaling") = kSounds_convolve_scaling_PEAK_099, arg("signal_outside_time_domain") = kSounds_convolve_signalOutsideTimeDomain_ZERO))

		.def("auto_correlate",
				&Sound_autoCorrelate,
				(arg("self"), arg("scaling") = kSounds_convolve_scaling_PEAK_099, arg("signal_outside_time_domain") = kSounds_convolve_signalOutsideTimeDomain_ZERO))

		.def("get_root_mean_square",
				&Sound_getRootMeanSquare,
				(arg("self"), arg("xmin") = 0.0, arg("xmax") = 0.0))

		.def("get_energy",
				&Sound_getEnergy,
				(arg("self"), arg("xmin") = 0.0, arg("xmax") = 0.0))

		.def("get_power",
				&Sound_getPower,
				(arg("self"), arg("xmin") = 0.0, arg("xmax") = 0.0))

		.def("get_energy_in_air",
				&Sound_getEnergyInAir,
				arg("self"))

		.def("get_power_in_air",
				&Sound_getPowerInAir,
				arg("self"))

		.def("get_intensity",
				&Sound_getIntensity_dB,
				arg("self"))

		.def("get_nearest_zero_crossing",
				&Sound_getNearestZeroCrossing,
				(arg("self"), arg("position"), arg("channel") = 1))

		.def("set_zero",
				&Sound_setZero,
				(arg("self"), arg("tmin") = 0.0, arg("tmax") = 0.0, arg("nearest_zero_crossing") = true))

		.def("concatenate",
				[] (const object &iterable, double overlap)
					{
						stl_input_iterator<Sound> iterator(iterable);
						std::vector<Sound> sounds(iterator, stl_input_iterator<Sound>());
						OrderedOf<structSound> soundList;
						for (const auto &sound : sounds)
							soundList.addItem_ref(sound);
						return Sounds_concatenate(soundList, overlap);
					},
				(arg("sounds"), arg("overlap") = 0.0))
				.staticmethod("concatenate")

		.def("multiply_by_window",
				&Sound_multiplyByWindow,
				(arg("self"), arg("window")))

		.def("scale_intensity",
				&Sound_scaleIntensity,
				(arg("self"), arg("new_average_intensity")))

		.def("override_sampling_frequency",
				&Sound_overrideSamplingFrequency,
				(arg("self"), arg("sample_rate")))

		.def("extract_part",
				&Sound_extractPart,
				(arg("self"), arg("start_time"), arg("end_time"), arg("window") = kSound_windowShape_RECTANGULAR, arg("relative_width") = 1.0, arg("preserve_times") = false))

		.def("extract_part_for_overlap",
				&Sound_extractPartForOverlap,
				(arg("self"), arg("start_time"), arg("end_time"), arg("overlap")))

		.def("as_array",
				[] (back_reference<Sound> self) -> ndarray { return from_data(&self.get()->z[1][1], dtype::get_builtin<double>(), std::vector<long>({self.get()->ny, self.get()->nx}), std::vector<long>({self.get()->nx * static_cast<long>(sizeof(double)), sizeof(double)}), self.source()); })

		.def("to_pitch",
				&Sound_to_Pitch,
				(arg("self"), arg("time_step") = 0.0, arg("minimum_pitch") = 75.0, arg("maximum_pitch") = 600.0))

		.def("to_intensity", // TODO Maybe get a template thing that just changes the type of the arguments, so we won't have an integer expected by parselmouth when it should be a boolean.
				[] (Sound self, double minimum_pitch, double time_step, bool subtract_mean) { return Sound_to_Intensity(self, minimum_pitch, time_step, subtract_mean); },
				(arg("self"), arg("minimum_pitch") = 100.0, arg("time_step") = 0.0, arg("subtract_mean") = true))

		.def("to_harmonicity_cc",
				&Sound_to_Harmonicity_cc,
				(arg("self"), arg("time_step") = 0.01, arg("minimum_pitch") = 75.0, arg("silence_treshold") = 0.1, arg("periods_per_window") = 1.0))
	;
	auto_thing_converter<structSound>();

	class_<structMFCC, CopyableAutoThing<structMFCC>, boost::noncopyable>("MFCC", no_init)
		.def(constructor(&Sound_to_MFCC,
				(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

		.def("__str__",
				[] (MFCC self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_coefficients",
				&getCoefficients,
				arg("self"))
	;
	auto_thing_converter<structMFCC>();


	enum_<kPitch_unit>("PitchUnit")
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
		.export_values()
	;

	class_<structPitch, CopyableAutoThing<structPitch>, boost::noncopyable>("Pitch", no_init)
		.def("__str__",
				[] (Pitch self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_value",
				[] (Pitch self, double time, kPitch_unit unit, bool interpolate) { return Pitch_getValueAtTime(self, time, unit, interpolate); },
				(arg("self"), arg("time"), arg("unit") = kPitch_unit_HERTZ, arg("interpolate") = true))
	;
	auto_thing_converter<structPitch>();


	class_<structIntensity, CopyableAutoThing<structIntensity>, boost::noncopyable>("Intensity", no_init)
		.def("__str__",
				[] (Intensity self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_value", // TODO Should be part of Vector class?
				[] (Intensity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				(arg("self"), arg("time"), arg("interpolation") = Interpolation::CUBIC))
	;
	auto_thing_converter<structIntensity>();


	class_<structHarmonicity, CopyableAutoThing<structHarmonicity>, boost::noncopyable>("Harmonicity", no_init)
		.def("__str__",
				[] (Harmonicity self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_value", // TODO Should be part of Vector class?
				[] (Harmonicity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				(arg("self"), arg("time"), arg("interpolation") = Interpolation::CUBIC))
	;
	auto_thing_converter<structHarmonicity>();
}
