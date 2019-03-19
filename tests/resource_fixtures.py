import pytest
import pytest_lazyfixture

import parselmouth


def combined_fixture(*args, **kwargs):
	return pytest.fixture(params=map(pytest_lazyfixture.lazy_fixture, args), ids=args, **kwargs)


@pytest.fixture
def sound_path(resources):
	yield resources["the_north_wind_and_the_sun.wav"]


@pytest.fixture
def sound(sound_path):
	yield parselmouth.read(sound_path)


@pytest.fixture
def intensity(sound):
	yield sound.to_intensity()


@pytest.fixture
def pitch(sound):
	yield sound.to_pitch()


@pytest.fixture
def spectrogram(sound):
	yield sound.to_spectrogram()


@combined_fixture('intensity', 'pitch', 'spectrogram', 'sound')
def sampled(request):
	yield request.param


@combined_fixture('sampled')
def thing(request):
	yield request.param


@pytest.fixture
def text_grid_path(resources):
	yield resources["the_north_wind_and_the_sun.TextGrid"]


@pytest.fixture
def text_grid(text_grid_path):
	yield parselmouth.read(text_grid_path)
