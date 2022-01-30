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
