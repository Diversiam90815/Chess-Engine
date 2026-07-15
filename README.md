# Chess Engine

## Overview

A bitboard-based chess engine written in C++20 with a CPU opponent and LAN multiplayer. The board state is encoded entirely in 64-bit integers for fast move generation, and the core is exposed as a plain C API (DLL) for straightforward integration with non-C++ runtimes. The project is structured as a core static library, a C API wrapper, and a console application for testing. All built through a unified Python-driven CMake build system.

## Features

- **Bitboard Representation**: The board state is encoded entirely in 64-bit integers. Pre-computed attack tables and Zobrist hashing power efficient move generation, legal-move validation, and transposition-table lookups.
- **CPU Opponent**: Single-player mode against the AI at multiple difficulty levels. Transposition tables are used to avoid redundant evaluation.
- **Adjustable Sides**: Choose whether to play as White or Black when starting a game against the CPU.
- **LAN Multiplayer**: Both players discover each other automatically on the local network. On machines with multiple network interfaces, a specific adapter can be selected for a stable connection.
- **Plain C API (DLL)**: The entire engine is accessible through a plain C API exported as a DLL, making it straightforward to drive from C#, Python, or any runtime with C FFI support.

## Technology Stack

| Category | Technology |
|---|---|
| Language | C++20 |
| Networking | [NetLink v0.1.0](https://github.com/Diversiam90815/NetLink) |
| Build System | CMake 4.0+, Python 3.x |
| Dependency Management | [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) |
| Testing | GoogleTest 1.15.2 + CTest |
| Documentation | Doxygen + doxygen-awesome-css |
| Static Analysis | CppCheck |
| Formatting | clang-format |

## Project Structure

```
Chess-Engine/
├── cmake/              # CMake modules (formatting, docs, testing, versioning, etc.)
├── scripts/            # Python build automation modules
├── src/
│   ├── core/           # Core engine — game logic, move generation, AI (static library)
│   ├── capi/           # Plain C wrapper around the core (shared library / DLL)
│   └── console_app/    # Console application for manual testing
├── tests/
│   └── Core.Tests/     # GoogleTest unit tests
├── build/              # Generated build artifacts (not committed)
├── install/            # Installed headers and libraries (not committed)
└── build.py            # Build entry point
```

## Prerequisites

- **C++ Compiler**: C++20 or higher (MSVC recommended on Windows)
- **CMake**: Version 4.0 or higher
- **Git**: Required — the build system derives the build number from commit history
- **Python**: 3.x (for `build.py`)

### Optional Developer Tools

The following tools are CMake-integrated and entirely optional. Each is controlled by a CMake option (all default to `ON` except `ENABLE_MEMCHECK`); builds do not fail if a tool is absent and its option is explicitly set to `OFF`.

| Tool | CMake Option | Purpose | Install (Windows) |
|---|---|---|---|
| Doxygen | `ENABLE_DOXYGEN` | HTML documentation | [doxygen.nl](https://www.doxygen.nl/download.html) + [Graphviz](https://graphviz.org/download/) for diagrams |
| CppCheck | `ENABLE_CPPCHECK` | Static analysis | `winget install cppcheck` |
| clang-format | `ENABLE_FORMAT` | Source formatting | `winget install llvm` |
| Valgrind | `ENABLE_MEMCHECK` | Memory checking (off by default) | Linux only |

To disable a tool, set its option in the root `CMakeLists.txt` before configuring:

```cmake
set(ENABLE_CPPCHECK OFF)
```

## Getting Started

### Cloning the Repository

```bash
git clone git@github.com:Diversiam90815/Chess-Engine.git
cd Chess-Engine
```

### Building

All builds go through `build.py`, which handles CMake configuration, compilation, and testing in a single command.

**Release build:**

```bash
python build.py -b
```

**Debug build:**

```bash
python build.py -bd
```

### `build.py` Flag Reference

| Flag | Long form | Description |
|---|---|---|
| `-p` | `--prepare` | Run CMake configure only (no compile) |
| `-b` | `--build` | Configure and compile |
| `-d` | `--debug` | Use Debug configuration (default: Release) |
| `-c CONFIG` | `--configuration` | Explicit config: `Debug`, `Release`, or `RelWithDebInfo` |
| `-a ARCH` | `--architecture` | Target architecture: `x64` (default) or `ARM64` |
| `-pl PLATFORM` | `--platform` | CMake generator: `Ninja`, `VS2022`, or `VS2026` |
| `-t` | `--runtest` | Run CTest after building |
| `--docs` | | Generate Doxygen documentation and open in the browser |

Examples:

```bash
# Build then run tests
python build.py -bt

# Debug build targeting ARM64 with a Visual Studio 2022 solution
python build.py -b -d -a ARM64 -pl VS2022

# Generate HTML documentation
python build.py --docs
```

### Build Outputs

| Artifact | Location |
|---|---|
| Core static library | `build/<arch>/src/core/` |
| C API DLL | `build/<arch>/src/capi/` |
| Console application | `build/<arch>/src/console_app/` |
| Test executable | `build/<arch>/tests/` |
| Installed headers / libs | `install/` |
| Doxygen HTML docs | `build/doxygen/html/index.html` |

### Running Tests

```bash
python build.py -bt    # build then run tests
python build.py -t     # run tests against an existing build
```

Tests are discovered automatically via `gtest_discover_tests()` and run through CTest with `--output-on-failure`.

## Build System Architecture

The build system is split into two layers: a **Python layer** that provides a consistent developer interface, and a **CMake layer** that performs the actual configuration and compilation.

### Python Layer (`scripts/`)

Rather than a single monolithic script, the build automation is broken into focused modules:

| Module | Responsibility |
|---|---|
| `build_runner.py` | Invokes CMake configure and build commands |
| `argument_parser.py` | CLI argument parsing and validation |
| `enums.py` | Typed enums for architecture, configuration, and platform |
| `paths.py` | Centralised path constants (root, build, install directories) |
| `versioning.py` | Reads `git rev-list` commit count and updates `PROJECT_VERSION` before each configure |
| `env_config.py` | Detects environment (Development / Staging / Production) from the active git branch |
| `utils.py` | Subprocess execution helpers and a `working_directory` context manager |

### CMake Layer (`cmake/`)

| Module | Responsibility |
|---|---|
| `cpm.cmake` | CPM package manager : downloads all dependencies at configure time |
| `BuildInfo.cmake` | Injects git metadata (commit SHA, branch, timestamp) into `buildinfo.h` at build time |
| `Format.cmake` | Runs clang-format as a pre-build step across all C++ sources |
| `CppCheck.cmake` | Attaches cppcheck as a `CXX_CPPCHECK` property on each target |
| `Doxygen.cmake` | Configures documentation generation with the doxygen-awesome-css theme |
| `Testing.cmake` | Integrates GoogleTest and CTest, with optional memory checking |
| `Memcheck.cmake` | Valgrind-based memory analysis (disabled by default) |

### Version Management

Project versions follow the pattern `MAJOR.MINOR.PATCH.BUILD`. The build number is derived automatically from the total git commit count (`git rev-list HEAD --count`) and written back to `CMakeLists.txt` before each configure step. The same metadata (commit SHA, branch name, and build timestamp) is compiled directly into the binary via a generated `buildinfo.h`, making every binary traceable to an exact revision.

### External Dependencies

All dependencies are managed by CPM.cmake and downloaded automatically at configure time; no manual installation is required beyond the tools listed in [Prerequisites](#prerequisites).

| Dependency | Version | Purpose |
|---|---|---|
| Logger | 1.3.81 | Logging infrastructure |
| NetLink | 0.1.0 | Peer discovery, TCP/UDP networking (bundles ASIO and nlohmann_json) |
| GoogleTest | 1.15.2 | Unit testing |
| doxygen-awesome-css | 2.3.1 | Documentation theme |

## Future Plans

- Refine positional evaluation and move scoring across CPU difficulty levels

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
