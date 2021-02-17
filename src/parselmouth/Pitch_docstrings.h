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
}