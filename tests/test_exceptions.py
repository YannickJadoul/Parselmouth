import pytest

import parselmouth
import warnings


def test_warning(sound):
	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with pytest.warns(parselmouth.PraatWarning, match="No non-empty intervals were found."):
		parselmouth.praat.call([text_grid, sound], "Extract non-empty intervals", 1, True)


def test_warning_as_error(sound):
	warnings.simplefilter('error', parselmouth.PraatWarning)

	text_grid = parselmouth.praat.call(sound, "To TextGrid", "tier", "")

	with pytest.raises(parselmouth.PraatWarning, match="No non-empty intervals were found."):
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
