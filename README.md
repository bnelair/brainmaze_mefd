# brainmaze_mefd

Modern C++ implementation of brainmaze_mefd, rebuilt from legacy code.

## Project Structure

```
brainmaze_mefd/
├── CMakeLists.txt          # CMake build configuration
├── include/                # Public header files
│   └── brainmaze_mefd.hpp
├── src/                    # Source files
│   ├── main.cpp
│   └── brainmaze_mefd.cpp
├── tests/                  # C++ unit tests (Google Test)
│   ├── test_main.cpp
│   └── test_brainmaze_mefd.cpp
└── legacy/                 # Legacy code reference
    ├── c/                  # Original C code
    │   └── README.md
    └── python/             # Original Python code
        └── README.md
```

## Building

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest --output-on-failure
```

## Usage

After building, run the main executable:

```bash
./brainmaze_mefd
```

## Testing

Tests are built by default using Google Test framework. To disable tests:

```bash
cmake -DBUILD_TESTS=OFF ..
```

## Legacy Code Reference

The `legacy/` directory contains the original C and Python code for reference:

- `legacy/c/` - Original C implementation
- `legacy/python/` - Original Python implementation

These files are for reference only and are not compiled as part of the new project.

## License

[Add license information here]