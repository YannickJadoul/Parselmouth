/*
 * Copyright (C) 2021  Yannick Jadoul and contributors
 *
 * This file is part of Parselmouth.
 *
 * Parselmouth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parselmouth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>
 */

#pragma once
#ifndef INC_PARSELMOUTH_POINTPROCESS_DOCSTRINGS_H
#define INC_PARSELMOUTH_POINTPROCESS_DOCSTRINGS_H

namespace parselmouth {

constexpr auto CREATE_CLASS_DOCSTRING = R"(Praat PointProcess.

A sequence object contain a sequence of points :math:`t_i` in time, defined
on a domain [``xmin``, ``xmax``]. The points are sorted in time, i.e.,
:math:`t_i+1 > t_i`.

Attributes
----------
n_points : float, readonly
    The number of time points
tmin : float, readonly
    Starting time of the analysis domain in seconds
tmax : float, readonly
    Ending time of the analysis domain in seconds

See Also
--------
:praat:`PointProcess`
)";

#define GET_RANGE_PARAMETER_DOCSTRING                                          \
  "from_time : float, optional\n"                                              \
  "    The start time of the part of the `PointProcess` to be measured in\n"   \
  "    seconds. If `None`, all the points from ``tmin`` are included.\n"       \
  "    (default: None)\n"                                                      \
  "\n"                                                                         \
  "end_time : float, optional\n"                                               \
  "    The end time of the part of the `PointProcess` to be measured in\n"     \
  "    seconds. If `None`, all the points to ``tmax`` are included.\n"         \
  "    (default: None)\n"                                                      \
  "\n"                                                                         \
  "period_floor : float\n"                                                     \
  "    The shortest possible interval to be used in the computation in\n"      \
  "    seconds. If an interval is shorter than this, it will be ignored (and\n"\
  "    the previous and next intervals will not be regarded as consecutive).\n"\
  "    This setting will normally be very small. (default: 0.0001).\n"         \
  "\n"                                                                         \
  "period_ceiling : float\n"                                                   \
  "    The longest possible interval that to be used in the computation in\n"  \
  "    seconds. If an interval is longer than this, it will be ignored (and\n" \
  "    the previous and next intervals will not be regarded as consecutive).\n"\
  "    For example, if the minimum frequency of periodicity is 50 Hz, set\n"   \
  "    this setting to 0.02 seconds; intervals longer than that could be\n"    \
  "    regarded as voiceless stretches and will be ignored. (default: 0.02)\n" \
  "\n"                                                                         \
  "maximum_period_factor : float, positive\n"                                  \
  "    The largest possible difference between consecutive intervals to\n"     \
  "    be used in the computation. If the ratio of the durations of two\n"     \
  "    consecutive intervals is greater than this, this pair of intervals\n"   \
  "    will be ignored (each of the intervals could still take part in the\n"  \
  "    computation in a comparison with its neighbour on the other side).\n"   \
  "    (default: 1.3)"

#define GET_SHIMMER_RANGE_PARAMETER_DOCSTRING                                  \
  "sound : parselmouth.Sound\n"                                                \
  "    Sound object containing the samples to evaluate the amplitude.\n"       \
  GET_RANGE_PARAMETER_DOCSTRING                                                \
  "maximum_amplitude_factor : float, positive\n"                               \
  "    Maximum amplitude factor. (default: 1.6)\n"                             \
  "\n"                                                                         \
  "See Also\n"                                                                 \
  "--------\n"                                                                 \
  ":praat:`Voice 3. Shimmer`\n"

constexpr auto CONSTRUCTOR_EMPTY_DOCSTRING =
	R"(Create an empty PointProcess.

Returns an empty PointProcess instance.

Parameters
----------
start_time : float
    :math:`t_{min}`, the beginning of the time domain, in seconds.
end_time : float
    :math:`t_{max}`, the end of the time domain, in seconds.

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
start_time : float, optional
    :math:`t_{min}`, the beginning of the time domain, in seconds. If
    `None`, the smallest value from ``times`` is used.
end_time : float, optional
    :math:`t_{max}`, the end of the time domain, in seconds. If `None`,
    the largest value from ``times`` is used.
)";

constexpr auto CREATE_POISSON_PROCESS_DOCSTRING =
	R"(Create a PointProcess instance with Poisson-distributed random time points.

Returns a new PointProcess instance that represents a Poisson process.
A Poisson process is a stationary point process with a fixed density :math:`\lambda`,
which means that there are, on the average, :math:`\lambda` events per second.

Parameters
----------
start_time : float, default: 0.0
    :math:`t_{min}`, the beginning of the time domain, in seconds.
end_time : float, default: 1.0
    :math:`t_{max}`, the end of the time domain, in seconds.
density : float, default: 100.0
    The average number of points per second.

See Also
--------
:praat:`Create Poisson process...`
)";

constexpr auto FROM_PITCH_DOCSTRING =
	R"(Create PointProcess from Pitch object.

Returns a new PointProcess instance which is generated from the specified
Pitch object. The acoustic periodicity contour stored in the Pitch object
is used as the frequency of an underlying point process (such as the
sequence of glottal closures in vocal-fold vibration).

Parameters
----------
pitch : parselmouth.Pitch
    Pitch object defining the periodicity contour.

See Also
--------
:praat:`Pitch: To PointProcess`
)";

constexpr auto GET_NUMBER_OF_POINTS_DOCSTRING =
	R"(Get the number of time points.

Returns the total number of time points defined in the PointProcess
instance.
)";

constexpr auto GET_NUMBER_OF_PERIODS_DOCSTRING = R"(Get the number of periods.

Get the number of periods within the specified time range.

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

constexpr auto GET_JITTER_LOCAL_DOCSTRING =
	R"(Get jitter measure (MDVP Jitt).

Returns the average absolute difference between consecutive periods,
divided by the average period. (MDVP Jitt: 1.040% as a threshold for
pathology).

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local)...`
)";

constexpr auto GET_JITTER_LOCAL_ABSOLUTE_DOCSTRING =
	R"(Get absolute jitter measure (MDVP Jita).

Get the average absolute difference between consecutive periods, in
seconds (MDVP Jita: 83.200 μs as a threshold for pathology).

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

constexpr auto GET_JITTER_RAP_DOCSTRING =
	R"(Get Relative Average Perturbation measure (MDVP RAP).

Get the Relative Average Perturbation, the average absolute difference
between a period and the average of it and its two neighbours, divided by
the average period (MDVP: 0.680% as a threshold for pathology).

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (rap)...`
)";

constexpr auto GET_JITTER_PPQ5_DOCSTRING =
	R"(Get 5-point PPQ measure (MDVP PPQ).

Get the five-point Period Perturbation Quotient, the average absolute
difference between a period and the average of it and its four closest
neighbours, divided by the average period (MDVP PPQ, and gives 0.840% as a
threshold for pathology).

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

constexpr auto GET_JITTER_DDP_DOCSTRING = R"(Get Praat jitter measure.

Get the average absolute difference between consecutive differences
between consecutive periods, divided by the average period.

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING R"(

See Also
--------
:praat:`PointProcess: Get jitter (local, absolute)...`
)";

constexpr auto GET_COUNT_AND_FRACTION_OF_VOICE_BREAKS_DOCSTRING =
	R"(Get voice break analysis outputs.

Returns a tuple, containing the outputs of the Praat voice break analysis:

    - the number of voice breaks
    - the degree of voice breaks (MDVP DVB)
    - the total duration of the voice breaks in seconds
    - the duration of the analysed part of the signal in seconds

Parameters
----------
from_time : float, optional
    The start time of the part of the PointProcess to be measured in
    seconds. If `None`, all the points from ``tmin`` are included.
    (default: None)

end_time : float, optional
    The end time of the part of the PointProcess to be measured in
    seconds. If `None`, all the points to ``tmax`` are included.
    (default: None)

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

constexpr auto GET_SHIMMER_LOCAL_DOCSTRING =
	R"(Get shimmer measure (MDVP Shim).

Returns the average absolute difference between the amplitudes of
consecutive periods, divided by the average amplitude (MDVP Shim: 3.810%
as a threshold for pathology).

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_LOCAL_DB_DOCSTRING =
	R"(Get shimmer measure in dB (MDVP ShdB).

Returns the average absolute base-10 logarithm of the difference between
the amplitudes of consecutive periods, multiplied by 20 (MDVP ShdB:
0.350 dB as a threshold for pathology).

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_APQ3_DOCSTRING =
	R"(Get 3-point APQ.

Returns the three-point Amplitude Perturbation Quotient, the average
absolute difference between the amplitude of a period and the average of
the amplitudes of its neighbours, divided by the average amplitude.

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_APQ5_DOCSTRING =
	R"(Get 5-point APQ.

Returns the five-point Amplitude Perturbation Quotient, the average
absolute difference between the amplitude of a period and the average of
the amplitudes of it and its four closest neighbours, divided by the
average amplitude.

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_APQ11_DOCSTRING =
	R"(Get 11-point APQ (MDVP APQ).

Returns the 11-point Amplitude Perturbation Quotient, the average absolute
difference between the amplitude of a period and the average of the
amplitudes of it and its ten closest neighbours, divided by the average
amplitude (MDVP APQ: 3.070% as a threshold for pathology).

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_SHIMMER_DDA_DOCSTRING =
	R"(Get Praat shimmer measure.

Returns the average absolute difference between consecutive differences
between the amplitudes of consecutive periods (three times APQ3).

Parameters
----------
)" GET_SHIMMER_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_LOW_INDEX_DOCSTRING =
	R"(Get nearest point below.

Returns the 1-base index of the nearest point before or at the specified
time. If the point process contains no points or the specified time is
before the first point, returns 0.

Parameters
----------
time : float
    The time from which a point is looked for in seconds.

See Also
--------
:praat:`PointProcess: Get low index...`
)";

constexpr auto GET_HIGH_INDEX_DOCSTRING =
	R"(Get nearest point above.

Returns the 1-base index of the nearest point at or after the specified
time. If the point process contains no points or the specified time is
after the last point, returns 0.

Parameters
----------
time : float
    The time from which a point is looked for in seconds.

See Also
--------
:praat:`PointProcess: Get high index...`
)";

constexpr auto GET_NEAREST_INDEX_DOCSTRING =
	R"(Get nearest point.

Returns the 1-base index of the point nearest to the specified time. If
the point process contains no points or the specified time is before the
first point or after the last point, returns 0.

Parameters
----------
time : float
    The time from which a point is looked for in seconds.

See Also
--------
:praat:`PointProcess: Get nearest index...`
)";

constexpr auto GET_WINDOW_POINTS_DOCSTRING =
	R"(Get included point range.

Returns the 1-base starting and ending indices of the time points inside
the specified time range.

Parameters
----------
from_time : float
    The starting time in seconds.

to_time : float
    The ending time in seconds.

Returns
-------
tuple of float
    (start, end)
)";

constexpr auto GET_INTERVAL_DOCSTRING =
	R"(Get period duration.

Returns the duration of the period interval around a specified time.

Parameters
----------
time : float
    The time from which a point is looked for in seconds

See Also
--------
:praat:`PointProcess: Get interval...`
)";

constexpr auto GET_MEAN_PERIOD_DOCSTRING =
	R"(Get mean period.

Returns the average period in the specified time range.

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

constexpr auto GET_STDEV_PERIOD_DOCSTRING =
	R"(Get standard deviation of periods.

Returns the standard deviation of the periods in the specified time range.

Parameters
----------
)" GET_RANGE_PARAMETER_DOCSTRING;

constexpr auto UNION_DOCSTRING =
	R"(Combine with another time process.

Returns a new `PointProcess` instance containing all the points of the two
original point processes, sorted by time.

Parameters
----------
other : parselmouth.PointProcess
    The other PointProcess object to combine with ``self``.

See Also
--------
:praat:`PointProcesses: Union`
)";

constexpr auto INTERSECTION_DOCSTRING =
	R"(Intersect with another time process.

Returns a new `PointProcess` instance containing only those points that
occur in both ``self`` and ``other`` `PointProcess` objects.

Parameters
----------
other : parselmouth.PointProcess
    The other PointProcess object to intersect with ``self``.

See Also
--------
:praat:`PointProcesses: Intersection`
)";

constexpr auto DIFFERENCE_DOCSTRING =
	R"(Subtract another time process.

Returns a new `PointProcess` instance containing only those points of the
``self`` point process that do not occur in the ``other`` point process.

Parameters
----------
other : parselmouth.PointProcess
    The other `PointProcess` object to subtract from ``self``.

See Also
--------
:praat:`PointProcesses: Difference`
)";

constexpr auto ADD_POINT_DOCSTRING =
	R"(Add time point.

Add the specified time point. If the point already exists in the point
process, nothing happens.

Parameters
----------
time : float
    Time point to be added.

See Also
--------
:praat:`PointProcess: Add point...`
)";

constexpr auto ADD_POINTS_DOCSTRING =
	R"(Add time points.

Add the specified time points. If any of the points already exists in the
point process, nothing happens for that point.

Parameters
----------
times : numpy.ndarray[float]
    Array of time points to be added.
)";

constexpr auto REMOVE_POINT_DOCSTRING =
	R"(Remove time point.

Remove the specified time point. (e.g., if ``point_number`` is 3, the third
point is removed) It does nothing if index is less than 1 or greater than
the number of points in the point process.

Parameters
----------
point_number : int
    1-based index of time point to remove.

See Also
--------
:praat:`PointProcess: Remove point...`
)";

constexpr auto REMOVE_POINT_NEAR_DOCSTRING =
	R"(Remove nearest time point.

Remove a time point nearest to the specified time. It does nothing if
there are no points in the point process.

Parameters
----------
time : float
    Time point to be removed.

See Also
--------
:praat:`PointProcess: Remove point near...`
)";

constexpr auto REMOVE_POINTS_DOCSTRING =
	R"(Remove a range of time points.

Remove all the time point that originally fell in the range
[from_point_number, to_point_number].

Parameters
----------
from_point_number : int
    Starting 1-based time point index.

to_point_number : int
    Ending 1-based time point index.

See Also
--------
:praat:`PointProcess: Remove points...`
)";

constexpr auto REMOVE_POINTS_BETWEEN_DOCSTRING =
	R"(Remove time points in a time range.

Remove all points that originally fell in the domain [from_time, to_time],
including the edges.

Parameters
----------
from_time : float
    Starting time in seconds.

to_time : float
    Ending time in seconds.

See Also
--------
:praat:`PointProcess: Remove points between...`
)";

constexpr auto FILL_DOCSTRING =
	R"(Add equispaced time points.

Add equispaced time points between the specified time range separated by
the specified period.

Parameters
----------
from_time : float, optional
    Starting time in seconds.

to_time : float, optional
    Ending time in seconds.

period : float
    Time interval in seconds. (default: 0.01)
)";

constexpr auto VOICE_DOCSTRING =
	R"(Add equispaced time points in unvoiced intervals.

Add equispaced time points separated by the specified period over all
existing periods longer than ``maximum_voiced_period``.

Parameters
----------
period : float
    Time interval in seconds. (default: 0.01)

maximum_voiced_period : float
    Time period longer than this is considered unvoiced, in seconds.
    (default: 0.02000000001)
)";

constexpr auto TRANSPLANT_DOMAIN_DOCSTRING =
	R"(Copy time domain.

Copy the time domain of the specified `Sound` object.

Parameters
----------
sound : parselmouth.Sound
    Source sound object.
)";

constexpr auto TO_TEXT_GRID_DOCSTRING =
	R"(Convert into a TextGrid.

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
	R"(Convert into a Sound with voiced/unvoiced information.

PointProcess object is converted to a sound object with voiced/unvoiced
information.

Parameters
----------
maximum_period : float
    The maximum interval that will be consider part of a larger voiced
    interval. (default: 0.02)

mean_period : float
    Half of this value will be taken to be the amount to which a voiced
    interval will extend beyond its initial and final points. Mean period
    should be less than Maximum period, or you may get intervals with
    negative durations. (default: 0.01)

See also
--------
:praat:`PointProcess: To TextGrid (vuv)...`
)";

constexpr auto TO_SOUND_PULSE_TRAIN_DOCSTRING =
	R"(Convert into a Sound with pulses

PointProcess object is converted to a sound object with a series of pulses,
each generated at every point in the point process. This pulse is filtered
at the Nyquist frequency of the resulting Sound by converting it into a
sampled sinc function.

Parameters
----------
sampling_frequency : float
    The sampling frequency of the resulting `Sound` object.
    (default: 44100.0)

adaptation_factor : float
    The factor by which a pulse height will be multiplied if the pulse time
    is not within ``adaptation_time`` from the pre-previous pulse, and by
    which a pulse height will again be multiplied if the pulse time is not
    within ``adaptation_time`` from the previous pulse. This factor is
    against abrupt starts of the pulse train after silences, and is 1.0 if
    you do want abrupt starts after silences. (default: 1.0)

adaptation_time : float
    The minimal period that will be considered a silence. (default: 0.05)

interpolation_depth : int
    The extent of the :math:`sinc` function to the left and to the right of
    the peak. (default: 2000)

See also
--------
:praat:`PointProcess: To Sound (pulse train)...`
)";

constexpr auto TO_SOUND_PHONATION_DOCSTRING =
	R"(Convert into a glottal waveform `Sound` object.

PointProcess object is converted to a sound object containing glottal
waveform at every point in the point process. Its shape depends on the
settings ``power1`` and ``power2`` according to the formula.

.. math:: U\(x\) = x^{power1} - x^{power2}

where :math:`x` is a normalized time that runs from 0 to 1 and :math:`U(x)`
is the normalized glottal flow in arbitrary units (the real unit is
:math:`m^3/s`).

Parameters
----------
sampling_frequency : float
    The sampling frequency of the resulting `Sound` object.
    (default: 44100.0)

adaptation_factor : float,
    The factor by which a pulse height will be multiplied if the pulse time
    is not within Maximum period from the previous pulse, and by which a
    pulse height will again be multiplied if the previous pulse time is not
    within ``maximum_period`` from the pre-previous pulse. This factor is
    against abrupt starts of the pulse train after silences, and is 1.0 if
    you do want abrupt starts after silences. (default: 1.0)

maximum_period : float
    The minimal period that will be considered a silence in seconds.
    (default: 0.05)

open_phase: float
    Fraction of a period when the glottis is open. (default: 0.7)

collision_phase : float
    Decay factor to ease the abrupt collision at closure. (default: 0.03)

power1 : float
    First glottal flow shape coefficient. (default: 3.0)

power2 : float
    Second glottal flow shape coefficient. (default: 4.0)

See also
--------
:praat:`PointProcess: To Sound (phonation)...`
)";

constexpr auto TO_SOUND_HUM_DOCSTRING =
	R"(Convert into a Sound with hum sound

PointProcess object is converted to a sound object with hum sound. A Sound
is created from pulses, followed by filtered by a sequence of second-order
filters that represent five formants.

See also
--------
:praat:`PointProcess: To Sound (hum)...`
)";

} // namespace parselmouth

#endif // INC_PARSELMOUTH_POINTPROCESS_DOCSTRINGS_H
