# Copyright (C) 2017  Yannick Jadoul
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
import subprocess
import sys

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

# setuptools to CMake solution by https://github.com/pybind/cmake_example/

class CMakeExtension(Extension):
	def __init__(self, name, sourcedir=''):
		Extension.__init__(self, name, sources=[])
		self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
	def run(self):
		try:
			import cmake
			cmake_executable = os.path.join(cmake.CMAKE_BIN_DIR, 'cmake')
		except ImportError:
			cmake_executable = 'cmake'

		try:
			out = subprocess.check_output([cmake_executable, '--version'])
		except OSError:
			raise RuntimeError("CMake must be installed to build the following extensions: " +
							   ", ".join(e.name for e in self.extensions))

		if platform.system() == "Windows":
			cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
			if cmake_version < '3.1.0':
				raise RuntimeError("CMake >= 3.1.0 is required on Windows")

		for ext in self.extensions:
			self.build_extension(ext, cmake_executable)

	def build_extension(self, ext, cmake_executable):
		extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
		cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
					  '-DPYTHON_EXECUTABLE=' + sys.executable]

		cfg = 'Debug' if self.debug else 'Release'
		build_args = ['--config', cfg]

		if platform.system() == "Windows":
			cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
			if sys.maxsize > 2**32:
				cmake_args += ['-A', 'x64']
			build_args += ['--', '/m']
		else:
			cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
			build_args += ['--', '-j2']

		env = os.environ.copy()
		env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''), self.distribution.get_version())
		if not os.path.exists(self.build_temp):
			os.makedirs(self.build_temp)
		subprocess.check_call([cmake_executable, ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
		subprocess.check_call([cmake_executable, '--build', '.'] + build_args, cwd=self.build_temp)

def read(*names, **kwargs):
	with io.open(os.path.join(os.path.dirname(__file__), *names), encoding=kwargs.get("encoding", "utf8")) as fp:
		return fp.read()

def find_version(*file_paths):
	version_file = read(*file_paths)
	version_match = re.search(r"^#define PARSELMOUTH_VERSION ([0-9a-z.]+)$", version_file, re.M)
	if version_match:
		return version_match.group(1)
	raise RuntimeError("Unable to find version string.")

setup(
	name='praat-parselmouth',
	version=find_version('src', 'version.h'),
	description='Praat in Python, the Pythonic way',
	long_description=read('README.md'),
	url='https://github.com/YannickJadoul/Parselmouth',
	author='Yannick Jadoul',
	author_email='Yannick.Jadoul@ai.vub.ac.be',
	license='GPLv3',
	classifiers=[
		'Intended Audience :: Developers',
		'Intended Audience :: Science/Research',
		'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
		'Operating System :: MacOS :: MacOS X',
		'Operating System :: POSIX :: Linux',
		'Operating System :: Unix',
		'Programming Language :: C++',
		'Programming Language :: Python :: 2',
		'Programming Language :: Python :: 2.7',
		'Programming Language :: Python :: 3',
		'Programming Language :: Python :: 3.3',
		'Programming Language :: Python :: 3.4',
		'Programming Language :: Python :: 3.5',
		'Programming Language :: Python :: 3.6',
		'Topic :: Scientific/Engineering',
		'Topic :: Software Development :: Libraries :: Python Modules',
	],
	keywords='praat speech signal processing phonetics',
	install_requires=[
		'numpy>=1.7.0',
	],
	ext_modules=[CMakeExtension('parselmouth')],
	cmdclass=dict(build_ext=CMakeBuild),
	zip_safe=False,
)
