# Copyright (C) 2016-2019  Yannick Jadoul
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

for SUBDIR in $(find . -name 'Makefile' | xargs dirname)
do
	if [ -f "$SUBDIR/CMakeLists.txt" ]
	then
		(cd $SUBDIR && CONTENTS=$(egrep -o '([[:graph:]]+\.o *)+' Makefile | sed -E 's/([[:graph:]]+)\.o/ls \1.c* | tr "\n" " " ;/ge' | sed '1!s/^/            /g' | sed 's/ $//' | sed '1s/^/add_sources(/' | sed '$s/$/)/') && cmp -s CMakeLists.txt <<< "$CONTENTS" || meld CMakeLists.txt <(echo "$CONTENTS"))
	fi
done
