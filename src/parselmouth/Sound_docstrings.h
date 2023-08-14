/*
* Copyright (C) 2023  Yannick Jadoul
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
#ifndef INC_PARSELMOUTH_SOUND_DOCSTRINGS_H
#define INC_PARSELMOUTH_SOUND_DOCSTRINGS_H

namespace parselmouth {

auto constexpr SOUND_DOCSTRING =
R"(A fragment of audio, represented by one or multiple channels of floating
point values between -1 and 1, sampled at a fixed sampling frequency.

Corresponds to a :praat:`Sound` object.

See Also
--------
:praat:`Sound`, :praat:`Sound files`,
:praat:`Sound files 1. General structure`
)";

auto constexpr SOUND_INIT_DOCSTRING =
R"(Create a new `Sound` object.

The new object can be created:
- as a copy of an existing `Sound` object,
- from an array of samples and a sampling frequency, or
- by reading an audio file from disk.

Parameters
----------
other : Sound
    The `Sound` object to copy.
samples : array_like[float]
    The samples of the new `Sound` object.
sampling_frequency : float, optional
    The sampling frequency of the new `Sound` object (default: 44100).
start_time : float, optional
    The start time (`~Function.xmin`) of the new `Sound` object
    (default: 0).
file_name : str, optional
    The file name of the audio file to load.
file_path : str
    The file path of an audio file to load from disk.

See Also
--------
:praat:`Sound files 2. File types`,
:praat:`Sound files 3. Files that Praat can read`
)";

auto constexpr SOUND_SAVE_DOCSTRING =
R"(Save a `Sound` object to an audio file on disk.

Parameters
----------
file_path : str
    The file path of the audio file to save to disk.
file_format : `SoundFileFormat`
    The audio file format to write to. This can either be a
    `SoundFileFormat` value (e.g., `SoundFileFormat.WAV`), or the string
    representation of the value (e.g., ``"WAV"``).

See Also
--------
:praat:`Sound files 2. File types`,
:praat:`Sound files 4. Files that Praat can write`
)";

} // namespace parselmouth

#endif // INC_PARSELMOUTH_SOUND_DOCSTRINGS_H
