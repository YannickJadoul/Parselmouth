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
#ifndef INC_PARSELMOUTH_PITCH_DOCSTRINGS_H
#define INC_PARSELMOUTH_PITCH_DOCSTRINGS_H

namespace parselmouth {

constexpr auto TO_POINT_PROCESS_DOCSTRING =
	R"(Create PointProcess from Pitch object.

Returns a new PointProcess instance which is generated from the specified
Pitch object. The acoustic periodicity contour stored in the Pitch object
is used as the frequency of an underlying point process (such as the
sequence of glottal closures in vocal-fold vibration).

See Also
--------
:praat:`Pitch: To PointProcess`
)";

constexpr auto TO_POINT_PROCESS_SOUND_DOCSTRING =
	R"(Create PointProcess.

Returns a new PointProcess instance by interpreting the acoustic
periodicity contour in the `Pitch` object as the frequency of an
underlying point process (such as the sequence of glottal closures in
vocal-fold vibration).

The unvoiced intervals in the ``pitch`` object is transferred to the point
process object, and the voiced intervals are further divided into each
phonation cycles.

Parameters
----------
sound : parselmouth.Sound or None, default=None
    Sound object containing the target sound waveform. If omitted,
    `PointProcess` is created only from the pitch contour. Analyzing the
    samples in `sound` object improves the accuracy of the resulting point
    process.

method : {"cc", "peaks"}, default="cc"
    Specify the Sound-assited generation method:

    "cc"    - Cross-correlation method. The fundamental periods of voice
              are identified by cross-correlating the sound samples.

    "peaks" - Peak-picking method. The fundamental periods of voice are
              identified by peak-picking the sound samples. Typically, less
              accurate than the cross-correlation method.

include_maxima : bool, default=True
    True to include the absolute maximum (for `method="peaks" only)

include_minima : bool, default=False
    True to include the absolute minimum (for `method="peaks" only)

See Also
--------
:praat:`Pitch: To PointProcess`
:praat:`Sound & Pitch: To PointProcess (cc)`
:praat:`Sound & Pitch: To PointProcess (peaks)...`
)";

constexpr auto TO_POINT_PROCESS_CC_DOCSTRING =
R"(Create PointProcess from Sound and Pitch objects using crosscorrelation.

Returns a new PointProcess instance, generated from the specified Sound
and Pitch instances using the cross-correlation method. The resulting
instance contains voiced and unvoiced intervals according to ``pitch``
object, and the voiced intervals are further divided into fundamental
periods of voice, identified by cross-correlating the sound samples.

Parameters
----------
sound : parselmouth.Sound
    Sound object containing the target sound waveform

See Also
--------
:praat:`Sound & Pitch: To PointProcess (cc)`
)";

constexpr auto TO_POINT_PROCESS_PEAKS_DOCSTRING =
R"(Create PointProcess from Sound and Pitch objects using peak-picking.

Returns a new PointProcess instance, generated from the specified `Sound`
and `Pitch` instances using the peak-picking method. The resulting
instance contains voiced and unvoiced intervals according to ``pitch``
object, and the voiced intervals are further divided into fundamental
periods of voice, identified by peak-picking the sound samples.

The periods that are found in this way are much more variable than those
found by `Pitch.to_point_process_cc()` and therefore less useful for
analysis and subsequent overlap-add synthesis.

Parameters
----------
sound : parselmouth.Sound
    Sound object containing the target sound waveform

See Also
--------
:praat:`Sound & Pitch: To PointProcess (peaks)...`
)";

} // namespace parselmouth

#endif // INC_PARSELMOUTH_PITCH_DOCSTRINGS_H
