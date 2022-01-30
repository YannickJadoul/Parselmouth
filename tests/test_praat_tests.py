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

try:
	import glob2 as glob  # Python 2 compatibility
except ImportError:
	import glob
import os
import sys


pytestmark = pytest.mark.praat_test

PRAAT_BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..",  "praat"))
PRAAT_TEST_BASE_DIR = os.path.join(PRAAT_BASE_DIR, "test")
PRAAT_DWTEST_BASE_DIR = os.path.join(PRAAT_BASE_DIR, "dwtest")

PRAAT_TEST_IGNORE_SUBDIRS = ["manually", "speed"]


def find_praat_test_files():
	for dir in glob.iglob(os.path.join(PRAAT_TEST_BASE_DIR, "*", "")):
		if os.path.basename(os.path.dirname(dir)) in PRAAT_TEST_IGNORE_SUBDIRS:
			continue
		for fn in glob.iglob(os.path.join(dir, "**", "*.praat"), recursive=True):
			if "_GUI_" in fn:
				continue
			rel_fn = os.path.relpath(fn, PRAAT_TEST_BASE_DIR)
			yield pytest.param(fn, id=rel_fn)


def find_praat_dwtest_files():
	for fn in glob.iglob(os.path.join(PRAAT_DWTEST_BASE_DIR, "test_*.praat")):
		if "_GUI_" in fn:
			continue
		rel_fn = os.path.relpath(fn, PRAAT_DWTEST_BASE_DIR)
		marks = []
		if rel_fn in ["test_SpeechSynthesizer.praat",
		              "test_SpeechSynthesizer_alignment.praat",
		              "test_alignment.praat",
		              "test_bss_twoSoundsMixed.praat"]:
			marks.append(pytest.mark.skipif(sys.platform == 'win32', reason="tests hang on AppVeyor CI; debugging further after Praat update"))
		yield pytest.param(fn, id=rel_fn, marks=marks)


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
