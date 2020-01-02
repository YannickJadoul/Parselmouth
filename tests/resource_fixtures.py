# Copyright (C) 2018-2020  Yannick Jadoul
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
