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

import itertools
import numpy as np
import os
import re
import textwrap


def test_call_with_extra_objects(sound):
	new = parselmouth.Sound(np.zeros((sound.n_channels, sound.n_samples)), sampling_frequency=sound.sampling_frequency, start_time=sound.start_time)
	sound.name = "the sound"
	parselmouth.praat.call(new, "Formula", "self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert sound == new

	with pytest.raises(parselmouth.PraatError, match=r"No such object \(note: variables start with nonupper case\)"):
		parselmouth.praat.call(new, "Formula", "self [col] + Sound_the_sound [col]")


def test_call_parameters(sound):
	assert parselmouth.praat.call(sound, "Add", 0.1) is None
	assert parselmouth.praat.call(sound, "Add", -1) is None
	assert parselmouth.praat.call(sound, "Override sampling frequency", 44100) is None
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" must be greater than 0"):
		assert parselmouth.praat.call(sound, "Override sampling frequency", -10.0) is None

	assert parselmouth.praat.call(sound, "Get time from sample number", 1) == sound.get_time_from_index(1)
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" should be a whole number"):
		assert parselmouth.praat.call(sound, "Get time from sample number", 0.5) != sound.get_time_from_index(1)
	assert parselmouth.praat.call(sound, "Set value at sample number", 1, 0.0) is None
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" should be a positive whole number"):
		assert parselmouth.praat.call(sound, "Set value at sample number", 0, -1, 0.0) is None

	assert parselmouth.praat.call(sound, "To Spectrum", True) == parselmouth.praat.call(sound, "To Spectrum", 1)
	assert parselmouth.praat.call(sound, "To Spectrum", False) == parselmouth.praat.call(sound, "To Spectrum", "no")

	assert parselmouth.praat.call(sound, "To TextGrid", "points intervals", "points").class_name == "TextGrid"
	assert parselmouth.praat.call("Create Sound from formula", "someSound", 1, 0, 1, 44100, "1/2").name == "someSound"

	many_channels = parselmouth.Sound(np.zeros((10, 1600)), 16000)
	assert parselmouth.praat.call(many_channels, "Extract channels", np.array([2, 3, 5, 7])).n_channels == 4
	assert parselmouth.praat.call(many_channels, "Extract channels", [2, 3, 5, 7]).n_channels == 4
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" should be a numeric vector, not a number"):
		assert parselmouth.praat.call(many_channels, "Extract channels", 4) == 1
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" should be a numeric vector, not a numeric matrix"):
		assert parselmouth.praat.call(many_channels, "Extract channels", np.array([[2, 3, 5, 7]])) == 4

	# If a Praat command with a NUMMAT argument gets added, a test should be added

	table = parselmouth.praat.call("Create Table with column names", "test", 10, ["a", "b", "c"])
	assert isinstance(table, parselmouth.Data)
	assert table.class_name == "Table"
	assert parselmouth.praat.call(table, "Get number of rows") == 10
	assert parselmouth.praat.call(table, "Get number of columns") == 3
	assert [parselmouth.praat.call(table, "Get column label", i + 1) for i in range(3)] == ["a", "b", "c"]
	table = parselmouth.praat.call("Create Table with column names", "test", 10, np.array(["a", "b", "c"]))
	assert [parselmouth.praat.call(table, "Get column label", i + 1) for i in range(3)] == ["a", "b", "c"]
	table = parselmouth.praat.call("Create Table with column names", "test", 10, np.array(["a", "b", "c"], dtype=np.object_))
	assert [parselmouth.praat.call(table, "Get column label", i + 1) for i in range(3)] == ["a", "b", "c"]
	with pytest.raises(ValueError, match=r"Cannot convert argument \"\['a', 'b', 3\]\" to a known Praat argument type"):
		parselmouth.praat.call("Create Table with column names", "test", 10, ['a', 'b', 3])
	with pytest.raises(parselmouth.PraatError, match=r"Argument \".*\" should be a string array, not a number"):
		parselmouth.praat.call("Create Table with column names", "test", 10, 42)


def test_call_return_many(sound):
	stereo_sound = sound.convert_to_stereo()
	channels = parselmouth.praat.call(stereo_sound, "Extract all channels")
	assert isinstance(channels, list)
	assert len(channels) == 2
	assert np.all(channels[0].values == sound.values)
	assert np.all(channels[1].values == sound.values)

	channels = parselmouth.praat.call(sound, "Extract all channels")
	assert isinstance(channels, list)
	assert len(channels) == 1
	assert np.all(channels[0].values == sound.values)


def test_call_return_string(text_grid):
	assert isinstance(parselmouth.praat.call(text_grid, "Get tier name", 1), str)
	parselmouth.praat.call(text_grid, "Set tier name", 1, "abc")
	assert parselmouth.praat.call(text_grid, "Get tier name", 1) == "abc"


def test_call_return_complex():
	polynomial = parselmouth.praat.call("Create Polynomial", "", -5, 5, "1 0 1")
	roots = parselmouth.praat.call(polynomial, "To Roots")
	assert parselmouth.praat.call(roots, "Get number of roots") == 2
	assert {parselmouth.praat.call(roots, "Get root", i + 1) for i in range(2)} == {1j, -1j}

	assert parselmouth.praat.call("Get incomplete gamma", 1, 0, 0, -1) == pytest.approx(np.exp(1j))


def test_call_return_vector(spectrogram):
	frame_times = parselmouth.praat.call(spectrogram, "List all frame times")
	assert isinstance(frame_times, np.ndarray)
	assert frame_times.shape == (spectrogram.n_frames,)
	assert frame_times.dtype == np.float64
	assert np.all(frame_times == spectrogram.xs())

	# TODO Test integer vector once Praat has a command that returns one


def test_call_return_matrix(spectrogram):
	matrix = parselmouth.praat.call(spectrogram, "To Matrix")
	assert isinstance(matrix, parselmouth.Matrix)

	values = parselmouth.praat.call(matrix, "Get all values")
	assert isinstance(values, np.ndarray)
	assert values.shape == (spectrogram.ny, spectrogram.nx)
	assert values.dtype == np.float64
	assert np.all(values == matrix.values)


def test_call_return_string_vector():
	sentence = "Lorem ipsum dolor sit amet"
	strings = parselmouth.praat.call("Create Strings from tokens", "test", sentence, " ")
	assert isinstance(strings, parselmouth.Data)
	assert strings.class_name == "Strings"
	assert strings.name == "test"
	assert parselmouth.praat.call(strings, "Get number of strings") == 5

	strings_vector = parselmouth.praat.call(strings, "List all strings")
	assert isinstance(strings_vector, np.ndarray)
	assert strings_vector.shape == (5,)
	assert strings_vector.dtype == np.object_
	assert np.all(strings_vector == sentence.split(" "))


def test_run(sound_path):
	script = textwrap.dedent("""\
	Read from file: "{}"
	To Intensity: 100.0, 0.0, "yes"
	selectObject: 1
	selectObject: "Intensity the_north_wind_and_the_sun"
	""".format(sound_path))

	assert parselmouth.praat.run(script)[0] == parselmouth.Sound(sound_path).to_intensity()

	with pytest.raises(parselmouth.PraatError, match="Found 3 arguments but expected only 0."):
		parselmouth.praat.run(script, 42, "some_argument", True)


def test_run_with_parameters(sound_path):
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
	""".format(sound_path))

	min_pitch = 75
	time_step = 0.05
	subtract_mean = False

	assert parselmouth.praat.run(script, min_pitch, time_step, subtract_mean)[0] == parselmouth.Sound(sound_path).to_intensity(min_pitch, time_step, subtract_mean)

	with pytest.raises(parselmouth.PraatError, match="Found 0 arguments but expected more."):
		parselmouth.praat.run(script)


def test_run_with_extra_objects(sound):
	new = parselmouth.Sound(np.zeros((sound.n_channels, sound.n_samples)), sampling_frequency=sound.sampling_frequency, start_time=sound.start_time)
	sound.name = "the sound"
	parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]", extra_objects=[sound])
	assert sound == new

	with pytest.raises(parselmouth.PraatError, match=r"No such object \(note: variables start with nonupper case\)"):
		parselmouth.praat.run(new, "Formula: ~ self [col] + Sound_the_sound [col]")


def test_run_sys_stdout(capsys):
	parselmouth.praat.run("writeInfo: 42")
	assert capsys.readouterr().out == "42"
	parselmouth.praat.run("appendInfo: 42")
	assert capsys.readouterr().out == "42"
	parselmouth.praat.run("writeInfoLine: 42")
	assert capsys.readouterr().out == "42\n"
	parselmouth.praat.run("writeInfoLine: \"The answer\", \" - \", 42\nappendInfo: \"The question - ?\"")
	assert capsys.readouterr().out == "The answer - 42\nThe question - ?"
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


def test_run_file_relative_paths(sound_path, script_path):
	assert os.getcwd() != os.path.abspath(os.path.dirname(script_path))
	rel_sound_path = os.path.relpath(sound_path, os.path.dirname(script_path))
	assert parselmouth.praat.run_file(script_path, rel_sound_path)[0] == parselmouth.Sound(sound_path)


def test_run_file_keep_cwd(sound_path, script_path):
	assert os.getcwd() != os.path.abspath(os.path.dirname(script_path))
	rel_sound_path = os.path.relpath(sound_path, os.path.dirname(script_path))
	rel_to_cwd_sound_path = os.path.relpath(sound_path, os.getcwd())
	assert parselmouth.praat.run_file(script_path, rel_to_cwd_sound_path, keep_cwd=True)[0] == parselmouth.Sound(sound_path)
	with pytest.raises(parselmouth.PraatError, match=fr"Cannot open file .*{rel_sound_path}.*"):
		parselmouth.praat.run_file(script_path, rel_sound_path, keep_cwd=True)


def test_run_return_input_object(sound):
	assert parselmouth.praat.run(sound, "")[0] == sound


def test_remove_referenced_objects(sound):
	mean = sound.values.mean()
	assert parselmouth.praat.call(sound, "Remove") is None
	assert sound.values.mean() == mean

	many_sounds = [sound.copy() for _ in range(10)]
	assert parselmouth.praat.run(many_sounds[:5], "select all\nRemove", extra_objects=many_sounds[5:]) == []
	assert all(s.values.mean() == mean for s in many_sounds)


def test_call_no_objects(sound):
	assert parselmouth.praat.call(sound, "Get number of samples") == sound.n_samples
	with pytest.raises(parselmouth.PraatError, match="Command \"Get number of samples\" not available for given objects."):
		parselmouth.praat.call("Get number of samples")


def test_unknown_argument_type(sound, text_grid, tmp_path):
	class Something:
		pass

	with pytest.raises(ValueError, match="Cannot convert argument \"<.*Something.*>\" to a known Praat argument type"):
		parselmouth.praat.call("Read from file", Something())


def test_praat_callback_prefixes():
	all_caps_classes_regex = "|".join({'EEG', 'ERP', 'DTW', 'PCA', 'SSCP'})
	prefix_regex = rf'([A-Z0-9]+(?:_(?!{all_caps_classes_regex})[A-Z0-9]+)*_+)'

	separators = {'nullptr', '0'}
	all_actions = itertools.chain(parselmouth.praat._get_actions(), parselmouth.praat._get_menu_commands())
	prefixes = {re.match(prefix_regex, callback_name).group(1) for (_, _, callback_name) in all_actions if callback_name not in separators}

	values = {'REAL', 'INTEGER', 'BOOLEAN', 'COMPLEX', 'STRING', 'NUMVEC', 'NUMMAT', 'STRVEC'}
	objects = {'NEW', 'NEW1', 'NEWTIMES2', 'READ1', 'READMANY'}
	info = {'HINT', 'INFO', 'LIST'}
	nothing = {'HELP', 'MODIFY', 'PRAAT', 'SAVE', 'GRAPHICS'}
	exception = {'PLAY', 'WINDOW', 'MOVIE'}
	editor = {'EDITOR_ONE', 'EDITOR_ONE_WITH_ONE'}

	old_prefixes = {prefix[:-1] for prefix in prefixes if not prefix.endswith('__')}
	assert old_prefixes == values | objects | info | nothing | exception | editor

	query = ({f'QUERY_NONE_FOR_{x}' for x in {'COMPLEX', 'REAL'}} |
	         {f'QUERY_ONE_FOR_{x}' for x in {'BOOLEAN', 'INTEGER', 'MATRIX', 'REAL', 'REAL_VECTOR', 'STRING', 'STRING_ARRAY'}} |
	         {f'QUERY_ONE_WEAK_{x}' for x in {'FOR_STRING', 'AND_ONE_FOR_INTEGER', 'AND_ONE_FOR_REAL'}} |
			 {f'QUERY_ONE_AND_{x}' for x in {'ONE_FOR_BOOLEAN', 'ONE_FOR_REAL', 'ONE_AND_ALL_FOR_REAL', 'ONE_AND_ONE_FOR_REAL'}} |
	         {'QUERY_TWO_AND_ONE_FOR_REAL', 'QUERY_TWO_FOR_REAL'})
	convert = ({f'CONVERT_{x}' for x in {'ALL_TO_MULTIPLE', 'EACH_TO_ONE', 'EACH_TO_MULTIPLE', 'EACH_WEAK_TO_ONE'}} |
	           {f'CONVERT_ONE_{x}' for x in {'TO_MULTIPLE', 'AND_ALL_LISTED_TO_ONE', 'AND_ALL_LISTED_TO_ONE', 'AND_ALL_TO_MULTIPLE', 'WEAK_AND_ONE_TO_ONE'}} |
	           {f'CONVERT_ONE_AND_ONE_{x}' for x in {'TO_ONE', 'TO_MULTIPLE', 'AND_ALL_TO_MULTIPLE', 'GENERIC_TO_ONE'}} |
			   {f'CONVERT_ONE_AND_ONE_AND_ONE_{x}' for x in {'TO_ONE'}} |
			   {f'CONVERT_TWO_{x}' for x in {'TO_ONE', 'TO_MULTIPLE', 'AND_ONE_TO_ONE'}} |
			   {'NEW', 'COMBINE_ALL_TO_ONE', 'COMBINE_ALL_LISTED_TO_ONE'})
	modify = ({'MODIFY', 'MODIFY_ALL', 'MODIFY_EACH', 'MODIFY_EACH_WEAK'} |
	          {f'MODIFY_FIRST_OF_ONE_{x}' for x in {'AND_ONE', 'AND_ALL', 'AND_ONE_AND_ONE', 'WEAK_AND_ONE', 'WEAK_AND_ONE_WITH_HISTORY', 'WEAK_AND_TWO'}})
	create = {'CREATE_ONE', 'CREATE_MULTIPLE'}
	info = {'INFO_NONE', 'INFO_ONE', 'INFO_ONE_AND_ONE', 'INFO_TWO', 'LIST'}
	read_save = {'READ_ONE', 'READ_MULTIPLE', 'SAVE_ALL', 'SAVE_ONE', 'SAVE_TWO'}
	graphics = {f'GRAPHICS_{x}' for x in {'EACH', 'NONE', 'ONE_AND_ONE', 'TWO', 'TWO_AND_ONE'}}
	play_record = {'PLAY', 'PLAY_EACH', 'PLAY_ONE_AND_ONE', 'RECORD_ONE'}
	editor_window = {'EDITOR_ONE', 'EDITOR_ONE_WITH_ONE', 'EDITOR_ONE_WITH_ONE_AND_ONE', 'CREATION_WINDOW', 'SINGLETON_CREATION_WINDOW'}
	etc = {'HELP', 'PRAAT', 'APPEND_ALL', 'PRAAT', 'PREFS', 'HINT', 'WARNING'}

	new_prefixes = {prefix[:-2] for prefix in prefixes if prefix.endswith('__')}
	assert new_prefixes == query | convert | modify | create | info | read_save | graphics | play_record | editor_window | etc


def test_collection_object(sound, text_grid, tmp_path):
	collection_path = tmp_path / "sound_and_text_grid.Collection"
	parselmouth.praat.call([sound, text_grid], "Save as text file", str(collection_path))
	[reread_sound, reread_text_grid] = parselmouth.praat.call("Read from file", str(collection_path))
	assert reread_sound == sound
	assert reread_text_grid == text_grid
