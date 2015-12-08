#include "fon/Sound.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/melder.h"
#undef I
#undef trace

#include "buffer_protocol.h"
#include "functor_signature.h"
#include <boost/python.hpp>
#include <boost/numpy.hpp>

#include "PraatUtils.h"

Sound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file).transfer();
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
	Melder_batch = true;
	
	using namespace boost::python;
	using namespace boost::numpy;

	boost::numpy::initialize();

	docstring_options docstringOptions(true, true, false);

	register_exception_translator<MelderError>(
			[] (const MelderError &) {
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				throw std::runtime_error(message);
	        });


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

	class_<structSound, boost::noncopyable> ("Sound", no_init)
		.def("__init__",
				make_constructor(&readSound,
						default_call_policies(),
						arg("path")))

		.def("__str__",
				[] (Sound self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("read_file",
				&readSound,
				return_value_policy<manage_new_object>())
				.staticmethod("read_file")

		.def("to_mfcc",
				returnsAutoThing(&Sound_to_MFCC),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))

		.def("to_mono",
				returnsAutoThing(&Sound_convertToMono),
				return_value_policy<manage_new_object>(),
				arg("self"))

		.def("to_stereo",
				returnsAutoThing(&Sound_convertToStereo),
				return_value_policy<manage_new_object>(),
				arg("self"))

		.def("extract_channel",
				returnsAutoThing(&Sound_extractChannel),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("channel")))

		.def("extract_left_channel",
				returnsAutoThing([] (Sound self) { return Sound_extractChannel(self, 1); }),
				return_value_policy<manage_new_object>(),
				(arg("self")))

		.def("extract_right_channel",
				returnsAutoThing([] (Sound self) { return Sound_extractChannel(self, 2); }),
				return_value_policy<manage_new_object>(),
				(arg("self")))

		.def("upsample",
				returnsAutoThing(&Sound_upsample),
				return_value_policy<manage_new_object>(),
				arg("self"))

		.def("resample",
				returnsAutoThing(&Sound_resample),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("sample_frequency"), arg("precision") = 50))

		.def("append",
				returnsAutoThing(&Sounds_append),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("silence"), arg("other")))

		.def("__add__",
				returnsAutoThing([] (Sound self, Sound other) { return Sounds_append(self, 0.0, other); }),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("other")))

		.def("convolve",
				returnsAutoThing(&Sounds_convolve),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("other"), arg("scaling") = kSounds_convolve_scaling_PEAK_099, arg("signal_outside_time_domain") = kSounds_convolve_signalOutsideTimeDomain_ZERO))

		.def("cross_correlate",
				returnsAutoThing(&Sounds_crossCorrelate),
				return_value_policy<manage_new_object>(),
				(arg("self"), arg("other"), arg("scaling") = kSounds_convolve_scaling_PEAK_099, arg("signal_outside_time_domain") = kSounds_convolve_signalOutsideTimeDomain_ZERO))

		.def("auto_correlate",
				returnsAutoThing(&Sound_autoCorrelate),
				return_value_policy<manage_new_object>(),
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
				(arg("self"), arg("tmin") = 0.0, arg("tmax") = 0.0, arg("roundTimesToNearestZeroCrossing") = true))
	;

	class_<structMFCC, boost::noncopyable>("MFCC", no_init)
		.def("__init__",
				make_constructor(returnsAutoThing(&Sound_to_MFCC),
						default_call_policies(),
						(arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

		.def("__str__",
				[] (MFCC self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_coefficients",
				&getCoefficients,
				arg("self"))
	;
}

