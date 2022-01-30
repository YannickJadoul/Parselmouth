# Copyright (C) 2019-2022  Yannick Jadoul
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

# git diff --color=always "$@" | grep -E --color=always -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -B 5 -A 5 | less -R
# git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk | colordiff | grep --color=always -f locale_regexes -e '' | less -R
# git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk | grep --color=always -f locale_regexes -e '' | ydiff -s -w 0 --wrap 

trap "exit" INT

locale_regexes="$(dirname "$(readlink -f "$0")")/locale_regexes"

TMP_DIR=$(mktemp -d)
git diff "$@" | grepdiff -f "$locale_regexes" -E --output=hunk > "$TMP_DIR/diff"
splitdiff -p 1 -a "$TMP_DIR/diff"
for part in $TMP_DIR/*.patch
do
	meld <(patch -R --strip=1 -i "$part" -o -) $(lsdiff --strip=1 "$part")
done
rm -r "$TMP_DIR"

git diff "$@" | grepdiff -f "$locale_regexes" -E --output=hunk | grep --color=always -f "$locale_regexes" -e '' | ydiff -s -w 0 --wrap
