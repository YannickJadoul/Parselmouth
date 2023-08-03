# Copyright (C) 2017-2023  Yannick Jadoul
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

import io
import os
import re
import shlex
import sys

try:
	from skbuild import setup
except ImportError:
	print("Please update pip to pip 10 or greater, or a manually install the PEP 518 requirements in pyproject.toml", file=sys.stderr)
	raise


def find_version(*file_paths):
	with open(os.path.join(os.path.dirname(__file__), "src", "version.h"), encoding='utf8') as f:
		version_file = f.read()
	version_match = re.search(r"^#define PARSELMOUTH_VERSION ([0-9a-z.]+)$", version_file, re.M)
	if version_match:
		return version_match.group(1)
	raise RuntimeError("Unable to find version string.")


setup(
	version=find_version(),
	packages=[''],
	package_dir={'': "src"},
	include_package_data=False,
	cmake_args=shlex.split(os.environ.get('PARSELMOUTH_EXTRA_CMAKE_ARGS', '')),
	cmake_install_dir="src",
)
