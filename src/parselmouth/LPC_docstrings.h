#pragma once

#include "common_docstring.h"

namespace parselmouth {

constexpr auto FRAME_CLASS_DOCSTRING = R"(Praat LPC Frame Class.

A class containing the linear predictive coding (LPC) analysis outcome over
a frame of acoustic data samples.

Note that the $a_0=1$ coefficient is omitted in a.

This class is instantiated in :obj:`~parselmouth.LPC` object and not
intended to be instantiated by user.

See also
--------
:praat:`Sound: LPC analysis`
:obj:`~parselmouth.LPC`
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
samples, :math:`x_n^' = \sum_{k=1}^p {a_k x_{n-k}}` then
:math:`{a_1, a_2, \ldots, a_p}` can be chosen to minimize the prediction
power :math:`Q_p` where :math:`Q_p = E[ |x_n - x_n^'|^2 ]`.

This class is not intended to be instantiated by user, instead from a
:obj:`~parselmouth.Sound` object using one of its to_lpc_xxx()
methods.

See Also
--------
:praat:`Sound: LPC analysis`
:obj:`parselmouth.LPC.Frame`
:func:`parselmouth.Sound.to_lpc_autocorrelation`
:func:`parselmouth.Sound.to_lpc_covariance`
:func:`parselmouth.Sound.to_lpc_burg`
:func:`parselmouth.Sound.to_lpc_marple`
)";

constexpr auto SAMPLING_PERIOD_DOCSTRING = R"(Sampling rate of the audio data)";
constexpr auto MAX_N_COEFFICIENTS_DOCSTRING = R"(Largest number of coefficients over all frames)";

constexpr auto TO_LINE_SPECTRAL_FREQUENCIES=R"(Convert to line spectra.

Returns :obj:`parselmouth.LineSpectralFrequencies` object with the
line frequencies found in the LPC models.

Parameter
----------
grid_size : float, default 0.0
    TBD
   
See also
--------
:obj:`parselmouth.LineSpectralFrequencies`
)";


constexpr auto TO_SPECTRUM_SLICE_DOCSTRING=R"(Convert to spectrogram.

Returns :obj:`parselmouth.Spectrum` object with the spectral
representation of the LPC model found at specified time t.

The Spectrum at t will be calculated from the nearest
:obj:`~parselmouth.LPC.Frame`. See :praat:`LPC: To Spectrum (slice)...`
for dedailed algorithm description.

Parameters
----------
time : float
    Time at which the spectrum should be calculated.
   
minimum_frequency_resolution : float, default 20.0
    Maximum distance separation of successive frequencies in the Spectrum,
    in Hz

bandwidth_reduction : float, default 0.0
    Reduces the bandwidth of each zero by this factor (<=0.0 for no
    reduction). Formants with small bandwidths show up very well as darker
    regions in the spectrogram because the poles lie close to the contour
    along which a spectrum is computed (the unit circle in the z-plane).
    Peak enhancement can be realized by computing a spectrum in the z-plane
    along a contour of radius:

        :math:`r = exp \left(– \pi \times
            \frac{bandwidthReduction}{samplingFrequency}\right)`.

deemphasis_frequency : float, default 50.0
    Performs de-emphasis when value is in this interval, specified in Hz.
    (0, Nyquist frequency)

See also
--------
:praat:`LPC: To Spectrum (slice)...`
)";

constexpr auto TO_SPECTROGRAM_DOCSTRING=R"(Convert to spectrogram.

Returns :obj:`parselmouth.Spectrogram` object with the spectral
representation of the LPC models.

For each LPC_Frame the corresponding Spectrum will be calculated according
to the algorithm explained in :func:`parselmouth.LPC.to_spectrum`. For each
frequency the power, i.e., the square of the complex values, will be stored
in the corresponding area in the Spectrogram.

Parameters
----------
minimum_frequency_resolution : float, default 20.0
    Maximum distance separation of successive frequencies in the Spectrum,
    in Hz

bandwidth_reduction : float, default 0.0
    Reduces the bandwidth of each zero by this factor (<=0.0 for no
    reduction). Formants with small bandwidths show up very well as darker
    regions in the spectrogram because the poles lie close to the contour
    along which a spectrum is computed (the unit circle in the z-plane).
    Peak enhancement can be realized by computing a spectrum in the z-plane
    along a contour of radius:

        :math:`r = exp \left(– \pi \times
            \frac{bandwidthReduction}{samplingFrequency}\right)`.

deemphasis_frequency : float, default 50.0
    Performs de-emphasis when value is in this interval, specified in Hz.
    (0, Nyquist frequency)

See also
--------
:praat:`LPC: To Spectrogram...`
)";

}// namespace parselmouth
