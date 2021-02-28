#pragma once

namespace parselmouth {

constexpr auto TO_LPC_AUTOCORRELATION_DOCSTRING = R"(Create LPC using autocorrelation method.

Run linear predictive coding (LPC) analysis with the autocorrelation 
method and returns a new :object:`~parselmouth.LPC` object containing the
analysis outcomes.

The autocorrelation algorithm is decribed in Markel & Gray (1976).

Warning
-------
You are advised not to use this command for formant analysis. For formant
analysis, instead use :func:`~parselmouth.Sound.to_formant_burg`, which 
also works via LPC.

Parameters
----------
prediction_order : int, default=16
    Number of linear prediction coefficients, also called the number of
    poles. Choose this number at least twice as large as the number of 
    spectral peaks that you want to detect. 

window_length : float, default=0.025
    Effective duration of each analysis frame in seconds. 

time_step : float, default=0.005
    Time step between two consecutive analysis frames in seconds.

preemphasis_frequency : float, default=50.0
    +6dB / octave filtering will be applied above this frequency. If you do
    not want pre-emphasis, choose a frequency greater than the Nyquist 
    frequency. 

See Also
--------
:praat:`Sound: To LPC (autocorrelation)...`
:object:`~parselmouth.LPC`
:func:`~parselmouth.Sound.to_lpc_covariance`
:func:`~parselmouth.Sound.to_lpc_burg`
:func:`~parselmouth.Sound.to_lpc_marple`
)";

constexpr auto TO_LPC_COVARIANCE_DOCSTRING = R"(Create LPC using covariance method.

Run linear predictive coding (LPC) analysis with the covariance method and
returns a new :object:`~parselmouth.LPC` object containing the analysis 
outcomes.

The covariance algorithm is decribed in Markel & Gray (1976).

Warning
-------
You are advised not to use this command for formant analysis. For formant
analysis, instead use :func:`~parselmouth.Sound.to_formant_burg`, which 
also works via LPC.

Parameters
----------
prediction_order : int, default=16
    Number of linear prediction coefficients, also called the number of
    poles. Choose this number at least twice as large as the number of 
    spectral peaks that you want to detect. 

window_length : float, default=0.025
    Effective duration of each analysis frame in seconds. 

time_step : float, default=0.005
    Time step between two consecutive analysis frames in seconds.

preemphasis_frequency : float, default=50.0
    +6dB / octave filtering will be applied above this frequency. 
    If you do not want pre-emphasis, choose a frequency greater than the 
    Nyquist frequency. 

See Also
--------
:praat:`Sound: To LPC (covariance)...`
:object:`~parselmouth.LPC`
:func:`~parselmouth.Sound.to_lpc_autocorrelation`
:func:`~parselmouth.Sound.to_lpc_burg`
:func:`~parselmouth.Sound.to_lpc_marple`
)";

constexpr auto TO_LPC_BURG_DOCSTRING = R"(Create LPC using Burg's method.

Run linear predictive coding (LPC) analysis with the Burg's method and 
returns a new :object:`~parselmouth.LPC` object containing the analysis 
outcomes.

Burg's algorithm is described in Anderson (1978)

Warning
-------
You are advised not to use this command for formant analysis. For formant
analysis, instead use :func:`~parselmouth.Sound.to_formant_burg`, which 
also works via LPC.

Parameters
----------
prediction_order : int, default=16
    Number of linear prediction coefficients, also called the number of
    poles. Choose this number at least twice as large as the number of 
    spectral peaks that you want to detect. 

window_length : float, default=0.025
    Effective duration of each analysis frame in seconds. 

time_step : float, default=0.005
    Time step between two consecutive analysis frames in seconds.

preemphasis_frequency : float, default=50.0
    +6dB / octave filtering will be applied above this frequency. If you do
    not want pre-emphasis, choose a frequency greater than the Nyquist 
    frequency. 

See Also
--------
:praat:`Sound: To LPC (burg)...`
:object:`~parselmouth.LPC`
:func:`~parselmouth.Sound.to_lpc_autocorrelation`
:func:`~parselmouth.Sound.to_lpc_covariance`
:func:`~parselmouth.Sound.to_lpc_marple`
)";

constexpr auto TO_LPC_MARPLE_DOCSTRING = R"(Create LPC using Marple's method.

Run linear predictive coding (LPC) analysis with the Marple's method and
returns a new :object:`~parselmouth.LPC` object containing the analysis 
outcomes.

The algorithm is described in Marple (1980).

Warning
-------
You are advised not to use this command for formant analysis. For formant
analysis, instead use :func:`~parselmouth.Sound.to_formant_burg`, which 
also works via LPC.

Parameters
----------
prediction_order : int, default=16
    Number of linear prediction coefficients, also called the number of
    poles. Choose this number at least twice as large as the number of 
    spectral peaks that you want to detect. 

window_length : float, default=0.025
    Effective duration of each analysis frame in seconds. 

time_step : float, default=0.005
    Time step between two consecutive analysis frames in seconds.

preemphasis_frequency : float, default=50.0
    +6dB / octave filtering will be applied above this frequency. If you do
    not want pre-emphasis, choose a frequency greater than the Nyquist 
    frequency. 

tolerance1 : float, default=1e-6
    Stop the iteration when E(m) / E(0) < tolerance1, where E(m) is the 
    prediction error for order m. 

tolerance 2 : float, default=1e-6
    Stop the iteration when (E(m) - E(m-1)) / E(m-1) < tolerance2. 

See Also
--------
:praat:`Sound: To LPC (marple)...`
:object:`~parselmouth.LPC`
:func:`~parselmouth.Sound.to_lpc_autocorrelation`
:func:`~parselmouth.Sound.to_lpc_covariance`
:func:`~parselmouth.Sound.to_lpc_burg`
)";

}// namespace parselmouth
