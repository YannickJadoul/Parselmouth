import pytest

import parselmouth


def test_run_script(resources):
	script = """
	Read from file: "{}"
	To Intensity: 100.0, 0.0, "yes"
	selectObject: 1
	selectObject: "Intensity the_north_wind_and_the_sun"
	""".format(resources["the_north_wind_and_the_sun.wav"])

	assert parselmouth.praat.run(script)[0] == parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"]).to_intensity()

	with pytest.raises(parselmouth.PraatError, match="Found 3 arguments but expected only 0."):
		parselmouth.praat.run(script, 42, "some_argument", True)


def test_run_script_with_parameters(resources):
	script = """
	form Test
		positive minPitch 100.0
		real timeStep 0.0
		boolean subtractMean "yes"
	endform
	
	Read from file: "{}"
	To Intensity: minPitch, timeStep, subtractMean
	selectObject: 1
	selectObject: "Intensity the_north_wind_and_the_sun"
	""".format(resources["the_north_wind_and_the_sun.wav"])

	min_pitch = 75.0
	time_step = 0.05
	subtract_mean = False

	assert parselmouth.praat.run(script, min_pitch, time_step, subtract_mean)[0] == parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"]).to_intensity(min_pitch, time_step, subtract_mean)

	with pytest.raises(parselmouth.PraatError, match="Found 0 arguments but expected more."):
		parselmouth.praat.run(script)


def test_call_no_objects(sound):
	assert parselmouth.praat.call(sound, "Get number of samples") == sound.n_samples
	with pytest.raises(parselmouth.PraatError, match="Command \"Get number of samples\" not available for given objects."):
		assert parselmouth.praat.call("Get number of samples")
