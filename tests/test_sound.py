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

	with pytest.warns(RuntimeWarning, match=r"Number of channels \([0-9]+\) is greater than number of samples \([0-9]+\)"):
		parselmouth.Sound(np.vstack((sine_values, cosine_values)).T, sampling_frequency=sampling_frequency)


def test_from_scalar(sampling_frequency):
	with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
		parselmouth.Sound(42, sampling_frequency=sampling_frequency)

	with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
		parselmouth.Sound(3.14159, sampling_frequency=sampling_frequency)

@pytest.mark.filterwarnings('ignore:Number of channels .* is greater than number of samples')
def test_channel_type():
	n_channels = 10
	sound = parselmouth.Sound(np.arange(n_channels)[:,None])
	assert sound.n_channels == n_channels
	for i in range(n_channels):
		assert np.array_equal(sound.extract_channel(parselmouth.Sound.Channel(i + 1)).values, [[i]])
		assert np.array_equal(sound.extract_channel(i + 1).values, [[i]])
	with pytest.raises(TypeError, match=r"extract_channel\(\): incompatible function arguments"):
		sound.extract_channel(-1)
	assert np.isnan(sound.get_nearest_zero_crossing(0, channel=1))
	with pytest.raises(ValueError, match=r"Channel number (.*) is larger than number of available channels (.*)\."):
		assert sound.get_nearest_zero_crossing(0, channel=n_channels + 1)
	assert np.array_equal(sound.extract_channel('LEFT').values, [[0]])
	assert np.array_equal(sound.extract_channel('right').values, [[1]])
	with pytest.raises(TypeError, match=r"extract_channel\(\): incompatible function arguments"):
		sound.extract_channel('MIDDLE')

	assert parselmouth.Sound.Channel(42).value == 42
	with pytest.raises(ValueError, match=r"Channel number should be positive or zero\."):
		parselmouth.Sound.Channel(-1)
	with pytest.raises(ValueError, match=r"Channel string can only be 'left' or 'right'\."):
		parselmouth.Sound.Channel('MIDDLE, I said')
