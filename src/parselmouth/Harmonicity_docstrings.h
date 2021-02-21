namespace parselmouth {


constexpr auto HARMONICITY_CLASS_DOCSTRING = R"(Praat Harmonicity.

A sequence object contain measures of the degree of acoustic periodicity,
also called Harmonics-to-Noise Ratio (HNR). Harmonicity is expressed in dB.

In the most use cases, `parselmouth.Harmonicity` objects are generated from
`parselmouth.Sound` objects with the `parselmouth.Sound.to_harmonicity()`
method. To constuct manually, use the following parameters.

Parameters
----------
data : array_like of float
    Harmonicity measure samples

time_step : float, optional
    Time increment between two successive samples in seconds. If omitted,
    a valid `end_time` must be specified.

start_time : float, default=0.0
    Starting time of the analysis domain in seconds

end_time : float, default=0.0
    Ending time of the analysis domain in seconds. If <= `t1`, it is set to
    cover all the samples in `data.

t1 : float, optional
    Time of the first harmonicity sample in seconds. If omitted, it is set
    to `start_time`.

Attributes
----------
t1   : float, readonly
    Time of the first harmonicity sample in seconds
time_step : float, readonly
    Time increment between two successive samples in seconds
start_time : float, readonly
    Starting time of the analysis domain in seconds
end_time : float, readonly
    Ending time of the analysis domain in seconds

Methods
-------
get_value(time, interpolation="CUBIC")
    Get (interpolated) harmonicity value at the specified `time`

get_value_in_frame(frame_number)
    Get harmonicity value at the specified analysis (1-based) frame number

formula(formula)
    Modify the harmonicity data according to Praat script command

get_maximum(from_time=0.0, to_time=0.0, interpolation="PARABOLIC")
    Get maximum harmonicity value in the specified time range

get_mean(from_time=0.0, to_time=0.0)
    Get mean harmonicity value in the specified time range

get_minimum(from_time=0.0, to_time=0.0, interpolation="PARABOLIC")
    Get minimum harmonicity value in the specified time range

get_standard_deviation(from_time=0.0, to_time=0.0)
    Get standard deviation of the harmonicity values in the specified time
    range

get_time_of_maximum(from_time=0.0, to_time=0.0, interpolation="PARABOLIC")
    Get the time when the maximum value is observed in the time range

get_time_of_minimum(from_time=0.0, to_time=0.0, interpolation="PARABOLIC")
    Get the time when the minimum value is observed in the time range

get_quantile(quantile)
    Get the quantile of the harmonicity samples, expressed in dB

See Also
--------
:praat:`Harmonicity`
)";

constexpr auto GET_VALUE_DOCSTRING = R"(Get estimated local HNR

Returns the HNR estimate (in dB) at a specified time. If this time is 
outside the time domain or outside the samples of the Harmonicity, the 
result is undefined.

Parameters
----------
time : double
    The time at which the value is evaluated

interpolation : {'NEAREST', 'LINEAR', 'CUBIC', 'SINC70', 'SINC700'},
                default='CUBIC'
    The interpolation method.

See Also
--------
:praat:`Harmonicity: Get value at time...`
)";

constexpr auto GET_VALUE_IN_FRAME_DOCSTRING = R"(Get local HNR

Returns the HNR in dB. If the index is less than 1 or greater than the
number of frames, the result is undefined.

Parameters
----------
frame_number : int
    The 1-base index of the target frame 

See Also
--------
:praat:`Harmonicity: Get value in frame...`
)";

constexpr auto GET_MAXIMUM_DOCSTRING = R"(Get maximum HNR

Returns the maximum HNR, expressed in dB.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

interpolation : {'NONE', 'PARABOLIC', 'CUBIC', 'SINC70', 'SINC700'},
                default='PARABOLIC'
    The method of vector peak interpolation.

See Also
--------
:praat:`Harmonicity: Get maximum...`
)";

constexpr auto GET_MEAN_DOCSTRING = R"(Get mean HNR

Returns the mean HNR, expressed in dB.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

See Also
--------
:praat:`Harmonicity: Get mean...`
)";

constexpr auto GET_MINIMUM_DOCSTRING = R"(Get minimum HNR

Returns the minimum HNR, expressed in dB.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

interpolation : {'NONE', 'PARABOLIC', 'CUBIC', 'SINC70', 'SINC700'},
                default='PARABOLIC'
    The method of vector peak interpolation.

See Also
--------
:praat:`Harmonicity: Get minimum...`
)";

constexpr auto GET_STANDARD_DEVIATION_DOCSTRING = R"(Get HNR standard deviation

Returns the standard deviation of the HNR samples, expressed in dB.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

See Also
--------
:praat:`Harmonicity: Get standard deviation...`
)";

constexpr auto GET_TIME_OF_MAXIMUM_DOCSTRING = R"(Get time of maximum HNR

Returns the time (in seconds) associated with the maximum HNR value.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

interpolation : {'NONE', 'PARABOLIC', 'CUBIC', 'SINC70', 'SINC700'},
                default='PARABOLIC'
    The method of vector peak interpolation.

See Also
--------
:praat:`Harmonicity: Get time of maximum...`
)";

constexpr auto GET_TIME_OF_MINIMUM_DOCSTRING = R"(Get time of minimum HNR

Returns the time (in seconds) associated with the minimum HNR value.

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Harmonicity object is considered. 

interpolation : {'NONE', 'PARABOLIC', 'CUBIC', 'SINC70', 'SINC700'},
                default='PARABOLIC'
    The method of vector peak interpolation.

See Also
--------
:praat:`Harmonicity: Get time of minimum...`
)";

constexpr auto GET_QUANTILE_DOCSTRING = R"(Get quantile of HNR samples

Returns the quantile of the HNR samples, expressed in dB.

Parameters
----------
quantile : double
    Quantile to compute, which must be between 0 and 1 inclusive.
)";

}// namespace parselmouth