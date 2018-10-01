import pytest
import pytest_lazyfixture

import parselmouth


def combined_fixture(*args, **kwargs):
	return pytest.fixture(params=map(lambda x: pytest.param(pytest_lazyfixture.lazy_fixture(x), id=x), args), **kwargs)


@pytest.fixture
def sound(resources):
	yield parselmouth.Data.read(resources["the_north_wind_and_the_sun.wav"])

@pytest.fixture
def intensity(sound):
	yield sound.to_intensity()

@pytest.fixture
def spectrogram(sound):
	yield sound.to_spectrogram()

@combined_fixture('sound', 'intensity', 'spectrogram')
def sampled(request):
	yield request.param
