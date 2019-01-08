import parselmouth


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
