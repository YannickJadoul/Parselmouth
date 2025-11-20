# Copyright (C) 2017-2022  Yannick Jadoul
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

from __future__ import print_function

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

if sys.version_info < (3, 6):
	VS_YEAR_TO_MSC_VER = {
		"2017":"1910",  # VS 2017 - can be +9
		"2019":"1920",  # VS 2019 - can be +9
		"2022":"1930",  # VS 2022 - can be +9
	}

	def patched_WindowsPlatform_init(self):
		import textwrap
		from skbuild.platform_specifics.windows import WindowsPlatform, CMakeVisualStudioCommandLineGenerator, CMakeVisualStudioIDEGenerator

		super(WindowsPlatform, self).__init__()

		supported_vs_years = [("2022", "v143"), ("2019", "v142"), ("2017", "v141")]
		self._vs_help = textwrap.dedent("""
			Building Windows wheels requires Microsoft Visual Studio 2017, 2019, or 2022:
	
			  https://visualstudio.microsoft.com/vs/
			""").strip()

		try:
			import ninja  # pylint: disable=import-outside-toplevel

			ninja_executable_path = os.path.join(ninja.BIN_DIR, "ninja")
			ninja_args = ["-DCMAKE_MAKE_PROGRAM:FILEPATH=" + ninja_executable_path]
		except ImportError:
			ninja_args = []

		extra = []
		for vs_year, vs_toolset in supported_vs_years:
			vs_version = VS_YEAR_TO_MSC_VER[vs_year]
			args = ["-D_SKBUILD_FORCE_MSVC={}".format(vs_version)]
			self.default_generators.extend(
				[
					CMakeVisualStudioCommandLineGenerator("Ninja", vs_year, vs_toolset, args=ninja_args + args),
					CMakeVisualStudioIDEGenerator(vs_year, vs_toolset),
				]
			)
			extra.append(CMakeVisualStudioCommandLineGenerator("NMake Makefiles", vs_year, vs_toolset, args=args))
		self.default_generators.extend(extra)

	import skbuild.platform_specifics.windows
	skbuild.platform_specifics.windows.WindowsPlatform.__init__ = patched_WindowsPlatform_init


def find_version(*file_paths):
	with io.open(os.path.join(os.path.dirname(__file__), "src", "version.h"), encoding='utf8') as f:
		version_file = f.read()
	version_match = re.search(r"^#define PARSELMOUTH_VERSION ([0-9a-z.]+)$", version_file, re.M)
	if version_match:
		return version_match.group(1)
	raise RuntimeError("Unable to find version string.")


setup(
	version=find_version(),
	packages=[''],
	package_dir={'': "src"},
	cmake_args=shlex.split(os.environ.get('PARSELMOUTH_EXTRA_CMAKE_ARGS', '')),
	cmake_install_dir="src",
)
