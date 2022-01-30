# Copyright (C) 2019-2022  Yannick Jadoul
#
# This file is part of Parselmouth.
#
# Parselmouth is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Parselmouth is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>

import pytest

import parselmouth
import warnings


def test_warning(sound, tmp_path):
	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with pytest.warns(parselmouth.PraatWarning, match="No non-empty intervals were found"):
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)

	with pytest.warns(parselmouth.PraatWarning, match=r"Writing samples to audio file: [0-9]+ out of [0-9]+ samples have been clipped"):
		sound.scale(2)
		sound.save(str(tmp_path / "clipped.wav"), "WAV")


def test_warning_as_error(sound):
	warnings.simplefilter('error', parselmouth.PraatWarning)

	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with pytest.raises(parselmouth.PraatWarning, match="No non-empty intervals were found"):
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)


def test_warnings_default(sound):
	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with warnings.catch_warnings(record=True) as recorded_warnings:
		for i in range(3):
			parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)

		assert len(recorded_warnings) == 3
		for w in recorded_warnings:
			assert str(w.message) == "No non-empty intervals were found."
			assert isinstance(w.message, parselmouth.PraatWarning)
