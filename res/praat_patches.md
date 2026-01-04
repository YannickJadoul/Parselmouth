List (possibly incomplete) of patches made in Praat within Parselmouth:

- `external/espeak/`:
  - MSVC compatibility: Avoid including `<unistd.h>` and `<strings.h>`, and define `strcasecmp` and `S_ISDIR` where necessary.
  - See also: https://github.com/espeak-ng/espeak-ng/tree/master/src/include/compat
