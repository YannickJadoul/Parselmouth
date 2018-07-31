import pytest

import numpy as np


def test_xs(sampled):
	assert np.all(sampled.xs() == sampled.x1 + sampled.dx * np.arange(sampled.nx))
	assert np.all(sampled.x_grid() == sampled.x1 + sampled.dx * (np.arange(sampled.nx + 1) - 0.5))
	assert np.all(sampled.x_bins() == np.vstack((sampled.x_grid()[:-1], sampled.x_grid()[1:])).T)


def test_len(sampled):
	assert len(sampled) == sampled.nx
