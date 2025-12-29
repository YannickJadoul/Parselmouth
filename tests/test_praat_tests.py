# Copyright (C) 2019-2023  Yannick Jadoul
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

import pathlib
import sys


pytestmark = pytest.mark.praat_test

PRAAT_BASE_DIR = pathlib.Path(__file__).resolve().parent.parent / "praat"
PRAAT_TEST_BASE_DIR = PRAAT_BASE_DIR / "test"
PRAAT_DWTEST_BASE_DIR = PRAAT_BASE_DIR / "dwtest"

PRAAT_TEST_IGNORE_SUBDIRS = ["manually", "speed"]


def find_praat_test_files():
	for dir in PRAAT_TEST_BASE_DIR.glob("*/"):
		if dir.name in PRAAT_TEST_IGNORE_SUBDIRS:
			continue
		for fn in dir.glob("**/*.praat"):
			if "_GUI_" in fn.name:
				continue
			rel_fn = fn.relative_to(PRAAT_TEST_BASE_DIR).as_posix()
			marks = []
			assert tuple(map(int, parselmouth.PRAAT_VERSION.split('.'))) <= (6, 4, 48), "Remove on Praat update"
			if rel_fn in ["sys/large PDF/large PDF.praat"]:
				marks.append(pytest.mark.skipif(sys.platform == 'win32', reason="Not available on Windows, and test removed in Praat 6.4.49"))
			yield pytest.param(str(fn), id=rel_fn, marks=marks)


def find_praat_dwtest_files():
	for fn in PRAAT_DWTEST_BASE_DIR.glob("test_*.praat"):
		if "_GUI_" in fn.name:
			continue
		rel_fn = fn.relative_to(PRAAT_DWTEST_BASE_DIR).as_posix()
		yield pytest.param(str(fn), id=rel_fn, marks=[])


PRAAT_TEST_FILES = sorted(find_praat_test_files())
PRAAT_DWTEST_FILES = sorted(find_praat_dwtest_files())


@pytest.fixture(scope='function', autouse=True)
def praat_random_seed():
	parselmouth.praat.run("random_initializeWithSeedUnsafelyButPredictably(5489)")
	yield
	parselmouth.praat.run("random_initializeSafelyAndUnpredictably()")


@pytest.mark.parametrize('test_file', PRAAT_TEST_FILES)
def test_praat_test(test_file):
	assert parselmouth.praat.run_file(test_file) == []


@pytest.mark.parametrize('test_file', PRAAT_DWTEST_FILES)
def test_praat_dwtest(test_file):
	assert parselmouth.praat.run_file(test_file) == []
