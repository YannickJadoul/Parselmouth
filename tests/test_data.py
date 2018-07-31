# -*- coding: utf-8 -*-

import pytest

import parselmouth


def test_read_nonexistent():
	with pytest.raises(parselmouth.PraatError, match=r'Cannot open file “.*nonexistent.wav”\.'):
		parselmouth.Data.read("nonexistent.wav")
