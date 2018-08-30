import pytest

import parselmouth


def test_sound_to_pitch(sound):
	fragment = sound.extract_part(to_time=0.4)

	assert fragment.to_pitch() == fragment.to_pitch(parselmouth.Sound.ToPitchMethod.AC, very_accurate=False)
	assert fragment.to_pitch(0.001, 50, 300) == fragment.to_pitch(parselmouth.Sound.ToPitchMethod.AC, time_step=0.001, pitch_ceiling=300, very_accurate=False, pitch_floor=50)

	assert fragment.to_pitch(pitch_floor=50.0, method=parselmouth.Sound.ToPitchMethod.AC) == fragment.to_pitch_ac(pitch_floor=50)
	assert fragment.to_pitch("CC", pitch_ceiling=300) == fragment.to_pitch_cc(pitch_ceiling=300.0)
