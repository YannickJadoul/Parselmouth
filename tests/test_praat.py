import pytest

import itertools
import numpy as np
import parselmouth
import textwrap


def test_call_return_many(sound):
	stereo_sound = sound.convert_to_stereo()
	channels = parselmouth.praat.call(stereo_sound, "Extract all channels")
	assert len(channels) == 2
	assert np.all(channels[0].values == sound.values)
	assert np.all(channels[1].values == sound.values)


def test_call_with_extra_objects(sound):
	new = parselmouth.Sound(np.zeros((sound.n_channels, sound.n_samples)), sampling_frequency=sound.sampling_frequency, start_time=sound.start_time)
	sound.name = "the sound"
	parselmouth.praat.call(new, "Formula", "self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert sound == new

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


def test_run_with_extra_objects(sound):
	new = parselmouth.Sound(np.zeros((sound.n_channels, sound.n_samples)), sampling_frequency=sound.sampling_frequency, start_time=sound.start_time)
	sound.name = "the sound"
	parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert sound == new

	with pytest.raises(parselmouth.PraatError, match=r"No such object \(note: variables start with lower case\)"):
		parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]")


def test_run_sys_stdout(capsys):
	parselmouth.praat.run("writeInfo: 42")
	assert capsys.readouterr().out == "42"
	parselmouth.praat.run("appendInfo: 42")
	assert capsys.readouterr().out == "42"  # TODO Not correct, "4242"
	parselmouth.praat.run("writeInfoLine: 42")
	assert capsys.readouterr().out == "42\n"
	parselmouth.praat.run("writeInfoLine: \"The answer\", \" - \", 42\nappendInfo: \"The question - ?\"")
	assert capsys.readouterr().out == "The answer - 42\nThe question - ?"  # TODO Not correct, "The answer - 42\nThe answer - 42\nThe question - ?"
	parselmouth.praat.run("writeInfoLine: \"The answer\", \" - \", 42\nwriteInfoLine: \"The question - ?\"")
	assert capsys.readouterr().out == "The answer - 42\nThe question - ?\n"
	parselmouth.praat.run("writeInfo: tab$, newline$")
	assert capsys.readouterr().out == "\t\n"


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


def test_praat_callback_prefixes():
	separators = {'nullptr', '0'}
	values = {'REAL', 'INTEGER', 'BOOLEAN', 'COMPLEX', 'STRING', 'NUMVEC', 'NUMMAT'}
	objects = {'NEW', 'NEW1', 'NEW2', 'NEWMANY', 'NEWTIMES2', 'READ1', 'READMANY'}
	info = {'HINT', 'INFO', 'LIST'}
	nothing = {'HELP', 'MODIFY', 'PRAAT', 'PREFS', 'SAVE', 'GRAPHICS'}
	exception = {'PLAY', 'RECORD1', 'WINDOW', 'MOVIE'}
	weird = {'BUG', 'DANGEROUS'}

	prefixes = set(action[2].split('_')[0] for action in itertools.chain(parselmouth.praat._get_actions(), parselmouth.praat._get_menu_commands()))
	assert prefixes == separators | values | objects | info | nothing | exception | weird
