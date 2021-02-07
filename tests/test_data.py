# Copyright (C) 2018-2021  Yannick Jadoul
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


def test_read():
	assert parselmouth.Data.read == parselmouth.read


def test_read_nonexistent():
	with pytest.raises(parselmouth.PraatError, match=r'Cannot open file \u201c.*nonexistent.wav\u201d\.'):
		parselmouth.read("nonexistent.wav")


# TODO Other encodings
