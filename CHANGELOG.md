# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]
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

[Unreleased]: https://github.com/YannickJadoul/Parselmouth/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/YannickJadoul/Parselmouth/compare/e363540...v0.1.0
