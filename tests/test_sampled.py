# Copyright (C) 2018-2023  Yannick Jadoul
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

import numpy as np


def test_xs(sampled):
	# Note: arm64's fmadd does not have intermediate rounding, so we don't always get the exact same values
	assert sampled.xs() == pytest.approx(sampled.x1 + sampled.dx * np.arange(sampled.nx), abs=0, rel=1e-15)
	assert sampled.x_grid() == pytest.approx(sampled.x1 + sampled.dx * (np.arange(sampled.nx + 1) - 0.5), abs=0, rel=1e-15)
	assert sampled.x_bins() == pytest.approx(np.vstack((sampled.x_grid()[:-1], sampled.x_grid()[1:])).T, abs=0, rel=1e-15)


def test_len(sampled):
	assert len(sampled) == sampled.nx
