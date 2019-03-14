from __future__ import unicode_literals  # Python 2 compatibility

import pytest

import parselmouth

from future.utils import text_to_native_str  # Python 2 compatibility


def test_read():
	assert parselmouth.Data.read == parselmouth.read


def test_read_nonexistent():
	with pytest.raises(parselmouth.PraatError, match=text_to_native_str(r'Cannot open file \u201c.*nonexistent.wav\u201d\.', encoding='utf-8')):
		parselmouth.read("nonexistent.wav")


# TODO Other encodings
