# Copyright (C) 2019-2023  Yannick Jadoul
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
import textwrap

import pytest

import parselmouth
import re
import sys
import warnings


def test_warning(sound, tmp_path):
	message = "Watch out!"
	with pytest.warns(parselmouth.PraatWarning, match=f"^{message}$"):
		parselmouth.praat._warn(message)

	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with pytest.warns(parselmouth.PraatWarning, match="No non-empty intervals were found"):
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)

	with pytest.warns(parselmouth.PraatWarning, match=r"Writing samples to audio file “.*clipped.wav”: [0-9]+ out of [0-9]+ samples have been clipped"):
		sound.scale(2)
		sound.save(str(tmp_path / "clipped.wav"), "WAV")


def test_warning_as_error(sound):
	warnings.simplefilter('error', parselmouth.PraatWarning)

	message = "Watch out! Seriously!"
	with pytest.raises(parselmouth.PraatWarning, match=f"^{message}$"):
		parselmouth.praat._warn(message)

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


def test_error():
	message = "This is not a drill! OK, maybe it actually is."
	with pytest.raises(parselmouth.PraatError, match=f"^{message}$"):
		parselmouth.praat._throw_error(message)


def test_crash():
	expected_message = textwrap.dedent("""\
		Parselmouth intercepted a crash in Praat:

		BOOM!

		To ensure correctness of Praat's calculations, it is advisable to NOT ignore this error
		and to RESTART Python before using more of Praat's functionality through Parselmouth.""")
	if sys.platform == 'win32':
		expected_message = expected_message.replace('\n', '\r\n')
	with pytest.raises(parselmouth.PraatCrash, match=f"^{re.escape(expected_message)}$"):
		parselmouth.praat._crash("BOOM!")

	message = "Errors still work, though"
	with pytest.raises(parselmouth.PraatError, match=f"^{message}$"):
		parselmouth.praat._throw_error(message)
