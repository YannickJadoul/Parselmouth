# Copyright (C) 2019-2022  Yannick Jadoul
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

import parselmouth

import textwrap


def test_stdout(sound, capsys):
	pitch = sound.to_pitch_ac()
	other_pitch = sound.to_pitch_ac(voicing_threshold=0.3)
	assert pitch != other_pitch

	parselmouth.praat.run([pitch, other_pitch], "Count differences")
	assert capsys.readouterr() == (pitch.count_differences(other_pitch), "")


def test_stderr(sound, capsys):
	pitch = sound.to_pitch_ac()
	other_pitch = sound.to_pitch_cc()
	assert pitch != other_pitch
	assert len(pitch) != len(other_pitch)

	parselmouth.praat.run([pitch, other_pitch], "Count differences")
	assert capsys.readouterr() == ("", "Pitch_difference: these Pitches are not aligned.\n")

	pitch.count_differences(other_pitch)
	assert capsys.readouterr() == ("", "Pitch_difference: these Pitches are not aligned.\n")


def test_melder_open_while_diverted(sound, capsys):
	script = """\
	writeInfoLine: "BEFORE"
	ignore$ = Report system properties
	appendInfoLine: "AFTER"
	"""

	parselmouth.praat.run(script)
	assert capsys.readouterr() == ("BEFORE\nAFTER\n", "")
