#!/bin/sh
set -e

test $# -eq 1 || (echo "Expected exactly one Praat version. Got: '$*'" >&2; exit 1)

praat_version=$1
toplevel=$(git rev-parse --show-toplevel)

echo "Running git subtree pull..."
# Merge conflicts result in `git subtree pull` failing with error code 1.
# If we're sure there's no merge going on before, and on error, check if there is now a pending merge.
# If there's a pending merge, we're good! If not, print the actual error and exit.
test ! -f "$toplevel/.git/MERGE_HEAD" || (echo "Ongoing merge. Aborting." >&2; exit 1)
stderr=$(git subtree pull --prefix praat/ --squash git@github.com:praat/praat.git "$praat_version" 2>&1 >/dev/null) || test -f "$toplevel/.git/MERGE_HEAD" || (echo "$stderr" >&2; exit 1)

echo "Undoing merge..."
if test -f "$toplevel/.git/MERGE_HEAD"
then
	squash_commit=$(cat "$toplevel/.git/MERGE_HEAD")
	git merge --abort
else
	squash_commit=$(git rev-parse HEAD^2)
	git reset --hard HEAD^
fi

echo "Pruning Praat docs/ folder..."
squash_parent=$(git rev-parse $squash_commit^)
squash_message=$(git show -s --format=%B $squash_commit)
new_squash_tree=$(git ls-tree $squash_commit --full-tree | grep -v "\bdocs\b" | git mktree)
new_squash_commit=$(git commit-tree $new_squash_tree -p $squash_parent -m "$squash_message")

# More or less equivalent to:
#   git checkout -q $squash_commit
#   git rm -r "$toplevel/docs/"  # $squash_commit has Praat's root as working dir
#   git commit --amend --no-edit
#   $new_squash_commit=$(git rev-parse HEAD)
#   git checkout -q -

echo "Redoing merge..."
git merge -Xsubtree='praat/' -m "Update Praat subtree to $praat_version" $new_squash_commit
