#pragma once

#include "common_docstring.h"

namespace parselmouth {

constexpr auto FRAME_CLASS_DOCSTRING = R"(Praat LPC Frame Class.

A class containing the linear predictive coding (LPC) analysis outcome over
a frame of acoustic data samples.

Note that the $a_0=1$ coefficient is omitted in a.

This class is instantiated in :object:`~parselmouth.LPC` object and not 
intended to be instantiated by user.

See also
--------
:praat:`Sound: LPC analysis`
:object:`~parselmouth.LPC`
)";

constexpr auto FRAME_N_COEFFICIENTS = R"(int : Number of AR coefficients a used to model the frame)";
constexpr auto FRAME_GAIN = R"(float : Linear predictive model gain)";
constexpr auto FRAME_A = R"(numpy.ndarray of float : Linear predictive model coefficients.)";


constexpr auto CLASS_DOCSTRING = R"(Praat LPC Class.

A sequence object contains the outcomes of linear predictive coding (LPC) 
analysis outcomes. The analysis is performed repeatedly with a sliding
window, which offsets :attr:`LPC.dt<parselmouth.LPC.dt>` seconds between 
frames.

The i-th item of this object is a :func:`LPC.Frame<parselmouth.LPC.Frame>`
object representing the i-th frame.

In the LPC analysis one tries to predict xn on the basis of the p previous 
samples, x′n = ∑ ak xn-k then {a1, a2, ..., ap} can be chosen to minimize
the prediction power Qp where Qp = E[ |xn - x′n|2 ].

This class is not intended to be instantiated by user, instead from a
:func`~parselmouth.Sound` object using one of its `to_lpc_xxx()` methods.

See Also
--------
:praat:`Sound: LPC analysis`
:object:`parselmouth.LPC.Frame`
:func:`parselmouth.Sound.to_lpc_autocorrelation`
:func:`parselmouth.Sound.to_lpc_covariance`
:func:`parselmouth.Sound.to_lpc_burg`
:func:`parselmouth.Sound.to_lpc_marple`
)";

constexpr auto SAMPLING_PERIOD_DOCSTRING = R"(Sampling rate of the audio data)";
constexpr auto MAX_N_COEFFICIENTS_DOCSTRING = R"(Largest number of coefficients over all frames)";

}// namespace parselmouth
