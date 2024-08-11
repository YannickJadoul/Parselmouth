# Copyright (C) 2024  Yannick Jadoul
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

import pathlib
import re
import sys


def get_makefile_variable(contents, variable, default=None):
	match = re.search(rf'^{variable} = ((?:\\\n|.)+)$', contents, re.MULTILINE)
	if match is None:
		return default
	return re.sub(r'\$\((\w+)\)', lambda m: get_makefile_variable(contents, m.group(1)), match.group(1))


def get_source_for_object(obj):
	path = pathlib.Path(obj)
	assert path.suffix == ".o"
	return next(str(path.with_suffix(ext)) for ext in [".cpp", ".c"] if path.with_suffix(ext).exists())


with open(sys.argv[1]) as f:
	makefile_contents = f.read()

objects = get_makefile_variable(makefile_contents, 'OBJECTS')
objects_lines = [re.sub(r' +', ' ', l.strip()) for l in objects.replace("\\\n", "\n").splitlines()]
source_lines = [re.sub(r'\S+\.o', lambda m: get_source_for_object(m.group()), l) for l in objects_lines if l.strip()]
sources = '\n\t'.join(source_lines)

praat_subdir = pathlib.Path(__file__).resolve().parents[2] / 'praat'
cppflags = get_makefile_variable(makefile_contents, 'CPPFLAGS', default="")
relative_include_dirs = [str(pathlib.Path(d).resolve().relative_to(praat_subdir)) for d in re.findall(r'-I (\S+)', cppflags)]
include_dirs = ' '.join(relative_include_dirs)

cmake_content = f"""\
add_praat_subdir(SOURCES
	{sources}""" + (f"""
INCLUDE_DIRS
	{include_dirs}""" if include_dirs else "") + """
)"""
print(cmake_content)
