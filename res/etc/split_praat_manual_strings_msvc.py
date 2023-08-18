# Copyright (C) 2023  Yannick Jadoul
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


"""Short explanation:
- Praat has discovered very, very long raw strings for its manual.
- For some reason, MSVC (currently) has a maximum length for the string
literals it can handle (16380 *bytes*). But putting two automatically
concatenated string literals, like `"abc" "def"`, is apparently fine.

So, here we do this automatically.
Rerun this script whenever necessary, as it will first remove all
previously inserted breaks.
"""

import pathlib
import re


PRAAT_RAW_STRING_START = b'R"~~~('
PRAAT_RAW_STRING_END = b')~~~"'

MSVC_MAX_LENGTH = 16380


def split_string(match):
    lines = match.group(1).splitlines(keepends=True)
    N = 0
    for i, line in enumerate(lines):
        assert b'\r' not in line
        if N + len(line) > MSVC_MAX_LENGTH:
            lines[i-1] = lines[i-1][:-1] + PRAAT_RAW_STRING_END + b' ' + PRAAT_RAW_STRING_START + lines[i-1][-1:]
            N = 0
        N += len(line)
    return PRAAT_RAW_STRING_START + b''.join(lines) + PRAAT_RAW_STRING_END


def split_strings_in_file(path):
    with path.open('rb') as f:
        content = f.read()

    content = re.sub(re.escape(PRAAT_RAW_STRING_END) + rb'\s+' + re.escape(PRAAT_RAW_STRING_START), b'', content)
    content = re.sub(re.escape(PRAAT_RAW_STRING_START) + rb'(.*?)' + re.escape(PRAAT_RAW_STRING_END), split_string, content, flags=re.DOTALL)  # Non-greedy!

    with path.open('wb') as f:
        f.write(content)


def main():
    praat_subdir = pathlib.Path(__file__).parents[2] / 'praat'
    for manual_file in praat_subdir.glob("**/manual_*.cpp"):
        split_strings_in_file(manual_file)


if __name__ == '__main__':
    main()
