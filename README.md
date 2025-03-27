# Your Workbench Library (YWL)

A modern C++23 module-based utility library designed for small to medium-sized projects.

## Overview

YWL provides utility extensions and enhancements to the C++ standard library, with a focus on modern C++ features and idioms. The library is organized into several modules:

- **Core Utilities** (`ywl.essential`)
    - Basic utilities and common functionality
    - Misc helpers and tools (e.g., extension on type traits and useful concepts)
    - General-purpose utilities (e.g., printing, logging, and rust-like enums extensions for std::variant)

- **Standard Library** (`ywl.std`)
    - Module-based standard library implementations provided by libc++, also faked a implementation for libstdc++ based on the libc++ implementation
    - Enhanced standard library features, add some functionalities that are promised to be in the standard library but not yet implemented

- **Concurrency & Coroutines**
    - Coroutine utilities and helpers, mainly include a generator type
    - Concurrent programming support, including a simple thread pool, concurrent queue, and channel

- **Additional Features**
    - Add a tiny virtual machine system which handles uncaught exceptions and registers signal handlers, notice that the virtual machine system might be extended further to support resources management and other features

## Requirements

- C++23 clang with libc++ or libstdc++

## Installation

- Please use CMake fetch content and link the library to your project, the modules should be automatically imported into your project

## License

- This project is licensed under the MIT License - see the [LICENSE](LICENSE)  file for details
## Third Party Notices

- **This project includes code from the LLVM Project**
  - Files under `ywl/std/build_std.compat.cppm` and related compatibility headers
  - Modified files:
    - `ywl/std/build_std.compat.cppm`, `ywl/std/build_std.cppm`: Modified from std.cppm and std.compat.cppm in libc++ source code, to provide a fake implementation for libstdc++ based on the libc++ implementation
    - `libcxx/*`: Not modified
    - `libstdcxx-msvc/*`: Modified for libstdc++ msvcrt support
  - Original files are part of the LLVM Project, licensed under Apache License 2.0 with LLVM Exception
  - See https://llvm.org/LICENSE.txt for license details
  - Modifications Copyright (c) 2025 Yiran Wang under MIT License