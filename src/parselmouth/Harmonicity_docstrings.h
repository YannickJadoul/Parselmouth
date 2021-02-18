namespace parselmouth
{

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

}