import pytest

import re

from builtins import str  # Python 2 compatibility


def test_name(thing):
	assert thing.name is None
	thing.name = "a thing (with disallowed characters!)"
	assert thing.name == "a_thing__with_disallowed_characters__"
	assert thing.class_name == thing.__class__.__name__
	assert thing.full_name == thing.class_name + " \"" + thing.name + "\""


def test_no_name(thing):
	assert thing.name is None
	thing.name = ""
	assert thing.name is not None
	thing.name = None
	assert thing.name is None
	assert thing.class_name == thing.__class__.__name__
	assert thing.full_name == thing.class_name


def test_info(thing):
	def clear_date(info_str):
		cleared, count = re.subn(r'^Date: .*\d\d:\d\d:\d\d.*', '', info_str, flags=re.MULTILINE)
		assert count == 1
		return cleared

	assert clear_date(thing.info()) == clear_date(str(thing))
	thing.name = "a thing"
	assert clear_date(thing.info()) == clear_date(str(thing))
