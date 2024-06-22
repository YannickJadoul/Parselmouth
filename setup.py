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

if os.name == "nt":
    # set environmental paths to build with the latest visual studio version
    import vswhere, subprocess as sp, platform

    vsinfo = vswhere.find_first(latest=True, products=["*"])
    if vsinfo is not None:  # VSC++ found
        vcvarsall = os.path.join(
            vsinfo["installationPath"], "VC", "Auxiliary", "Build", "vcvarsall.bat"
        )
        arch = (
            platform.machine().lower()
        )  # x86 | amd64 | x86_amd64 | x86_arm | x86_arm64 | amd64_x86 | amd64_arm | amd64_arm64
        ret = sp.run(
            f"set && cls && \"{vcvarsall}\" {arch} && cls && set", shell=True, stdout=sp.PIPE,stderr=sp.PIPE
        )
        for string in ret.stdout.decode("utf8").splitlines():
            # vsvars.bat likes to print some fluff at the beginning.
            # Skip lines that don't look like environment variables.
            if "=" not in string:
                continue

            name, new_value = string.split("=", 1)
            old_value = os.getenv(name, None)

            # For new variables "old_value === undefined".
            if new_value != old_value:
                # Special case for a bunch of PATH-like variables: vcvarsall.bat
                # just prepends its stuff without checking if its already there.
                # This makes repeated invocations of this action fail after some
                # point, when the environment variable overflows. Avoid that.
                if name.upper() in ["PATH", "INCLUDE", "LIB", "LIBPATH"]:
                    # Remove duplicates by keeping the first occurance and preserving order.
                    # This keeps path shadowing working as intended.
                    paths = new_value.split(os.pathsep)
                    paths = [p for i, p in enumerate(paths) if paths.index(p) == i]
                    new_value = os.pathsep.join(paths)

                os.environ[name] = new_value

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
