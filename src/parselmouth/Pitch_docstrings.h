namespace parselmouth
{

constexpr auto GET_MEAN_STRENGTH_DOCSTRING = R"(Get mean pitch strength measure

Returns the value of user-selected measure of the periodicity strength.

Parameters
----------
from_time : double, default=0.0
    The start time of the compuation. Use 0.0 to start with the first 
    available frame

end_time : double, default=0.0
    The end time of the compuation. Use 0.0 to end with the last available
    frame

type : {'ac', 'nhr', 'hnr_db'}, default="hnr_db"
    Type of strength measure to compute

See Also
--------
:praat:`Voice report`
)";


constexpr auto GET_MEAN_DOCSTRING = R"(Get mean frequency

Returns the mean of periodicity frequency in the specified unit.

Parameters
----------
from_time : double, default=0.0
    The start time of the compuation. Use 0.0 to start with the first 
    available frame

end_time : double, default=0.0
    The end time of the compuation. Use 0.0 to end with the last available
    frame

unit : {'HERTZ','HERTZ_LOGARITHMIC','MEL','LOG_HERTZ','SEMITONES_1',
        'SEMITONES_100','SEMITONES_200','SEMITONES_440','ERB'},
       default=HERTZ
    Frequency unit

See Also
--------
:praat:`Voice report`
)";

constexpr auto GET_MAXIMUM_DOCSTRING = R"(Get maximum frequency

Returns the maximum frequency in the specified unit

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Pitch object is considered. 

unit : {'HERTZ','HERTZ_LOGARITHMIC','MEL','LOG_HERTZ','SEMITONES_1',
        'SEMITONES_100','SEMITONES_200','SEMITONES_440','ERB'},
       default=HERTZ
    Frequency unit

interpolate : bool, default=True
    True to evalaute parabolically interpolated pitch peaks; False to 
    select the raw frequency samples.

See Also
--------
:praat:`Voice report`
)";

constexpr auto GET_MINIMUM_DOCSTRING = R"(Get minimum frequency

Returns the minimum frequency in the specified unit

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Pitch object is considered. 

unit : {'HERTZ','HERTZ_LOGARITHMIC','MEL','LOG_HERTZ','SEMITONES_1',
        'SEMITONES_100','SEMITONES_200','SEMITONES_440','ERB'},
       default=HERTZ
    Frequency unit

interpolate : bool, default=True
    True to evalaute parabolically interpolated pitch peaks; False to 
    select the raw frequency samples.

See Also
--------
:praat:`Voice report`
)";

constexpr auto GET_STANDARD_DEVIATION_DOCSTRING = R"(Get standard deviation of frequency

Returns the standard deviation of frequency samples in the specified unit

Parameters
----------
from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Pitch object is considered. 

unit : {'HERTZ','HERTZ_LOGARITHMIC','MEL','LOG_HERTZ','SEMITONES_1',
        'SEMITONES_100','SEMITONES_200','SEMITONES_440','ERB'},
       default=HERTZ
    Frequency unit

See Also
--------
:praat:`Voice report`
)";


constexpr auto GET_QUANTILE_DOCSTRING = R"(Get quantile of frequency samples

Returns the quantile of the frequency samples in the specified unit

Parameters
----------
quantile : double
    Quantile to compute, which must be between 0 and 1 inclusive.

from_time : double=0.0
    The starting time of the analysis time domain.

to_time : double=0.0
    The ending time of the analysis time domain. Values outside this domain
    are ignored. If `to_time` is not greater than `from_time`, the entire
    time domain of the Pitch object is considered. 

unit : {'HERTZ','HERTZ_LOGARITHMIC','MEL','LOG_HERTZ','SEMITONES_1',
        'SEMITONES_100','SEMITONES_200','SEMITONES_440','ERB'},
       default=HERTZ
    Frequency unit

See Also
--------
:praat:`Voice report`
)";

}// namespace parselmouth