# git diff --color=always "$@" | grep -E --color=always -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -B 5 -A 5 | less -R
# git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk | colordiff | grep --color=always -f locale_regexes -e '' | less -R
# git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk | grep --color=always -f locale_regexes -e '' | ydiff -s -w 0 --wrap 

trap "exit" INT

TMP_DIR=$(mktemp -d)
git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk > "$TMP_DIR/diff"
splitdiff -p 1 -a "$TMP_DIR/diff"
for part in $TMP_DIR/*.patch
do
	meld <(patch -R --strip=1 -i "$part" -o -) $(lsdiff --strip=1 "$part")
done
rm -r "$TMP_DIR"

git diff "$@" | grepdiff -f "$(dirname "$(readlink -f "$0")")/locale_regexes" -E --output=hunk | grep --color=always -f locale_regexes -e '' | ydiff -s -w 0 --wrap
