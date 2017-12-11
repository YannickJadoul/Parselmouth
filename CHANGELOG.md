# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

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

[Unreleased]: https://github.com/YannickJadoul/Parselmouth/compare/v0.2.1...HEAD
[0.2.1]: https://github.com/YannickJadoul/Parselmouth/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/YannickJadoul/Parselmouth/compare/v0.1.1...v0.2.0
[0.1.1]: https://github.com/YannickJadoul/Parselmouth/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/YannickJadoul/Parselmouth/compare/e363540...v0.1.0
