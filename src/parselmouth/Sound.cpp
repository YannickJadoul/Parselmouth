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

			// TODO Group different filters into enum/class/...?
			;
}

} // namespace parselmouth
