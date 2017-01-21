#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "fon/Sound.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"
#include "dwsys/NUMmachar.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/Thing.h"

#include "praat/MelderInfoInterceptor.h"


namespace pybind11 {
namespace detail {

template <typename Thing>
class type_caster<_Thing_auto<Thing>> {
public:
    static handle cast(_Thing_auto<Thing> &&src, return_value_policy policy, handle parent) {
        handle result = type_caster_base<Thing>::cast(src.get(), policy, parent);
        if (result)
            src.releaseToAmbiguousOwner();
        return result;
    }
    static PYBIND11_DESCR name() { return type_caster_base<Thing>::name(); }
};

template <typename Thing>
struct is_holder_type<Thing, _Thing_auto<Thing>> : std::true_type {};

}
}


autoSound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file);
}


void initializePraat()
{
	// TODO Look at praat initialization again, see if there's a better solution that ad-hoc copy-pasting
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

	py::class_<structSound, autoSound>(m, "Sound")
		.def("__str__",
				[] (Sound self) { MelderInfoInterceptor info; self->v_info(); return info.get(); })

		.def_static("read_file",
				&readSound,
				"path"_a)

		.def_static("create_pure_tone",
				&Sound_createAsPureTone,
				"number_of channels"_a = 1, "start_time"_a = 0.0, "end_time"_a = 0.4, "sample_rate"_a = 44100.0, "frequency"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01, "fade_out_duration"_a = 0.01)

//		.def("create_tone_complex",
//				&Sound_createFromToneComplex,
//				"start_time"_a = 0.0, "end_time"_a = 1.0, "sample_rate"_a = 44100.0, "phase"_a = 440.0, "amplitude"_a = 0.2, "fade_in_duration"_a = 0.01, "fade_out_duration"_a = 0.01)
//				// double startingTime, double endTime,	double sampleRate, int phase, double frequencyStep,	double firstFrequency, double ceiling, long numberOfComponents


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
				[] (Sound self) -> py::array { return py::array(py::dtype::of<double>(), {static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); })

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
	;

	return m.ptr();
}
