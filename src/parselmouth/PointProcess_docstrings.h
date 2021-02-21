namespace parselmouth
{

constexpr auto CREATE_CLASS_DOCSTRING = R"(Praat PointProcess.

A sequence object contain a sequence of points $t_i$ in time, defined
on a domain [`xmin`, `xmax`]. The points are sorted in time, i.e., 
$t_i+1 > t_i$.

Attributes
----------
tmin : float, readonly
    Starting time of the analysis domain in seconds
tmax : float, readonly
    Ending time of the analysis domain in seconds

Methods
-------
get_number_of_points()
    Get the number of time points

get_number_of_periods()
    Get the number of periods

get_count_and_fraction_of_voice_breaks(from_time=0.0, to_time=0.0, 
                                       period_ceiling=0.02)
    Get voice break analysis outputs

get_mean_period(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get average of voice periods

get_stdev_period(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get standard deviation of voice periods

get_jitter_ddp(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get Praat jitter measure

get_jitter_local(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get jitter measure (MDVP Jitt)

get_jitter_local_absolute(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get absolute jitter measure (MDVP Jita)

get_jitter_ppq5(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get 5-point PPQ measure (MDVP PPQ)

get_jitter_rap(from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3)
    Get Relative Average Perturbation measure (MDVP RAP)

get_shimmer_local(sound, from_time=0.0, to_time=0.0, period_floor=0.0001, 
                period_ceiling=0.02, maximum_period_factor=1.3, 
                maximum_amplitude_factor=1.6)
    Get shimmer measure (MDVP Shim)
                
get_shimmer_local_apq3(sound, from_time=0.0, to_time=0.0, 
                period_floor=0.0001, period_ceiling=0.02, 
                maximum_period_factor=1.3, maximum_amplitude_factor=1.6)
    Get 3-point APQ

get_shimmer_local_apq5(sound, from_time=0.0, to_time=0.0, 
                period_floor=0.0001, period_ceiling=0.02, 
                maximum_period_factor=1.3, maximum_amplitude_factor=1.6)
    Get 5-point APQ

get_shimmer_local_apq11(sound, from_time=0.0, to_time=0.0, 
                period_floor=0.0001, period_ceiling=0.02, 
                maximum_period_factor=1.3, maximum_amplitude_factor=1.6)
    Get 11-point APQ (MDVP APQ)

get_shimmer_local_dB(sound, from_time=0.0, to_time=0.0, 
                period_floor=0.0001, period_ceiling=0.02, 
                maximum_period_factor=1.3, maximum_amplitude_factor=1.6)
    Get shimmer measure in dB (MDVP ShdB)

get_shimmer_local_dda(sound, from_time=0.0, to_time=0.0, 
                period_floor=0.0001, period_ceiling=0.02,
                maximum_period_factor=1.3, maximum_amplitude_factor=1.6)
    Get Praat shimmer measure

get_nearest_index(time)
    Get nearest point

get_high_index(time)
    Get nearest point above

get_low_index(time)
    Get nearest point below

get_interval(time)
    Get points surrounding the time

get_window_points(from_time, to_time)
    Get start and end points of included point range

add_point(time)
    Add time point

add_points(times)
    Add multiple time points

remove_point(point_number)
    Remove time point

remove_point_near(time)
    Remove the nearest time point 

remove_points(from_point_number, to_point_number)
    Remove a range of time points

remove_points_between(from_time, to_time)
    Remove time points in a time range

fill(from_time, to_time, period=0.01)
    Add equispaced time points

voice(from_time, to_time, period=0.01, maximum_voiced_period=0.02000000001)
    Add equispaced time points in unvoiced intervals

union(other)
    Combine with another time process

intersection(other)
    Intersect with another time process

difference(other)
    Subtract another time process

transplant_domain(sound)
    Copy time domain


Static Method
-------------
create_poisson_process(start_time=0.0, end_time=1.0, density=100.0)
    Create a PointProcess instance with Poisson-distributed random time 
    points

See Also
--------
:praat:`PointProcess`


)";

#define GET_RANGE_PARAMETER_DOCSTRING                                          \
  "from_time : float \n"                                                       \
  "    The start time of the part of the PointProcess to be measured in\n"     \
  "    seconds. If 0.0, all the points to `start_time` are included.\n"        \
  "    (default: 0.0)\n"                                                       \
  "\n"                                                                         \
  "end_time : float \n"                                                        \
  "    The end time of the part of the PointProcess to be measured in \n"      \
  "    seconds. If 0.0, all the points to `end_time` are included. \n"         \
  "    (default: 0.0) \n"                                                      \
  "\n"                                                                         \
  "period_floor : float \n"                                                    \
  "    The shortest possible interval to be used in the computation in \n"     \
  "    seconds. If an interval is shorter than this, it will be ignored (and " \
  "\n"                                                                         \
  "    the previous and next intervals will not be regarded as consecutive). " \
  "\n"                                                                         \
  "    This setting will normally be very small. (default: 0.0001). \n"        \
  "\n"                                                                         \
  "period_ceiling : float \n"                                                  \
  "    The longest possible interval that to be used in the computation in \n" \
  "    seconds. If an interval is longer than this, it will be ignored (and "  \
  "\n"                                                                         \
  "    the previous and next intervals will not be regarded as consecutive). " \
  "\n"                                                                         \
  "    For example, if the minimum frequency of periodicity is 50 Hz, set \n"  \
  "    this setting to 0.02 seconds; intervals longer than that could be \n"   \
  "    regarded as voiceless stretches and will be ignored. (default: 0.02) "  \
  "\n"                                                                         \
  "\n"                                                                         \
  "maximum_period_factor : float \n"                                           \
  "    The largest possible difference between consecutive intervals that to " \
  "\n"                                                                         \
  "    be used in the computation. If the ratio of the durations of two  \n"   \
  "    consecutive intervals is greater than this, this pair of intervals \n"  \
  "    will be ignored (each of the intervals could still take part in the \n" \
  "    computation in a comparison with its neighbour on the other side). \n"  \
  "    (default: 1.3)"

#define GET_SHIMMER_RANGE_PARAMETER_DOCSTRING                                  \
  "sound : Parselmouth.Sound \n"                                               \
  "    Sound object containing the samples to evaluate the amplitude "         \
  "\n" GET_RANGE_PARAMETER_DOCSTRING "maximum_amplitude_factor : float \n"     \
  "    Maximum amplitude factor \n"                                            \
  "\n"                                                                         \
  "See Also \n"                                                                \
  "-------- \n"                                                                \
  ":praat:`Voice 3. Shimmer` \n"

constexpr auto CONSTRUCTOR_EMPTY_DOCSTRING =
    R"(Create an empty PointProcess.

Returns an empty PointProcess instance.

Parameters
----------
start_time : float
    $t_{min}$, the beginning of the time domain, in seconds. (default: 0.0)
end_time : float
    $t_{max}$, the end of the time domain, in seconds. (default: 1.0)

See Also
--------
:praat:`Create empty PointProcess...`
)";

constexpr auto CONSTRUCTOR_FILLED_DOCSTRING =
    R"(Create a PointProcess filled with time points.

Returns a new PointProcess instance that contains the time points 
specified.

Parameters
----------
times : sequence-like of float
    A sequence of time points in seconds to be added to the PointProcess.
start_time : float or None, default=None
    $t_{min}$, the beginning of the time domain, in seconds. If None, the 
    smallest value from `times` is used.
end_time : float or None, default=None
    $t_{max}$, the end of the time domain, in seconds. If None, the largest
    value from `times` is used.
)";

constexpr auto CREATE_POISSON_PROCESS_DOCSTRING =
    R"(Create a PointProcess instance with Poisson-distributed random time points.

Returns a new PointProcess instance that represents a Poisson process. 
A Poisson process is a stationary point process with a fixed density $λ$, 
which means that there are, on the average, $λ$ events per second.

Parameters
----------
start_time : float
    $t_{min}$, the beginning of the time domain, in seconds. (default: 0.0)
end_time : float
    $t_{max}$, the end of the time domain, in seconds. (default: 1.0)
float : density
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

constexpr auto GET_NUMBER_OF_POINTS_DOCSTRING =
    R"(Get the number of time points.

Returns the total number of time points defined in the PointProcess 
instance)";

constexpr auto GET_NUMBER_OF_PERIODS_DOCSTRING = R"(Get the number of periods.

Get the number of periods within the specified time range

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_TIME_FROM_INDEX_DOCSTRING =
    R"(Get time associated with the point number.

Returns a time, specified by the time point number. If the number is not a 
valid, it returns None.

Parameters
----------
point_number : int
    Index (1-based) of the requested time point.
)";

constexpr auto GET_JITTER_LOCAL_DOCSTRING = R"(Get jitter measure (MDVP Jitt)

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

constexpr auto GET_JITTER_LOCAL_ABSOLUTE_DOCSTRING =
    R"(Get absolute jitter measure (MDVP Jita)

Get the average absolute difference between consecutive periods, in 
seconds (MDVP Jita: 83.200 μs as a threshold for pathology)

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

constexpr auto GET_JITTER_RAP_DOCSTRING =
    R"(Get Relative Average Perturbation measure (MDVP RAP)

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

constexpr auto GET_JITTER_PPQ5_DOCSTRING =
    R"(Get 5-point PPQ measure (MDVP PPQ)

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
from_time : float
    The start time of the part of the PointProcess to be measured in 
    seconds. If 0.0, all the points to `start_time` are included.
    (default: 0.0)

end_time : float
    The end time of the part of the PointProcess to be measured in
    seconds. If 0.0, all the points to `end_time` are included. 
    (default: 0.0)

period_ceiling : float
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

constexpr auto GET_SHIMMER_LOCAL_DOCSTRING = R"(Get shimmer measure (MDVP Shim)

Returns the average absolute difference between the amplitudes of 
consecutive periods, divided by the average amplitude (MDVP Shim: 3.810% 
as a threshold for pathology)

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_LOCAL_DB_DOCSTRING =
    R"(Get shimmer measure in dB (MDVP ShdB)

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

constexpr auto GET_SHIMMER_LOCAL_APQ11_DOCSTRING =
    R"(Get 11-point APQ (MDVP APQ)

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
time : float
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
time : float
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
time : float
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
from_time : float
    The starting time in seconds

to_time : float
    The ending time in seconds

Returns
-------
tuple of float
    (start, end) 
)";

constexpr auto GET_INTERVAL_DOCSTRING = R"(Get period duration

Returns the duration of the period interval around a specified time.

Parameters
----------
time : float
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
time : float
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
times : numpy.ndarray<float>
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
time : float
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

constexpr auto REMOVE_POINTS_BETWEEN_DOCSTRING =
    R"(Remove time points in a time range

Remove all points that originally fell in the domain [from_time, to_time],
including the edges.

Parameters
----------
from_time : float
    Starting time in seconds

to_time : float
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
from_time : float
    Starting time in seconds

to_time : float
    Ending time in seconds

period : float
    Time interval in seconds (default: 0.01)
)";

constexpr auto VOICE_DOCSTRING =
    R"(Add equispaced time points in unvoiced intervals

Add equispaced time points separated by the specified period over all
existing periods longer than maximum_voiced_period

Parameters
----------
from_time : float
    Starting time in seconds

to_time : float
    Ending time in seconds

period : float
    Time interval in seconds (default: 0.01)

maximum_voiced_period : float
    Time period longer than this is considered unvoiced, in seconds 
    (default: 0.02000000001)
)";

constexpr auto TRANSPLANT_DOMAIN_DOCSTRING = R"(Copy time domain.

Copy the time domain of the specified `sound` object.

Parameters
----------
sound : Parselmouth.Sound
    Source sound object
)";

constexpr auto TO_TEXT_GRID_DOCSTRING = R"(Convert into a TextGrid

PointProcess object is converted to a sound object by genering a pulse at
every point in the point process. This pulse is filtered at the Nyquist
frequency of the resulting Sound by converting it into a sampled sinc
function.

Parameters
----------
tier_names : str
    A list of the names of the tiers that you want to create, separated by 
    spaces. 

point_tiers : str
    A list of the names of the tiers that you want to be point tiers; the 
    rest of the tiers will be interval tiers.

See also
--------
:praat:`PointProcess: To TextGrid...`
)";

constexpr auto TO_TEXT_GRID_VUV_DOCSTRING =
    R"(Convert into a Sound with voiced/unvoiced information

PointProcess object is converted to a sound object with voiced/unvoiced 
information.

Parameters
----------
maximum_period : float, default=0.02
    The maximum interval that will be consider part of a larger voiced 
    interval. 

mean_period : float, default=0.01
    Half of this value will be taken to be the amount to which a voiced
    interval will extend beyond its initial and final points. Mean period
    should be less than Maximum period, or you may get intervals with
    negative durations. 

See also
--------
:praat:`PointProcess: To TextGrid (vuv)...`
)";

constexpr auto TO_SOUND_PHONATION_DOCSTRING =
    R"(Convert into a glottal waveform Sound object

PointProcess object is converted to a sound object containing glottal
waveform at every point in the point process. Its shape depends on the 
settings `power1` and `power2` according to the formula

$U\(x\) = x^{power1} - x^{power2}$

where $x$ is a normalized time that runs from 0 to 1 and $U(x)$ is the 
normalized glottal flow in arbitrary units (the real unit is m^3/s).

Parameters
----------
sampling_frequency : float, default=44100.0
    The sampling frequency of the resulting Sound object

adaptation_factor : float, default=1.0
    The factor by which a pulse height will be multiplied if the pulse time
    is not within Maximum period from the previous pulse, and by which a
    pulse height will again be multiplied if the previous pulse time is not
    within `maximum_period` from the pre-previous pulse. This factor is 
    against abrupt starts of the pulse train after silences, and is 1.0 if 
    you do want abrupt starts after silences. 

maximum_period : float, default=0.05
    The minimal period that will be considered a silence in seconds.

open_phase: float, default=0.7
    Fraction of a period when the glottis is open

collision_phase : float, default=0.03
    Decay factor to ease the abrupt collision at closure

power1 : float, default=3.0
    First glottal flow shape coefficient

power2 : float, default=4.0
    Second glottal flow shape coefficient

See also
--------
:praat:`PointProcess: To Sound (phonation)...`
)";

constexpr auto TO_SOUND_PULSE_TRAIN_DOCSTRING =
    R"(Convert into a Sound with pulses

PointProcess object is converted to a sound object with a series of pulses,
each generated at every point in the point process. This pulse is filtered
at the Nyquist frequency of the resulting Sound by converting it into a
sampled sinc function.

Parameters
----------
sampling_frequency : float, default=44100.0
    The sampling frequency of the resulting Sound object

adaptation_factor : float, default=1.0
    The factor by which a pulse height will be multiplied if the pulse time
    is not within `adaptation_time` from the pre-previous pulse, and by
    which a pulse height will again be multiplied if the pulse time is not
    within `adaptation_time` from the previous pulse. This factor is
    against abrupt starts of the pulse train after silences, and is 1.0 if 
    you do want abrupt starts after silences. 
    
adaptation_time : float, default=0.05
    The minimal period that will be considered a silence

interpolation_depth : int, default=2000        
    The extent of the sinc function to the left and to the right of the 
    peak

See also
--------
:praat:`PointProcess: To Sound (pulse train)...`
)";

constexpr auto TO_SOUND_HUM_DOCSTRING = R"(Convert into a Sound with hum sound

PointProcess object is converted to a sound object with hum sound. A Sound
is created from pulses, followed by filtered by a sequence of second-order
filters that represent five formants.

See also
--------
:praat:`PointProcess: To Sound (hum)...`
)";

} // namespace parselmouth
