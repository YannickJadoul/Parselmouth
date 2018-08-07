import pytest

import parselmouth
import numpy as np

SAMPLING_FREQUENCIES = (100, 16000, 44100)


def test_from_numpy_array_mono():
	for sampling_frequency in SAMPLING_FREQUENCIES:
		sine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
		sound = parselmouth.Sound(sine_values, sampling_frequency=sampling_frequency)
		assert np.all(sound.values == sine_values[np.newaxis,:])
		assert sound.n_samples == len(sine_values)
		assert sound.n_channels == 1
		assert sound.sampling_frequency == sampling_frequency
		assert sound.duration == 1


def test_from_numpy_array_stereo():
	for sampling_frequency in SAMPLING_FREQUENCIES:
		sine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
		cosine_values = np.sin(2 * np.pi * np.arange(sampling_frequency) / sampling_frequency)
		sound = parselmouth.Sound(np.vstack((sine_values, cosine_values)), sampling_frequency=sampling_frequency)
		assert np.all(sound.values == np.vstack((sine_values, cosine_values)))
		assert sound.n_samples == len(sine_values)
		assert sound.n_channels == 2
		assert sound.sampling_frequency == sampling_frequency
		assert sound.duration == 1


def test_from_scalar():
	for sampling_frequency in SAMPLING_FREQUENCIES:
		with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
			parselmouth.Sound(42, sampling_frequency=sampling_frequency)

		with pytest.raises(ValueError, match="Cannot create Sound from a single 0-dimensional number"):
			parselmouth.Sound(3.14159, sampling_frequency=sampling_frequency)
