#pragma once

#include "common_docstring.h"

namespace parselmouth {

constexpr auto CLASS_DOCSTRING = R"(Praat LineSpectralFrequencies Class.

A sequence object contains a set of frequencies of line spectral contents,
found in the linear predictive coding (LPC) analysis. It contains
frequencies associated with each of LPC analysis frames, which are offset
by :attr:`LineSpectralFrequencies.dt<parselmouth.LPC.dt>` seconds.

The i-th item of this object is a numpy.ndarray of floats, containing the
frequencies found in the i-th LPC analysis window.

This class is not intended to be instantiated by user, instead by calling
:func:`~parselmouth.LPC.to_linespectralfrequencies()` method.

See Also
--------
:praat:`Sound: LPC analysis`
:func:`parselmouth.LPC.to_linespectralfrequencies`
)";

constexpr auto MAXIMUM_FREQUENCY_DOCSTRING = R"(float, readonly : Largest frequency over all frames.)";
constexpr auto MAXIMUM_NUMBER_OF_FREQUENCIES_DOCSTRING = R"(int, readonly : Largest number of frequency samples over all frames.)";

constexpr auto TO_LPC_DOCSTRING=R"(Convert to LPC object.

Returns :obj:`parselmouth.LPC` object with equivalent AR models.

See also
--------
:obj:`parselmouth.LPC`
:func:`parselmouth.LPC.to_line_spectral_frequencies`
)";

}// namespace parselmouth
