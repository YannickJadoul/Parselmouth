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

import parselmouth

import inspect
import itertools
import re


def dict_members(obj, predicate=None):
	# Unlike 'inspect.getmembers', this doesn't return inherited members
	return filter(predicate, obj.__dict__.values())


def all_members(obj):
	for member in dict_members(obj, inspect.isroutine): yield member  # Python 2 compatibility; Python 3: use 'yield from'

	for member in dict_members(obj, lambda m: inspect.isclass(m) or inspect.ismodule(m)):
		yield member
		for x in all_members(member): yield x  # Python 2 compatibility; Python 3: use 'yield from'


def all_docstrings(obj):
	for member in itertools.chain((obj,), all_members(obj)):
		docstring = getattr(member, '__doc__', None)  # __doc__ instead of inspect.getdoc to not replace tabs
		if docstring is not None:
			yield member, docstring


def is_signature(line, name):
	return bool(re.match(r'^(?:\d+\. )?{}\(.*\)(?: -> .+)?$'.format(name), line))  # Python 2 compatibility; Python 3: re.fullmatch


def test_docstring_formatting():
	for _, docstring in all_docstrings(parselmouth):
		assert '\t' not in docstring
		assert not docstring.startswith("\n")


def test_docstring_line_lengths():
	for member, docstring in all_docstrings(parselmouth):
		for line in docstring.splitlines():
			assert len(line) <= 75 or is_signature(line, member.__name__)
