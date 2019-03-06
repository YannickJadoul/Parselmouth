import pytest

import parselmouth


def test_create(resources):
	assert parselmouth.TextGrid(0.0, 1.0) == parselmouth.TextGrid(0.0, 1.0, [], [])
	assert parselmouth.TextGrid(0.0, 1.0, ["a", "b", "c", "d", "e"], ["b", "d", "e"]) == parselmouth.TextGrid(0.0, 1.0, "a b c d e", "b d e")
	assert parselmouth.TextGrid(0.0, 1.0, "a b c d e", "b d e") == parselmouth.praat.call("Create TextGrid", 0.0, 1.0, "a b c d e", "b d e")
	assert isinstance(parselmouth.Data.read(resources["the_north_wind_and_the_sun.TextGrid"]), parselmouth.TextGrid)


def test_tgt(resources):
	tgt = pytest.importorskip('tgt')
	text_grid_filename = resources["the_north_wind_and_the_sun.TextGrid"]
	text_grid = parselmouth.Data.read(text_grid_filename)  # TODO Replace with TextGrid constructor taking filename?
	# assert text_grid.to_tgt() == tgt.read_textgrid(text_grid_filename, 'utf-8')
	assert parselmouth.TextGrid.from_tgt(text_grid.to_tgt()) == text_grid


# TODO Test error message when 'tgt' is not installed?
