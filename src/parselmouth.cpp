#include "fon/Sound.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/melder.h"
#undef I
#undef trace

#include "buffer_protocol.h"
#include "constructor.h"
#include "functor_signature.h"

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/numpy.hpp>

#include "PraatUtils.h"

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


template <class T>
class MovingCopyable : public T
{
public:
	template <typename... Args>	MovingCopyable<T>(Args&&... args) : T(std::forward<Args>(args)...) {}
	MovingCopyable<T>(T &other) : T(std::move(other)) {}
	MovingCopyable<T>(const T &other) = delete;
	MovingCopyable<T>(MovingCopyable<T> &other) : T(std::move(other)) {}
	MovingCopyable<T>(const MovingCopyable<T> &other) = delete;
	MovingCopyable<T>(T &&other) : T(std::move(other)) {}
	MovingCopyable<T>(MovingCopyable<T> &&) = default;
	MovingCopyable<T> &operator=(T &other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(const T &other) = delete;
	MovingCopyable<T> &operator=(MovingCopyable<T> &other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(const MovingCopyable<T> &other) = delete;
	MovingCopyable<T> &operator=(T &&other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(MovingCopyable<T> &&) = default;
};

template <typename T>
using CopyableAutoThing = MovingCopyable<_Thing_auto<T>>;

template <typename T>
inline T *get_pointer(const _Thing_auto<T> &ptr)
{
	return ptr.peek();
}

namespace boost {
namespace python {

template <class T>
struct pointee<_Thing_auto<T>>
{
	typedef T type;
};

template <class T>
struct pointee<CopyableAutoThing<T>>
{
	typedef T type;
};

} // namespace python
} // namespace boost

template <typename T>
struct auto_thing_converter : boost::python::to_python_converter<_Thing_auto<T>, auto_thing_converter<T>, true>
{
	typedef boost::python::objects::make_ptr_instance<T, boost::python::objects::pointer_holder<CopyableAutoThing<T>, T>> MakeInstance;

    static PyObject *convert(const _Thing_auto<T> &x)
    {
    	// Sorry, we'll need to steal this _Thing_auto, just like an auto_ptr is stolen.
    	// The only difference is that we cannot accept the _Thing_auto by value, because is respects move semantics and cannot be copied.
    	// So, if this were an auto_ptr, the const_cast would happen before the call to this convert function that would take it by value.
        return MakeInstance::execute(const_cast<_Thing_auto<T>&>(x));
    }

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    static PyTypeObject const *get_pytype() { return MakeInstance::get_pytype(); }
#endif
};

BOOST_PYTHON_MODULE(parselmouth)
{
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

	class_<structSound, CopyableAutoThing<structSound>, boost::noncopyable> ("Sound", no_init)
		.def("__init__",
				constructor(returnsAutoThing(&readSound)),
				(arg("self"), arg("path")))

		.def("__str__",
				[] (Sound self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("read_file",
				&readSound,
				arg("path"))
				.staticmethod("read_file")

		/*.def("create_pure_tone",
				returnsAutoThing(&Sound_createAsPureTone),
				return_value_policy<manage_new_object>(),
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
						autoCollection collection = Collection_create(classSound, sounds.size());
						for (const auto &sound : sounds)
							Collection_addItem_ref(collection.peek(), sound);
						return Sounds_concatenate_e(collection.peek(), overlap);
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
	;
	auto_thing_converter<structSound>();

	class_<structMFCC, CopyableAutoThing<structMFCC>, boost::noncopyable>("MFCC", no_init)
		.def("__init__",
				constructor(returnsAutoThing(&Sound_to_MFCC)),
				(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))

		.def("__str__",
				[] (MFCC self) { MelderInfoInterceptor info; self->v_info(); return info.get(); },
				arg("self"))

		.def("get_coefficients",
				&getCoefficients,
				arg("self"))
	;
	auto_thing_converter<structMFCC>();
}
