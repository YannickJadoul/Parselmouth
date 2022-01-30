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

import sys


def test_create(text_grid_path):
	assert parselmouth.TextGrid(0.0, 1.0) == parselmouth.TextGrid(0.0, 1.0, [], [])
	assert parselmouth.TextGrid(0.0, 1.0, ["a", "b", "c", "d", "e"], ["b", "d", "e"]) == parselmouth.TextGrid(0.0, 1.0, "a b c d e", "b d e")
	assert parselmouth.TextGrid(0.0, 1.0, "a b c d e", "b d e") == parselmouth.praat.call("Create TextGrid", 0.0, 1.0, "a b c d e", "b d e")
	assert isinstance(parselmouth.read(text_grid_path), parselmouth.TextGrid)
	with pytest.raises(parselmouth.PraatError, match="The end time should be greater than the start time"):
		parselmouth.TextGrid(1.0, 0.0)
	with pytest.raises(parselmouth.PraatError, match="Point tier name 'c' is not in list of all tier names"):
		parselmouth.TextGrid(0.0, 1.0, ["a", "b"], ["a", "c", "d"])


@pytest.mark.parametrize('include_empty_intervals', [True, False])
def test_tgt(text_grid_path, include_empty_intervals):
	tgt = pytest.importorskip('tgt')

	text_grid = parselmouth.read(text_grid_path)  # TODO Replace with TextGrid constructor taking filename?
	assert [t.annotations for t in text_grid.to_tgt(include_empty_intervals=include_empty_intervals).tiers] == [t.annotations for t in tgt.read_textgrid(text_grid_path, 'utf-8', include_empty_intervals=include_empty_intervals)]
	assert parselmouth.TextGrid.from_tgt(text_grid.to_tgt()) == text_grid


def test_tgt_missing(text_grid_path, monkeypatch):
	monkeypatch.setitem(sys.modules, 'tgt', None)

	with pytest.raises(RuntimeError, match="Could not import 'tgt'"):
		parselmouth.read(text_grid_path).to_tgt()
	with pytest.raises(TypeError, match="incompatible function arguments"):
		parselmouth.TextGrid.from_tgt(None)


def test_tgt_exceptions(text_grid_path, monkeypatch):
	tgt = pytest.importorskip('tgt')

	class MockTextGrid(object):  # Python 2 compatibility
		def __init__(self, *args, **kwargs):
			pass
	monkeypatch.setattr(tgt, "TextGrid", MockTextGrid)

	with pytest.raises(AttributeError, match=r"'MockTextGrid' object has no attribute '.*'|MockTextGrid instance has no attribute '.*'"):  # Python 2 compatibility
		parselmouth.read(text_grid_path).to_tgt()
	with pytest.raises(TypeError, match="'MockTextGrid' object is not iterable"):
		parselmouth.TextGrid.from_tgt(MockTextGrid())
