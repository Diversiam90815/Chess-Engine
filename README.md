# Chess Engine

[![Windows Build](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/windows.yml/badge.svg)](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/windows.yml)
[![macOS Build](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/macos.yml/badge.svg)](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/macos.yml)
[![Linux Build](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/linux.yml/badge.svg)](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/linux.yml)
[![Tests](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/tests.yml/badge.svg)](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/tests.yml)
[![Static Analysis](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/Diversiam90815/Chess-Engine/actions/workflows/static-analysis.yml)

## Overview

A bitboard-based chess engine written in C++20 with a CPU opponent and LAN multiplayer. The board state is encoded entirely in 64-bit integers for fast move generation. The core is pure C++ and knows nothing about its hosts: it publishes an ordered stream of engine events that a host drains on its own thread. Two hosts ship in this repo: a plain C API (DLL) for non-C++ runtimes, and a console app you can play in the terminal. All built through a unified Python-driven CMake build system.


## Features

- **Bitboard Representation**: The board state is encoded entirely in 64-bit integers. Pre-computed attack tables and Zobrist hashing power efficient move generation, legal-move validation, and transposition-table lookups.
- **CPU Opponent**: Single-player mode against the AI at multiple difficulty levels. Transposition tables are used to avoid redundant evaluation.
- **Adjustable Sides**: Choose whether to play as White or Black when starting a game against the CPU.
- **LAN Multiplayer**: Both players discover each other automatically on the local network. On machines with multiple network interfaces, a specific adapter can be selected for a stable connection.
- **Plain C API (DLL)**: The entire engine is accessible through a plain C API exported as a DLL, making it straightforward to drive from C#, Python, or any runtime with C FFI support.
- **Console App**: Play a full game in the terminal (local co-op or against the CPU) with legal-move listing, undo, move history and board flipping.

## Architecture

The engine is a self-contained brain that doesn't know or care who's asking it to play chess. It has no idea whether it's talking to a Windows app, a terminal, or a test — it just plays the game and reports what happened.

```
              ┌───────────────────┐
              │   Chess Engine    │   plays the game, knows the rules,
              │   (the "brain")   │   reports every move and state change
              └─────────┬─────────┘
                        │
              one stream of events
                        │
        ┌───────────────┴───────────────┐
        │                               │
┌───────▼────────┐             ┌────────▼─────────┐
│  C API (DLL)   │             │  Console App     │
│  used by the   │             │  play a game     │
│  WinUI3 app    │             │  in the terminal │
└────────────────┘             └──────────────────┘
```

Two ideas make this work:

- **One event stream, many listeners.** Whenever something happens — a move is played, a piece is captured, the game ends — the engine writes it to a single ordered list. Each host reads that list at its own pace: the console app reads it between prompts so it never interrupts you mid-command, and the Windows app's DLL reads it and forwards each event to the C# side. Neither host waits on the other, and neither needs to know how the other works.
- **Swappable front ends.** Because the engine doesn't depend on any particular host, new ways to play (a web UI, a different CLI, an AI-vs-AI runner) can be added without touching the engine itself — they just read the same event stream and ask the engine to do things ("play this move", "what are the legal moves here?").

The [Perft app](src/apps/perft) is a third, developer-only front end used for performance testing rather than play.

## Technology Stack

| Category | Technology |
|---|---|
| Language | C++20 |
| Networking | [NetLink v0.2.0](https://github.com/Diversiam90815/NetLink) |
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
│   └── apps/
│       ├── common/     # Board and move rendering shared by the executables
│       ├── console/    # Play a game in the terminal
│       └── perft/      # Perft application for measuring performance
├── tests/
│   └── Core.Tests/     # GoogleTest unit tests
├── build/              # Generated build artifacts (not committed)
├── install/            # Installed headers and libraries (not committed)
└── build.py            # Build entry point
```

## Prerequisites

Chess Engine builds on **Windows, Linux, and macOS**. Providing a default local player name and an log file path is entirely the host application's responsibility (it fills in `EngineSettings`, or `Init()`/`SetLocalPlayerName()` in the C API); the engine itself doesn't look either up.

- **C++ Compiler**: C++20 or higher (MSVC on Windows, GCC or Clang on Linux/macOS)
- **CMake**: Version 4.0 or higher
- **Git**: Required — the build system derives the build number from commit history
- **Python**: 3.x (for `build.py`)
- **Linux only**: `libnl-genl-3-dev` / `libnl-3-dev` (NetLink's Linux WiFi backend dependency)

### Optional Developer Tools

The following tools are CMake-integrated and entirely optional. Each is controlled by a CMake option (all default to `ON` except `ENABLE_MEMCHECK`); builds do not fail if a tool is absent and its option is explicitly set to `OFF`.

| Tool | CMake Option | Purpose | Notes |
|---|---|---|---|
| Doxygen | `ENABLE_DOXYGEN` | HTML documentation | [doxygen.nl](https://www.doxygen.nl/download.html) + [Graphviz](https://graphviz.org/download/) for diagrams |
| CppCheck | `ENABLE_CPPCHECK` | Static analysis | Available via package managers on all platforms (e.g. `winget install cppcheck`, `apt install cppcheck`, `brew install cppcheck`) |
| clang-format | `ENABLE_FORMAT` | Source formatting | Available via package managers on all platforms |
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
| Console application | `build/<arch>/src/apps/console/` |
| Perft application | `build/<arch>/src/apps/perft/` |
| Test executable | `build/<arch>/tests/` |
| Installed headers / libs | `install/` |
| Doxygen HTML docs | `build/doxygen/html/index.html` |

### Playing in the Console

```bash
./build/x64/src/apps/console/Debug/Chess.Engine.Console
```

The app asks for a mode first — local co-op, or against the CPU (your colour and a difficulty). Then it draws the board each turn and takes commands:

| Command | Effect |
|---|---|
| `e2e4`, `e2 e4` | Play a move |
| `e7e8q` | Play a move, promoting to `q`, `r`, `b` or `n` (omit it and you'll be asked) |
| `moves` | List every legal move in the position |
| `moves e2` | List the moves leaving one square, highlighted on the board |
| `board` | Re-draw the board |
| `undo` | Take back the last move (both plies when playing the CPU) |
| `history` | Show the moves played so far |
| `flip` | Switch the board orientation |
| `new` | Start a new game |
| `help` / `quit` | Show the command list / leave |

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
| Logger | 2.0.86 | Logging infrastructure |
| NetLink | 0.2.0 | Peer discovery, TCP/UDP networking (bundles ASIO and nlohmann_json) |
| GoogleTest | 1.15.2 | Unit testing |
| doxygen-awesome-css | 2.3.1 | Documentation theme |

## Future Plans

- Refine positional evaluation and move scoring across CPU difficulty levels

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
