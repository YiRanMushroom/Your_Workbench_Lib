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

## Key Features

- Full C++20 modules support
- Modern C++ design patterns
- STL-style implementation
- Easy-to-use printing facilities
- Lightweight and modular
- Focuses on small project usage

## Requirements

- C++23 clang with libc++ or libstdc++

## Installation

- Please use CMake fetch content and link the library to your project, the modules should be automatically imported into your project