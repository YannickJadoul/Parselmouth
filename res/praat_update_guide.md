Guide to update the Praat version of the git subtree in `praat/`:
- Look up the latest Praat release tag: https://github.com/praat/praat/releases.
- Run `res/update_praat_subtree.sh <praat-version>` (in a Unix terminal or git bash on Windows).
  In short, this will `git subtree merge` the new version without the `docs/` subfolder.
  In more detail, this script will:
  - Fetch the corresponding commit, and subtree squash-and-merge: `git subtree pull --prefix praat/ --squash git@github.com:praat/praat.git <praat-version>`.
  - If the merge did not succeed, record the hash of the squashed commit with Praat changes and abort the pending merge.
  - If the merge succeeded, record the hash of the squashed-and-merged commit with Praat changes and reset to before the merge.
  - Amend the squashed commit, removing the full `doc/` subfolder.
  - Merge that amended commit: `git merge -Xsubtree='praat/' -m "Update Praat subtree to <praat-version>" <new-squash-commit>`.
- Resolve (possibly a lot of) merge conflicts.
- Check for source files added or deleted, and adapt the `CMakeLists.txt` files: `res/etc/makefilelist.sh`.
- Check for changes in Praat's `makefile.defs` files: `git difftool HEAD^ -- praat/makefiles`.
- Check for changes related to global locale-sensitive functions: `res/etc/locale_regex_diff.sh HEAD^`.
- Check for and fix string literals that are too long for MSVC: `res/etc/split_praat_manual_strings_msvc.py`.
- Compile and run tests (with `--run-praat-tests`) and hope nothing broke.
- Commit, push, and hope nothing breaks in CI.
- Update the Praat version in the docs and README (Praat citations).
