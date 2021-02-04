Guide to update the Praat version of the git subtree in `praat/`:
- Look up the latest Praat release, and that commit's hash: https://github.com/praat/praat/releases.
- Fetch the latest commits from the Praat remote: `git fetch praat`.
- Merge and squash the new commits into the git subtree: `git subtree merge --prefix praat/ --squash the_praat_release_commit`.
- Resolve (possibly a lot of) merge conflicts.
- Check for source files added or deleted, and adapt the `CMakeLists.txt` files: `res/etc/makefilelist.sh`.
- Check for changes in Praat's `makefile.defs` files: `git difftool HEAD^ -- praat/makefiles`.
- Check for changes related to global locale-sensitive functions: `res/etc/locale_regex_diff.sh HEAD^`.
- Compile and run tests (with `--run-praat-tests`) and hope nothing broke.
- Commit, push, and hope nothing breaks in CI.
- Update the Praat version in the docs and README (Praat citations).

