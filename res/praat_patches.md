List (possibly incomplete) of patches made in Praat within Parselmouth:

- `melder/melder.h`:
  - MSVC compatibility: Define `off_t`, `ftello`, and `fseeko` to be 64-bit variants, to match `_FILE_OFFSET_BITS=64` in Makefiles.
- `melder/melder_casual.h`:
  - Disabled `Melder_casual` output, since it only does debug output and isn't easily redirectable to `sys.stdout`.
- `external/espeak/`:
  - MSVC compatibility: Avoid including `<unistd.h>` and `<strings.h>`, and define `strcasecmp` and `S_ISDIR` where necessary.
  - See also: https://github.com/espeak-ng/espeak-ng/tree/master/src/include/compat

To get a diff of all changes: `git diff ":/Squashed 'praat/' changes" HEAD:praat/`
