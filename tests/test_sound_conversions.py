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


def test_sound_to_pitch(sound):
	fragment = sound.extract_part(to_time=0.4)

	assert fragment.to_pitch() == fragment.to_pitch(parselmouth.Sound.ToPitchMethod.AC, very_accurate=False)
	assert fragment.to_pitch(0.001, 50, 300) == fragment.to_pitch(parselmouth.Sound.ToPitchMethod.AC, time_step=0.001, pitch_ceiling=300, very_accurate=False, pitch_floor=50)

	assert fragment.to_pitch(pitch_floor=50.0, method=parselmouth.Sound.ToPitchMethod.AC) == fragment.to_pitch_ac(pitch_floor=50)
	assert fragment.to_pitch("CC", pitch_ceiling=300) == fragment.to_pitch_cc(pitch_ceiling=300.0)
