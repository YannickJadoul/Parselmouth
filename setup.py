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


def read(*args):
	with io.open(os.path.join(os.path.dirname(__file__), *args), encoding='utf8') as f:
		return f.read()


def find_version(*file_paths):
	version_file = read(*file_paths)
	version_match = re.search(r"^#define PARSELMOUTH_VERSION ([0-9a-z.]+)$", version_file, re.M)
	if version_match:
		return version_match.group(1)
	raise RuntimeError("Unable to find version string.")


long_description="""\
# Parselmouth - Praat in Python, the Pythonic way

**Parselmouth** is a Python library for the [Praat](http://www.praat.org)
software.

Though other attempts have been made at porting functionality from Praat to
Python, Parselmouth is unique in its aim to provide a complete and Pythonic
interface to the internal Praat code. While other projects either wrap Praat's
scripting language or reimplementing parts of Praat's functionality in Python,
Parselmouth directly accesses Praat's C/C++ code (which means the algorithms
and their output are exactly the same as in Praat) and provides efficient
access to the program's data, but *also* provides an interface that looks
no different from any other Python library.

Drop by our [Gitter chat room](https://gitter.im/PraatParselmouth/Lobby) or
post a message to our
[Google discussion group](https://groups.google.com/d/forum/parselmouth) if
you have any question, remarks, or requests!

More information on the **installation** and some basic **examples**
can be found on Parselmouth's GitHub repository:
https://github.com/YannickJadoul/Parselmouth"""

setup(
	name='praat-parselmouth',
	version=find_version("src", "version.h"),
	description="Praat in Python, the Pythonic way",
	long_description=long_description,
	long_description_content_type='text/markdown',
	url="https://github.com/YannickJadoul/Parselmouth",
	project_urls={
		"Bug Tracker": "https://github.com/YannickJadoul/Parselmouth/issues",
		"Documentation": "https://parselmouth.readthedocs.io/",
		"Source Code": "https://github.com/YannickJadoul/Parselmouth",
	},
	author="Yannick Jadoul",
	author_email="Yannick.Jadoul@ai.vub.ac.be",
	license='GPLv3',
	classifiers=[
		'Development Status :: 5 - Production/Stable',
		'Intended Audience :: Developers',
		'Intended Audience :: Science/Research',
		'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
		'Operating System :: MacOS :: MacOS X',
		'Operating System :: Microsoft :: Windows',
		'Operating System :: POSIX :: Linux',
		'Operating System :: Unix',
		'Programming Language :: C++',
		'Programming Language :: Python :: 2',
		'Programming Language :: Python :: 2.7',
		'Programming Language :: Python :: 3',
		'Programming Language :: Python :: 3.5',
		'Programming Language :: Python :: 3.6',
		'Programming Language :: Python :: 3.7',
		'Programming Language :: Python :: 3.8',
		'Topic :: Scientific/Engineering',
		'Topic :: Software Development :: Libraries :: Python Modules',
	],
	keywords=["praat", "speech", "signal processing", "phonetics"],
	python_requires='>=2.7,!=3.0.*,!=3.1.*,!=3.2.*,!=3.3.*,!=3.4.*',
	install_requires=[
		'numpy>=1.7.0',
	],
	ext_modules=[CMakeExtension('parselmouth')],
	cmdclass=dict(build_ext=CMakeBuild),
	zip_safe=False,
)
