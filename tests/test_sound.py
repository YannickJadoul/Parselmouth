# Copyright (C) 2018-2022  Yannick Jadoul
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
import numpy as np


@pytest.fixture(params=[100, 16000, 44100])
def sampling_frequency(request):
	return request.param


def test_from_numpy_array_mono(sampling_frequency):
	sine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
	sound = parselmouth.Sound(sine_values, sampling_frequency=sampling_frequency)
	assert np.all(sound.values == sine_values[np.newaxis,:])
	assert sound.n_samples == len(sine_values)
	assert sound.n_channels == 1
	assert sound.sampling_frequency == sampling_frequency
	assert sound.duration == 1

	sound = parselmouth.Sound(sine_values[1::3], sampling_frequency=sampling_frequency)
	assert np.all(sound.values == sine_values[np.newaxis,1::3])


def test_from_numpy_array_stereo(sampling_frequency):
	sine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
	cosine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
	sound = parselmouth.Sound(np.vstack((sine_values, cosine_values)), sampling_frequency=sampling_frequency)
	assert np.all(sound.values == [sine_values, cosine_values])
	assert sound.n_samples == len(sine_values)
	assert sound.n_channels == 2
	assert sound.sampling_frequency == sampling_frequency
	assert sound.duration == 1

	sound = parselmouth.Sound(np.vstack((sine_values, cosine_values))[::-1,1::3], sampling_frequency=sampling_frequency)
	assert np.all(sound.values == [cosine_values[1::3], sine_values[1::3]])


def test_from_scalar(sampling_frequency):
	with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
		parselmouth.Sound(42, sampling_frequency=sampling_frequency)

	with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
		parselmouth.Sound(3.14159, sampling_frequency=sampling_frequency)
