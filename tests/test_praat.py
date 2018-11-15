import pytest

import numpy as np
import parselmouth
import textwrap


def test_call_with_extra_objects(sound):
	sound.name = "the sound"
	assert sound.name == "the_sound" # TODO Move (check) somewhere else (as well), e.g. test_thing.py?
	new = parselmouth.praat.call("Create Sound from formula", "new", sound.n_channels, sound.start_time, sound.end_time, sound.sampling_frequency, "0")
	parselmouth.praat.call(new, "Formula", "self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert np.all(sound.values == new.values)
	# assert sound == new fails because x1 floating point values are not exactly equal, because Praat calculates both in a slightly different way

	with pytest.raises(parselmouth.PraatError, match=r"No such object \(note: variables start with lower case\)"):
		parselmouth.praat.call(new, "Formula", "self [col] + Sound_the_sound [col]")


def test_run(resources):
	script = textwrap.dedent("""\
	Read from file: "{}"
	To Intensity: 100.0, 0.0, "yes"
	selectObject: 1
	selectObject: "Intensity the_north_wind_and_the_sun"
	""".format(resources["the_north_wind_and_the_sun.wav"]))

	assert parselmouth.praat.run(script)[0] == parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"]).to_intensity()

	with pytest.raises(parselmouth.PraatError, match="Found 3 arguments but expected only 0."):
		parselmouth.praat.run(script, 42, "some_argument", True)


def test_run_with_parameters(resources):
	script = textwrap.dedent("""
	form Test
		positive minPitch 100.0
		real timeStep 0.0
		boolean subtractMean "yes"
	endform
	
	Read from file: "{}"
	To Intensity: minPitch, timeStep, subtractMean
	selectObject: 1
	selectObject: "Intensity the_north_wind_and_the_sun"
	""".format(resources["the_north_wind_and_the_sun.wav"]))

	min_pitch = 75.0
	time_step = 0.05
	subtract_mean = False

	assert parselmouth.praat.run(script, min_pitch, time_step, subtract_mean)[0] == parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"]).to_intensity(min_pitch, time_step, subtract_mean)

	with pytest.raises(parselmouth.PraatError, match="Found 0 arguments but expected more."):
		parselmouth.praat.run(script)


@pytest.mark.skip
def test_run_with_extra_objects(sound):
	new = parselmouth.Sound(np.zeros((sound.n_channels, sound.n_samples)), sampling_frequency=sound.sampling_frequency, start_time=sound.start_time)
	sound.name = "the sound"
	parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert np.all(sound.values == new.values)

	with pytest.raises(parselmouth.PraatError, match=r"No such object \(note: variables start with lower case\)"):
		parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]")


def test_run_with_capture_output():
	assert parselmouth.praat.run("writeInfo: 42", capture_output=True) == ([], "42")
	assert parselmouth.praat.run("appendInfo: 42", capture_output=True) == ([], "42")
	assert parselmouth.praat.run("writeInfoLine: 42", capture_output=True) == ([], "42\n")
	assert parselmouth.praat.run("writeInfoLine: \"The answer\", \" - \", 42\nappendInfo: \"The question - ?\"", capture_output=True) == ([], "The answer - 42\nThe question - ?")
	assert parselmouth.praat.run("writeInfoLine: \"The answer\", \" - \", 42\nwriteInfoLine: \"The question - ?\"", capture_output=True) == ([], "The question - ?\n")
	assert parselmouth.praat.run("writeInfo: tab$, newline$", capture_output=True) == ([], "\t\n")


def test_run_with_return_variables():
	script = textwrap.dedent("""\
	a = 42
	b$ = "abc"
	c# = {1, 1, 2, 3, 5, 8, 13, 21, 34}
	d## = outer##({1, (1 + sqrt(5)) / 2}, c#)
	""")

	objects, variables = parselmouth.praat.run(script, return_variables=True)
	assert objects == []
	assert 'a' in variables and 'a$' not in variables and isinstance(variables['a'], float) and variables['a'] == 42
	assert 'b$' in variables and 'b' not in variables and variables['b$'] == "abc"
	assert 'c#' in variables and 'c' not in variables and isinstance(variables['c#'], np.ndarray) and variables['c#'].dtype == np.dtype(float) and variables['c#'].shape == (9,) and np.all(variables['c#'] == [1, 1, 2, 3, 5, 8, 13, 21, 34])
	assert 'd##' in variables and 'd#' not in variables and isinstance(variables['d##'], np.ndarray) and variables['d##'].dtype == np.dtype(float) and variables['d##'].shape == (2, 9) and np.all(variables['d##'] == np.outer([1, (1 + np.sqrt(5)) / 2], [1, 1, 2, 3, 5, 8, 13, 21, 34]))
	assert set(variables.keys()) == {'a', 'b$', 'c#', 'd##', 'newline$', 'tab$', 'shellDirectory$', 'defaultDirectory$', 'preferencesDirectory$', 'homeDirectory$', 'temporaryDirectory$', 'macintosh', 'windows', 'unix', 'left', 'right', 'mono', 'stereo', 'all', 'average', 'praatVersion$', 'praatVersion'}


def test_run_with_capture_output_and_return_variables():
	script = textwrap.dedent("""\
	a = 42
	b$ = "abc"
	writeInfoLine: a
	appendInfoLine: b$
	""")

	objects, output, variables = parselmouth.praat.run(script, capture_output=True, return_variables=True)
	assert objects == []
	assert output == "42\nabc\n"
	assert 'a' in variables and 'b$' in variables
