List (possibly incomplete) of patches made in Praat within Parselmouth:

- `melder/melder.h`:
  - MSVC compatibility: Define `off_t`, `ftello`, and `fseeko` to be 64-bit variants, to match `_FILE_OFFSET_BITS=64` in Makefiles.
- `melder/melder_casual.h`:
  - Disabled `Melder_casual` output, since it only does debug output and isn't easily redirectable to `sys.stdout`.
- `melder/melder_error.cpp` & `melder/melder_error.h`:
  - Added `Melder_clearCrash()` and changed `Melder_hasCrash()` and `crashMessage()` to allow translating `Melder_crash` to a Python exception.
- `sys/praat.cpp` & `sys/praat.h`
  - Macroed out most of `praat_run()` and retained only the asserts as `praat_testPlatformAssumptions()`, called from Parselmouth's `initializePraat()`.
  - Added ownership tracking of Praat objects in the object list in `structPraat_Object`, so that Praat objects inside Python objects can be borrowed during `parselmouth.praat.call/run/run_file`, adapted functions creating/deleting objects, and added `praat_newReference`.
  - MSVC compatibility: Macroed out certain asserts in `praat_testPlatformAssumptions()` since MSVC doesn't like it when we divide by zero or overflow in literal expressions.
- `external/espeak/`:
  - MSVC compatibility: Avoid including `<unistd.h>` and `<strings.h>`, and define `strcasecmp` and `S_ISDIR` where necessary. See also: https://github.com/espeak-ng/espeak-ng/tree/master/src/include/compat

To get a diff of all changes: `git diff ":/Squashed 'praat/' changes" HEAD:praat/`
