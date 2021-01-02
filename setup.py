# Copyright (C) 2017-2020  Yannick Jadoul
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
import platform
import re
import shlex
import subprocess
import sys

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# setuptools to CMake solution based on https://github.com/pybind/cmake_example/

class CMakeExtension(Extension):
	def __init__(self, name, sourcedir=""):
		Extension.__init__(self, name, sources=[])
		self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
	def build_extension(self, ext):
		extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
		if not extdir.endswith(os.path.sep):
			extdir += os.path.sep

		cfg = 'Debug' if self.debug else 'Release'

		cmake_args = [
			'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}'.format(extdir),
			'-DPython_EXECUTABLE={}'.format(sys.executable),
			'-DCMAKE_BUILD_TYPE={}'.format(cfg),
		]
		build_args = []

		if self.compiler.compiler_type == 'msvc':
			cmake_args += ['-A', 'Win32' if self.plat_name == 'win32' else 'x64']
			cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
			build_args += ['--config', cfg]

		cmake_args += shlex.split(os.environ.get('PARSELMOUTH_EXTRA_CMAKE_ARGS', ''))
		build_args += shlex.split(os.environ.get('PARSELMOUTH_EXTRA_BUILD_ARGS', ''))

		if not os.path.exists(self.build_temp):
			os.makedirs(self.build_temp)

		subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp)
		subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)




def find_version(*file_paths):
	with io.open(os.path.join(os.path.dirname(__file__), "src", "version.h"), encoding='utf8') as f:
		version_file = f.read()
	version_match = re.search(r"^#define PARSELMOUTH_VERSION ([0-9a-z.]+)$", version_file, re.M)
	if version_match:
		return version_match.group(1)
	raise RuntimeError("Unable to find version string.")


setup(
	version=find_version(),
	ext_modules=[CMakeExtension('parselmouth')],
	cmdclass=dict(build_ext=CMakeBuild),
)
