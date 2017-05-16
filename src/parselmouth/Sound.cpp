#include "Parselmouth.h"

#include "dwtools/Sound_extensions.h"
#include "dwtools/Sound_to_MFCC.h"
#include "fon/Sound_and_Spectrogram.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

template <typename T>
using optional = std::experimental::optional<T>;

namespace {

template <typename T, typename Container>
OrderedOf<T> referencesToOrderedOf(const Container &container)
{
	OrderedOf<T> orderedOf;
	std::for_each(begin(container), end(container), [&orderedOf] (T &item) { orderedOf.addItem_ref(&item); });
	return orderedOf;
}

autoSound readSound(const std::string &path)
{
	structMelderFile file = {nullptr};
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file);
}

} // namespace

void initSound(parselmouth::PraatBindings &bindings)
{
	// TODO Remove
	bindings.get<Sound>()
			.def_static("read_file",
			            &readSound,
			            "path"_a);


	bindings.get<Sound>()
					// TODO Constructors: from file (?) and from array

			.def("autocorrelate",
			     &Sound_autoCorrelate,
			     "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			// TODO Make sure pybind11's std::reference_wrapper does not accept None/nullptr
			.def_static("combine_to_stereo",
			            [] (const std::vector<std::reference_wrapper<structSound>> &sounds) { auto ordered = referencesToOrderedOf<structSound>(sounds); return Sounds_combineToStereo(&ordered); },
			            "sounds"_a)

			.def_static("concatenate", // TODO Overlap is POSITIVE
			            [] (const std::vector<std::reference_wrapper<structSound>> &sounds, double overlap) { auto ordered = referencesToOrderedOf<structSound>(sounds); return Sounds_concatenate(ordered, overlap); },
			            "sounds"_a, "overlap"_a = 0.0)
			// TODO concatenate recoverably
			// TODO concatenate as member function?

			.def("convert_to_mono",
			     &Sound_convertToMono)

			.def("convert_to_stereos",
			     &Sound_convertToStereo)

			// TODO Automatically wrap to have Sound -> structSound& ?
			.def("convolve",
			     [] (Sound self, structSound &other, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain &signal_outside_time_domain) { return Sounds_convolve(self, &other, scaling, signal_outside_time_domain); },
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			.def("cross_correlate",
			     [] (Sound self, structSound &other, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain &signal_outside_time_domain) { return Sounds_crossCorrelate(self, &other, scaling, signal_outside_time_domain); },
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)
			// TODO Cross-correlate (short)?

			.def("de_emphasize",
			     &Sound_deEmphasis,
			     "from_frequency"_a = 50.0) // TODO "from" / "from_frequency" ? Not POSITIVE now!?

			.def("deepen_band_modulation", // TODO All arguments POSITIVE
			     &Sound_deepenBandModulation,
			     "enhancement"_a = 20.0, "from_frequency"_a = 300.0, "to_frequency"_a = 8000.0, "slow_modulation"_a = 3.0, "fast_modulation"_a = 30.0, "band_smoothing"_a = 100.0)

			.def("extract_all_channels",
			     [] (Sound self)
			     {
				     std::vector<Sound> result; // TODO Make std::vector<autoSound>
				     result.reserve(self->ny);
				     for (auto i = 1; i <= self->ny; ++i) {
					     result.emplace_back(Sound_extractChannel(self, i).releaseToAmbiguousOwner());
				     }
				     return result;
			     })

			.def("extract_channel", // TODO Channel POSITIVE? (Actually CHANNEL; >= 1, but does not always have intended result (e.g., Set value at sample...))
			     &Sound_extractChannel,
			     "channel"_a)

			.def("extract_channel", // TODO Channel enum type?
			     [] (Sound self, std::string channel)
			     {
				     std::transform(channel.begin(), channel.end(), channel.begin(), static_cast<int (*)(int)>(&std::tolower));
				     if (channel == "left")
					     return Sound_extractChannel(self, 1);
				     if (channel == "right")
					     return Sound_extractChannel(self, 2);
				     Melder_throw(U"'channel' can only be 'left' or 'right'"); // TODO Melder_throw or throw PraatError ?
			     })

			.def("extract_left_channel",
			     [] (Sound self) { return Sound_extractChannel(self, 1); })

			.def("extract_right_channel",
			     [] (Sound self) { return Sound_extractChannel(self, 2); })

			.def("extract_part", // TODO relativeWidth is POSITIVE // TODO Something for optional<double> for from and to in Sounds?
			     [] (Sound self,optional<double> from, optional<double> to, kSound_windowShape windowShape, double relativeWidth, bool preserveTimes) { return Sound_extractPart(self, from.value_or(self->xmin), to.value_or(self->xmax), windowShape, relativeWidth, preserveTimes); },
			     "from"_a = nullptr, "to"_a = nullptr, "window_shape"_a = kSound_windowShape_RECTANGULAR, "relative_width"_a = 1.0, "preserve_times"_a = false)

			.def("extract_part_for_overlap", // TODO Overlap is POSITIVE
			     [] (Sound self, optional<double> from, optional<double> to, double overlap) { return Sound_extractPartForOverlap(self, from.value_or(self->xmin), to.value_or(self->xmax), overlap); },
			     "from"_a = nullptr, "to"_a = nullptr, "overlap"_a)

			// TODO Filters
			// TODO Group different filters into enum/class/...?

			// TODO Formula and Formula (part)

			.def("get_energy",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getEnergy(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullptr, "to"_a = nullptr)

			.def("get_energy_in_air",
			     &Sound_getEnergyInAir)

			.def("get_index_from_time",
			     &Sampled_xToIndex,
			     "time"_a)

			.def("get_intensity", // TODO Get intensity (dB) -> get_intensity_dB/get_intensity_db
			     &Sound_getIntensity_dB)

			.def("get_nearest_zero_crossing", // TODO Channel is CHANNEL
			     [](Sound self, double time, long channel) {
				     if (channel > self->ny) channel = 1;
				     return Sound_getNearestZeroCrossing (self, time, channel);
			     },
			     "time"_a, "channel"_a = 1)

			.def("get_number_of_channels",
			     [] (Sound self) { return self->ny; })

			.def_readonly("num_channels",
			              static_cast<long structSound::*>(&structSound::ny)) // TODO Remove static_cast once SampledXY is exported, or once this is fixed

			.def("get_number_of_samples",
			     [] (Sound self) { return self->nx; })

			.def_readonly("num_samples",
			              static_cast<int32 structSound::*>(&structSound::nx)) // TODO Remove static_cast once Sampled is exported, or once this is fixed

			.def("get_power",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getPower(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullptr, "to"_a = nullptr)

			.def("get_power_in_air",
			     &Sound_getPowerInAir)

			.def("get_root_mean_square",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getRootMeanSquare(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullptr, "to"_a = nullptr)

			.def("get_rms",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getRootMeanSquare(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullptr, "to"_a = nullptr)

			.def("get_sample_period",
			     [] (Sound self) { return self->dx; })

			.def_readonly("sample_period",
			              static_cast<double structSound::*>(&structSound::dx)) // TODO Remove static_cast once Sampled is exported, or once this is fixed

			.def("get_sample_rate",
			     [] (Sound self) { return 1 / self->dx; })

			.def_property_readonly("sample_period",
			                       [] (Sound self) { return 1 / self->dx; })

			.def("get_time_from_index", // TODO PraatIndex to distinguish 1-based silliness?
			     [] (Sound self, long sample) { return Sampled_indexToX(self, sample); },
			     "sample"_a)

			// TODO Get value at sample index

			.def("lengthen", // TODO Lengthen (Overlap-add) ? // TODO All parameters are POSITIVE
			     [] (Sound self, double minimumPitch, double maximumPitch, double factor) {
				     if (minimumPitch >= maximumPitch) Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
				     return Sound_lengthen_overlapAdd(self, minimumPitch, maximumPitch, factor);
			     },
			     "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0, "factor"_a)

			.def("multiply_by_window",
			     &Sound_multiplyByWindow,
			     "window_shape"_a)

			;
}

} // namespace parselmouth
