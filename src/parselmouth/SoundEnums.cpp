#include "Parselmouth.h"

namespace parselmouth {

void initSoundEnums(PraatBindings &bindings)
{
	bindings.get<WindowShape>()
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

	bindings.get<AmplitudeScaling>()
			.value("integral", kSounds_convolve_scaling_INTEGRAL)
			.value("sum", kSounds_convolve_scaling_SUM)
			.value("normalize", kSounds_convolve_scaling_NORMALIZE)
			.value("peak_0_99", kSounds_convolve_scaling_PEAK_099)
			;

	bindings.get<SignalOutsideTimeDomain>()
			.value("zero", kSounds_convolve_signalOutsideTimeDomain_ZERO)
			.value("similar", kSounds_convolve_signalOutsideTimeDomain_SIMILAR)
			;
}

} // namespace parselmouth
