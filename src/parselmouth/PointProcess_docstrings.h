namespace parselmouth
{

#define GET_RANGE_PARAMETER_DOCSTRING                                              \
    "from_time : double \n"                                                        \
    "    The start time of the part of the PointProcess to be measured in\n"       \
    "    seconds. If 0.0, all the points to `start_time` are included.\n"          \
    "    (default: 0.0)\n"                                                         \
    "\n"                                                                           \
    "end_time : double \n"                                                         \
    "    The end time of the part of the PointProcess to be measured in \n"        \
    "    seconds. If 0.0, all the points to `end_time` are included. \n"           \
    "    (default: 0.0) \n"                                                        \
    "\n"                                                                           \
    "period_floor : double \n"                                                     \
    "    The shortest possible interval to be used in the computation in \n"       \
    "    seconds. If an interval is shorter than this, it will be ignored (and \n" \
    "    the previous and next intervals will not be regarded as consecutive). \n" \
    "    This setting will normally be very small. (default: 0.0001). \n"          \
    "\n"                                                                           \
    "period_ceiling : double \n"                                                   \
    "    The longest possible interval that to be used in the computation in \n"   \
    "    seconds. If an interval is longer than this, it will be ignored (and \n"  \
    "    the previous and next intervals will not be regarded as consecutive). \n" \
    "    For example, if the minimum frequency of periodicity is 50 Hz, set \n"    \
    "    this setting to 0.02 seconds; intervals longer than that could be \n"     \
    "    regarded as voiceless stretches and will be ignored. (default: 0.02) \n"  \
    "\n"                                                                           \
    "maximum_period_factor : double \n"                                            \
    "    The largest possible difference between consecutive intervals that to \n" \
    "    be used in the computation. If the ratio of the durations of two  \n"     \
    "    consecutive intervals is greater than this, this pair of intervals \n"    \
    "    will be ignored (each of the intervals could still take part in the \n"   \
    "    computation in a comparison with its neighbour on the other side). \n"    \
    "    (default: 1.3)"

#define GET_SHIMMER_RANGE_PARAMETER_DOCSTRING                                                            \
    "sound : Parselmouth.Sound \n"                                                                       \
    "    Sound object containing the samples to evaluate the amplitude \n" GET_RANGE_PARAMETER_DOCSTRING \
    "maximum_amplitude_factor : double \n"                                                               \
    "    Maximum amplitude factor \n"                                                                    \
    "\n"                                                                                                 \
    "See Also \n"                                                                                        \
    "-------- \n"                                                                                        \
    ":praat:`Voice 3. Shimmer` \n"

    constexpr auto CREATE_POISSON_PROCESS_DOCSTRING = R"(Create a PointProcess instance with Poisson-distributed random time points.

Returns a new PointProcess instance that represents a Poisson process. 
A Poisson process is a stationary point process with a fixed density $λ$, 
which means that there are, on the average, $λ$ events per second.

Parameters
----------
start_time : double
    $t_{min}$, the beginning of the time domain, in seconds. (default: 0.0)
end_time : double
    $t_{max}$, the end of the time domain, in seconds. (default: 1.0)
double : density
    The average number of points per second. (default: 100.0)

See Also
--------
:praat:`Create Poisson process...`
)";

    constexpr auto FROM_PITCH_DOCSTRING = R"(Create PointProcess from Pitch object.

Returns a new PointProcess instance which is generated from the specified 
Pitch object. The acoustic periodicity contour stored in the Pitch object 
is used as the frequency of an underlying point process (such as the 
sequence of glottal closures in vocal-fold vibration).

Parameters
----------
pitch : Parselmouth.Pitch
    Pitch object defining the periodicity contour

See Also
--------
:praat:`Pitch: To PointProcess`
)";

    constexpr auto FROM_SOUND_PITCH_CC_DOCSTRING = R"(Create PointProcess from Sound and Pitch objects using crosscorrelation.

Returns a new PointProcess instance, generated from the specified Sound 
and Pitch instances using the cross-correlation method. The resulting 
instance contains voiced and unvoiced intervals according to `pitch` 
object, and the voiced intervals are further divided into fundamental 
periods of voice, identified by cross-correlating the sound samples.

Parameters
----------
sound : Parselmouth.Sound
    Sound object containing the target sound waveform

pitch : Parselmouth.Pitch
    Pitch object defining the periodicity contour of `sound`

See Also
--------
:praat:`Sound & Pitch: To PointProcess (cc)`
)";

    constexpr auto FROM_SOUND_PITCH_PEAKS_DOCSTRING = R"(Create PointProcess from Sound and Pitch objects using peak-picking.

Returns a new PointProcess instance, generated from the specified Sound 
and Pitch instances using the peak-picking method. The resulting 
instance contains voiced and unvoiced intervals according to `pitch` 
object, and the voiced intervals are further divided into fundamental 
periods of voice, identified by peak-picking the sound samples.

The periods that are found in this way are much more variable than those 
found by `from_sound_pitch_cc()` and therefore less useful for analysis 
and subsequent overlap-add synthesis.

Parameters
----------
sound : Parselmouth.Sound
    Sound object containing the target sound waveform

pitch : Parselmouth.Pitch
    Pitch object defining the periodicity contour of `sound`

See Also
--------
:praat:`Sound & Pitch: To PointProcess (peaks)...`
)";

    constexpr auto GET_NUMBER_OF_POINTS_DOCSTRING = R"(Get the number of time points.

Returns the total number of time points defined in the PointProcess 
instance)";

    constexpr auto GET_NUMBER_OF_PERIODS_DOCSTRING = R"(Get the number of periods.

Get the number of periods within the specified time range

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_TIME_FROM_INDEX_DOCSTRING = R"(Get time associated with the point number.

Returns a time, specified by the time point number. If the number is not a 
valid, it returns None.

Parameters
----------
point_number : int
    Index (1-based) of the requested time point.
)";

    constexpr auto GET_JITTER_LOCAL_DOCSTRING = R"(Get jitter measure

Returns the average absolute difference between consecutive periods, 
divided by the average period. (MDVP Jitt: 1.040% as a threshold for 
pathology)

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local)...`
)";

    constexpr auto GET_JITTER_LOCAL_ABSOLUTE_DOCSTRING = R"(Get absolute jitter measure

Get the average absolute difference between consecutive periods, in 
seconds (MDVP Jita: 83.200 μs as a threshold for pathology)

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

    constexpr auto GET_JITTER_RAP_DOCSTRING = R"(Get Relative Average Perturbation measure.

Get the Relative Average Perturbation, the average absolute difference 
between a period and the average of it and its two neighbours, divided by 
the average period (MDVP: 0.680% as a threshold for pathology)

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (rap)...`
)";

    constexpr auto GET_JITTER_PPQ5_DOCSTRING = R"(Get 5-point PPQ measure

Get the five-point Period Perturbation Quotient, the average absolute 
difference between a period and the average of it and its four closest 
neighbours, divided by the average period (MDVP PPQ, and gives 0.840% as a
threshold for pathology)

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

    constexpr auto GET_JITTER_DDP_DOCSTRING = R"(Get Praat jitter measure

Get the average absolute difference between consecutive differences 
between consecutive periods, divided by the average period

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

    constexpr auto GET_COUNT_AND_FRACTION_OF_VOICE_BREAKS_DOCSTRING =
        R"(Get voice break analysis outputs

Returns a tuple, containing the outputs of the Praat voice break analysis:

    - the number of voice breaks
    - the degree of voice breaks (MDVP DVB)
    - the total duration of the voice breaks in seconds
    - the duration of the analysed part of the signal in seconds

Parameters
----------
from_time : double
    The start time of the part of the PointProcess to be measured in 
    seconds. If 0.0, all the points to `start_time` are included.
    (default: 0.0)

end_time : double
    The end time of the part of the PointProcess to be measured in
    seconds. If 0.0, all the points to `end_time` are included. 
    (default: 0.0)

period_ceiling : double
    The longest possible interval that to be used in the computation in 
    seconds. If an interval is longer than this, it will be ignored (and 
    the previous and next intervals will not be regarded as consecutive). 
    For example, if the minimum frequency of periodicity is 50 Hz, set 
    this setting to 0.02 seconds; intervals longer than that could be 
    regarded as voiceless stretches and will be ignored. (default: 0.02)

See Also
--------
:praat:`Voice 1. Voice breaks`
)";

    constexpr auto GET_SHIMMER_LOCAL_DOCSTRING = R"(Get shimmer measure

Returns the average absolute difference between the amplitudes of 
consecutive periods, divided by the average amplitude (MDVP Shim: 3.810% 
as a threshold for pathology)

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_SHIMMER_LOCAL_DB_DOCSTRING = R"(Get shimmer measure in dB

Returns the average absolute base-10 logarithm of the difference between 
the amplitudes of consecutive periods, multiplied by 20 (MDVP ShdB: 
0.350 dB as a threshold for pathology)

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_SHIMMER_LOCAL_APQ3_DOCSTRING = R"(Get 3-point APQ

Returns the three-point Amplitude Perturbation Quotient, the average 
absolute difference between the amplitude of a period and the average of 
the amplitudes of its neighbours, divided by the average amplitude

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_SHIMMER_LOCAL_APQ5_DOCSTRING = R"(Get 5-point APQ

Returns the five-point Amplitude Perturbation Quotient, the average  
absolute difference between the amplitude of a period and the average of 
the amplitudes of it and its four closest neighbours, divided by the 
average amplitude

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_SHIMMER_LOCAL_APQ11_DOCSTRING = R"(Get shimmer measure

Returns the 11-point Amplitude Perturbation Quotient, the average absolute 
difference between the amplitude of a period and the average of the 
amplitudes of it and its ten closest neighbours, divided by the average 
amplitude (MDVP APQ: 3.070% as a threshold for pathology)

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_SHIMMER_LOCAL_DDA_DOCSTRING = R"(Get Praat shimmer measure

Returns the average absolute difference between consecutive differences 
between the amplitudes of consecutive periods (three times APQ3)

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_LOW_INDEX_DOCSTRING = R"(Get nearest point below

Returns the 1-base index of the nearest point before or at the specified 
time. If the point process contains no points or the specified time is 
before the first point, returns 0.

Parameters
----------
time : double
    The time from which a point is looked for in seconds

See Also
--------
:praat:`PointProcess: Get low index...`
)";

    constexpr auto GET_HIGH_INDEX_DOCSTRING = R"(Get nearest point above

Returns the 1-base index of the nearest point at or after the specified 
time. If the point process contains no points or the specified time is 
after the last point, returns 0.

Parameters
----------
time : double
    The time from which a point is looked for in seconds

See Also
--------
:praat:`PointProcess: Get high index...`
)";

    constexpr auto GET_NEAREST_INDEX_DOCSTRING = R"(Get nearest point

Returns the 1-base index of the point nearest to the specified time. If 
the point process contains no points or the specified time is before the 
first point or after the last point, returns 0.

Parameters
----------
time : double
    The time from which a point is looked for in seconds

See Also
--------
:praat:`PointProcess: Get nearest index...`
)";

    constexpr auto GET_WINDOW_POINTS_DOCSTRING = R"(Get included point range

Returns the 1-base starting and ending indices of the time points inside
the specified time range.

Parameters
----------
from_time : double
    The starting time in seconds

to_time : double
    The ending time in seconds
)";

    constexpr auto GET_INTERVAL_DOCSTRING = R"(Get period duration

Returns the duration of the period interval around a specified time.

Parameters
----------
time : double
    The time from which a point is looked for in seconds

See Also
--------
:praat:`PointProcess: Get interval...`
)";

    constexpr auto GET_MEAN_PERIOD_DOCSTRING = R"(Get mean period

Returns the average period in the specified time range.

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

    constexpr auto GET_STDEV_PERIOD_DOCSTRING = R"(Get SD of periods

Returns the standard deviation of the periods in the specified time range.

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

    constexpr auto UNION_DOCSTRING = R"(Combine with another time process

Returns a new PointProcess instance containing all the points of the two 
original point processes, sorted by time.

Parameters
----------
other : Parselmouth.PointProcess
    The other PointProcess object to combine with `self`

See Also
--------
:praat:`PointProcesses: Union`
)";

    constexpr auto INTERSECTION_DOCSTRING = R"(Intersect with another time process

Returns a new PointProcess instance containing only those points that occur
in both `self` and `other` PointProcess objects

Parameters
----------
other : Parselmouth.PointProcess
    The other PointProcess object to intersect with `self`

See Also
--------
:praat:`PointProcesses: Intersection`
)";

    constexpr auto DIFFERENCE_DOCSTRING = R"(Subtract another time process

Returns a new PointProcess instance containing only those points of the 
`self` point process that do not occur in the `other` point process

Parameters
----------
other : Parselmouth.PointProcess
    The other PointProcess object to subtract from `self`

See Also
--------
:praat:`PointProcesses: Difference`
)";

    constexpr auto ADD_POINT_DOCSTRING = R"(Add time point

Add the specified time point. If the point already exists in the point
process, nothing happens.

Parameters
----------
time : double
    Time to be added

See Also
--------
:praat:`PointProcess: Add point...`
)";

    constexpr auto ADD_POINTS_DOCSTRING = R"(Add time points

Add the specified time points. If any of the points already exists in the 
point process, nothing happens for that point.

Parameters
----------
times : numpy.ndarray<double>
    Array of times to be added
)";

    constexpr auto REMOVE_POINT_DOCSTRING = R"(Remove time point

"Remove a time point specified by (1-base index) point_number"

Remove the specified time point. (e.g., if `point_number` is 3, the third 
point is removed) It does nothing if index is less than 1 or greater than 
the number of points in the point process. 

Parameters
----------
point_number : int
    1-based index of time point to remove

See Also
--------
:praat:`PointProcess: Remove point...`
)";

    constexpr auto REMOVE_POINT_NEAR_DOCSTRING = R"(Remove nearest time point

Remove a time point nearest to the specified time. It does nothing if 
there are no points in the point process.

Parameters
----------
time : double
    Time to be removed

See Also
--------
:praat:`PointProcess: Remove point near...`
)";

    constexpr auto REMOVE_POINTS_DOCSTRING = R"(Remove a range of time points

Remove all the time point that originally fell in the range 
[from_point_number, to_point_number].

Parameters
----------
from_point_number : int
    Starting 1-based time point index

to_point_number : int
    Ending 1-based time point index

See Also
--------
:praat:`PointProcess: Remove points...`
)";

    constexpr auto REMOVE_POINTS_BETWEEN_DOCSTRING = R"(Remove time points in a time range

Remove all points that originally fell in the domain [from_time, to_time],
including the edges.

Parameters
----------
from_time : double
    Starting time in seconds

to_time : double
    Ending time in seconds

See Also
--------
:praat:`PointProcess: Remove points between...`
)";

    constexpr auto FILL_DOCSTRING = R"(Add equispaced time points

Add equispaced time points between the specified time range separated by 
the specified period.

Parameters
----------
from_time : double
    Starting time in seconds

to_time : double
    Ending time in seconds

period : double
    Time interval in seconds (default: 0.01)
)";

    constexpr auto VOICE_DOCSTRING = R"(Add equispaced time points in unvoiced intervals

Add equispaced time points separated by the specified period over all
existing periods longer than maximum_voiced_period

Parameters
----------
from_time : double
    Starting time in seconds

to_time : double
    Ending time in seconds

period : double
    Time interval in seconds (default: 0.01)

maximum_voiced_period : double
    Time period longer than this is considered unvoiced, in seconds 
    (default: 0.02000000001)
)";
}