# Change Log

## Unreleased

### Added

- Manual page for documentation
- map<string, Foo> type for dictionaries
- [GCC,Clang] use __builtin_expect for performance

### Fixed

- Some unhandled I/O errors on code generator
- Improved error messages on parser
- Improved attribute support

### Changed

- Docker is used for development environment now
- Benchmark data is now stored in a submodule. This will allow having large
  test data without making `master` branch larger.

## [0.2.0] - 2016-10-30

### Added

- JSON serialization support

## [0.1.0] - 2016-10-22

Initial Varsion

### Added

- JSON Schema to C++ transpiler
- JSON parsing support
- Benchmarks against some other C++ JSON parsers


[0.1.0]: https://github.com/mserdarsanli/QuantumJson/releases/tag/v0.1.0
[0.2.0]: https://github.com/mserdarsanli/QuantumJson/releases/tag/v0.2.0
