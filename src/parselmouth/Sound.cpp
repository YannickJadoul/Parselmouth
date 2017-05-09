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

			.def_static("combine_to_stereo",
			            [] (const std::vector<Sound> &sounds) { /*Sounds_combineToStereo; TODO Implement */ }, // TODO Iterable, or *args?
						"sounds"_a)

			.def("convert_to_mono",
			     &Sound_convertToMono)

			// TODO Handle nullptr for 'other'
			.def("convolve",
			     &Sounds_convolve,
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			// TODO Group different filters into enum/class/...?
			;
}

} // namespace parselmouth
