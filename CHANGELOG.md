# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.3.3] - 2019-05-19
### Fixed
- Fixed crash in `praat.call` and `praat.run` when returning already existing Praat objects.
- Fixed bug/crash in `praat.call` and `praat.run` when passing empty list of objects.
- Removed `setlocale` calls from Praat and replace locale-dependent standard library calls with locale-independent ones (see #11).
- Fixed sdist build by adding tests folder to `MANIFEST.in` (see #9).

## [0.3.2] - 2018-08-30
### Added
- Exposed `tmin`, `tmax`, `trange`, `nt`, `t1`, `dt`, `ts`, `t_grid`, and `t_bins` for `TimeFunction` and `TimeSampled` aspects.
- Added `PREBUILT_PRAAT_DIR` variable to CMake configuraiton to allow reusing an already built Praat static library.
### Changed
- Refactored bindings framework to allow for forward declarations and to get reduce the importance and use of `parselmouth/Parselmouth.h`.
### Fixed
- Fixed `parselmouth.to_pitch` overload resolution, stopping `*args` and `**kwargs` from matching all calls.

## [0.3.1] - 2018-08-10
### Added
- Added Binder setup description and links from documentation.
- Added test framework with pytest and set up first batch of tests.
- Added Python 3.7 configurations to CI configurations.
### Changed
- Updated pybind11 to v2.2.3.
- Updated documentation, changing examples from simple reStructuredText to Jupyter notebooks, executable through Binder.
### Fixed
- Fixed `parselmouth.praat.run_file` to include files from directory of the script file, but to run in the original working directory.
- Corrected typo in `Sound.to_harmonicity_ac` and `to_harmonicity_cc` (see #6).

## [0.3.0] - 2018-02-09
### Added
- Exposed calls to Praat commands through `parselmouth.praat.call`.
- Exposed running Praat scripts through `parselmouth.praat.run` and `parselmouth.praat.run_script`.
- Added `Thing.class_name` as read-only property to get underlying Praat class, even when that one is not yet exposed.
- Added `name` and `full_name` properties to `Thing` class.
- Added `save`, `save_as_text_file`, `save_as_short_text_file`, and `save_as_binary_file` to `Data` class.
### Changed
- Updated Praat version to 6.0.37.
- Updated pybind11 to development version v2.3.dev0 (for `py::args` bugfix in dispatcher).
### Fixed
- Corrected confusion between `get_time_from_frame_number` and `get_frame_number_from_time` in time-sampled classes.
- Minor fixes to the examples in the docs.

## [0.2.1] - 2017-12-11
### Added
- Set up documentation on ReadTheDocs, including a rough API reference.
### Changed
- Exposed some missing `Intensity` and `Pitch` methods, and `Pitch`'s internals.
### Fixed
- Made Travis CI and AppVeyor install NumPy.

## [0.2.0] - 2017-09-15
### Added
- Added time domain query and modification methods on Sampled classes with a time aspect.
- Exposed classes in the middle of Praat's inheritance hierarchy, `Function`, `Sampled`, and `SampledXY`.
- Exposed main functionality of `Pitch`, `Formants`, and `MFCC`.
- Added manylinux1 wheel building on Travis CI.
### Changed
- 'number of' accessors prefixed changed from `num_` to `n_` (e.g., `Sound.num_channels` ->`Sound.n_channels`).
- Updated Praat version to 6.0.31.
- Updated pybind11 to version v2.2.1.
### Fixed
- Adapted encoding of `Thing.__str__` on Python 2 to take `locale.getpreferredencoding()` into account.

## [0.1.1] - 2017-08-18
### Added
- Compilation of Praat on MSVC.
- This file.
### Changed
- Further extended Travis CI configuration and added one for AppVeyor.
### Fixed
- Corrected README example involving sample times vs. logical range.
- Introduced `NonNegative[T]` type and annotation, fixing `Sound.concatenate(..., overlap=0.0)`.
- Changed use of Python-reserved (keyword) arguments `from` (and matching `to`) to `from_time` and `to_time`.

## [0.1.0] - 2017-06-11
### Added
- Initial release with main pybind11 framework, based on Praat version 6.0.28.
- Main functionality of `Thing`, `Data`, `Matrix`, `Vector`, `Sound`, `Spectrum`, `Spectrogram`, and `Intensity` classes. Preliminary implementations of `Pitch`, `Harmonicity`, `Formant`, and `MFCC`.
- Basic Travis CI configuration.

[Unreleased]: https://github.com/YannickJadoul/Parselmouth/compare/v0.3.3...HEAD
[0.3.3]: https://github.com/YannickJadoul/Parselmouth/compare/v0.3.2...v0.3.3
[0.3.2]: https://github.com/YannickJadoul/Parselmouth/compare/v0.3.1...v0.3.2
[0.3.1]: https://github.com/YannickJadoul/Parselmouth/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/YannickJadoul/Parselmouth/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/YannickJadoul/Parselmouth/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/YannickJadoul/Parselmouth/compare/v0.1.1...v0.2.0
[0.1.1]: https://github.com/YannickJadoul/Parselmouth/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/YannickJadoul/Parselmouth/compare/e363540...v0.1.0
