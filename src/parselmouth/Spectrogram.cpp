#include "Parselmouth.h"

#include "fon/Sound_and_Spectrogram.h"
#include "fon/Spectrum_and_Spectrogram.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Spectrogram>::init() {
	// TODO Constructor!

	def("get_power_at",
	    [](Spectrogram self, double time, double frequency) { return Matrix_getValueAtXY(self, time, frequency); },
		"time"_a, "frequency"_a);

	def("to_spectrum_slice", // TODO Pythonic alias?
	    &Spectrogram_to_Spectrum,
	    "time"_a);

	def("synthesize_sound", // TODO Sampling frequency not POSITIVE!?
	    &Spectrogram_to_Sound,
		"sampling_frequency"_a = 44100.0);

	def("to_sound", // TODO Sampling frequency not POSITIVE!?
	    &Spectrogram_to_Sound,
	    "sampling_frequency"_a = 44100.0);


	// TODO fmin, fmax, fn, f1 ?

	// TODO Formula (in Matrix?)

	// TODO TimeFrameSampled
	// TODO TimeFunction
}

// TODO Spectrogram_extension: BandFilterSpectrogram, BarkSpectrogram, MelSpectrogram

} // namespace parselmouth
