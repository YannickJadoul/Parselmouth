# Copyright (C) 2020-2022  Yannick Jadoul
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

from __future__ import division  # Python 2 compatibility

import pytest

import parselmouth


def test_raw_file(tmp_path):
	r, c = 37, 23
	matrix = parselmouth.praat.call("Create Matrix", "matrix", 0, 1, c, 1 / c, 0.5 / c, 0, 1, r, 1 / r, 0.5 / r, 'randomUniform(0, 42)')
	file_path = tmp_path / "matrix.txt"
	parselmouth.praat.call(matrix, "Save as headerless spreadsheet file", str(file_path))
	reread_matrix = parselmouth.praat.call("Read Matrix from raw text file", str(file_path))
	assert reread_matrix.n_rows == r
	assert reread_matrix.n_columns == c
	assert reread_matrix.xmin == 0.5
	assert reread_matrix.xmax == c + 0.5
	assert reread_matrix.dx == 1
	assert reread_matrix.x1 == 1
	assert reread_matrix.ymin == 0.5
	assert reread_matrix.ymax == r + 0.5
	assert reread_matrix.dy == 1
	assert reread_matrix.y1 == 1
