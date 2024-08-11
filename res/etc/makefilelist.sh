# Copyright (C) 2016-2023  Yannick Jadoul
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

SCRIPT_PATH=$(realpath res/etc/praat_makefile_to_cmake.py)

for SUBDIR in $(find . -mindepth 3 -name 'Makefile' | xargs dirname)
do
	if [ -f "$SUBDIR/CMakeLists.txt" ]
	then
	  pushd "$SUBDIR" > /dev/null
		CONTENTS=$(python $SCRIPT_PATH Makefile) && cmp -s CMakeLists.txt <<< "$CONTENTS" || meld CMakeLists.txt <(echo "$CONTENTS")
		popd > /dev/null
	fi
done
