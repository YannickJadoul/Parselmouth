import pytest


# TODO 'thing' resource fixture
def test_name(sound):
	assert sound.name is None
	sound.name = "a sound (with disallowed characters!)"
	assert sound.name == "a_sound__with_disallowed_characters__"
	assert sound.class_name == "Sound"
	assert sound.full_name == sound.class_name + " \"" + sound.name + "\""


def test_no_name(sound):
	assert sound.name is None
	sound.name = ""
	assert sound.name is not None
	sound.name = None
	assert sound.class_name == "Sound"
	assert sound.full_name == sound.class_name


def test_info(sound):
	assert sound.info() == str(sound)
	sound.name = "a sound"
	assert sound.info() == str(sound)
