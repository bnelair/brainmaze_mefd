# Installation Guide for brainmaze_mefd

This guide covers installation of brainmaze_mefd on various platforms.

## System Requirements

- **Python**: 3.10 or higher
- **OS**: Linux, macOS, or Windows
- **Architecture**: x86_64 or ARM64 (Apple Silicon)
- **RAM**: Minimum 2GB (more recommended for large files)
- **Disk Space**: ~100MB for the library

## Quick Installation (Recommended)

The easiest way to install is via PyPI:

```bash
pip install brainmaze-mefd
```

This will automatically install:
- The compiled C++ library for your platform
- Python bindings
- All required dependencies

## Platform-Specific Installation

### macOS

#### Using pip (simplest)

```bash
# For Intel Macs
pip install brainmaze-mefd

# For Apple Silicon (M1/M2/M3)
pip install brainmaze-mefd
```

If you have issues with Apple Silicon, ensure you have the development tools:

```bash
xcode-select --install
```

#### Using Homebrew (alternative)

```bash
brew tap mselair/brainmaze_mefd
brew install brainmaze-mefd
```

### Linux

#### Debian/Ubuntu

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install python3-pip python3-dev build-essential

# Install brainmaze_mefd
pip install brainmaze-mefd
```

#### Red Hat/CentOS/Fedora

```bash
# Install dependencies
sudo yum groupinstall "Development Tools"
sudo yum install python3-devel

# Install brainmaze_mefd
pip install brainmaze-mefd
```

#### Arch Linux

```bash
sudo pacman -S base-devel python
pip install brainmaze-mefd
```

### Windows

#### Using pip (simplest)

```bash
pip install brainmaze-mefd
```

#### Using Anaconda (recommended for Windows)

```bash
conda install -c mselair brainmaze-mefd
```

If you encounter issues:
1. Ensure you have [Visual C++ Build Tools](https://visualstudio.microsoft.com/downloads/)
2. Or install through [Microsoft Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

## Installing from Source

For development or if pre-built wheels aren't available for your platform:

```bash
# Clone the repository
git clone https://github.com/mselair/brainmaze_mefd.git
cd brainmaze_mefd

# Install in development mode
pip install -e .

# Or install normally
pip install .
```

### Building from Source Requirements

- **CMake**: 3.15 or higher
- **C++ Compiler**: C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- **Python Development Headers**: `python3-dev` package

#### Build from Source on macOS

```bash
# Install cmake if not present
brew install cmake

# Clone and install
git clone https://github.com/mselair/brainmaze_mefd.git
cd brainmaze_mefd
pip install -e .
```

#### Build from Source on Linux

```bash
# For Ubuntu/Debian
sudo apt-get install cmake python3-dev build-essential

# For CentOS/RHEL
sudo yum install cmake python3-devel

# Then build
git clone https://github.com/mselair/brainmaze_mefd.git
cd brainmaze_mefd
pip install -e .
```

#### Build from Source on Windows

```bash
# Using Visual Studio Code with Python extension:
# 1. Open the repository folder
# 2. Install Python and pip
# 3. Run: pip install -e .

# Or from command prompt:
git clone https://github.com/mselair/brainmaze_mefd.git
cd brainmaze_mefd
pip install -e .
```

## Verifying Installation

After installation, verify everything works:

```python
# Test 1: Import the library
from brainmaze_mefd import MefReader, MefWriter
print("✅ Library imported successfully")

# Test 2: Create a simple MEF file
import numpy as np
import time

writer = MefWriter('test.mefd', overwrite=True)
data = np.random.randn(1000)
writer.write_data(data, 'test_channel', int(time.time() * 1e6), 1000)
print("✅ MEF file created successfully")

# Test 3: Read it back
reader = MefReader('test.mefd')
read_data = reader.get_data('test_channel')
print(f"✅ Read {len(read_data)} samples from MEF file")
```

If all tests pass, you're ready to go!



Install all optional dependencies:

```bash
pip install brainmaze-mefd[full]
```

## Updating to Latest Version

```bash
# Upgrade to latest version
pip install --upgrade brainmaze-mefd

# Downgrade to specific version
pip install brainmaze-mefd==1.0.0

# Install development version
pip install git+https://github.com/mselair/brainmaze_mefd.git
```

## Troubleshooting

### Issue: `ImportError: cannot import name 'MefReader'`

**Solution**: Ensure package is installed:
```bash
pip install --force-reinstall brainmaze-mefd
python -c "from brainmaze_mefd import MefReader; print('OK')"
```

### Issue: `ModuleNotFoundError: No module named 'brainmaze_mefd'`

**Solution**: Install to correct Python environment:
```bash
# Check Python location
which python

# Install to correct Python
/path/to/python -m pip install brainmaze-mefd
```

### Issue: Compilation errors on macOS

**Solution**: Update Xcode command line tools:
```bash
xcode-select --install
xcode-select --reset
pip install --force-reinstall brainmaze-mefd
```

### Issue: Compilation errors on Linux

**Solution**: Install development packages:
```bash
# Ubuntu/Debian
sudo apt-get install build-essential python3-dev

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install python3-devel

# Then reinstall
pip install --force-reinstall brainmaze-mefd
```

### Issue: Windows installation hangs

**Solution**: Install Visual C++ Build Tools:
1. Download from: https://visualstudio.microsoft.com/downloads/
2. Choose "Visual Studio Build Tools"
3. Install "Desktop development with C++"
4. Restart and retry installation

### Issue: `Permission denied` error

**Solution**: Install for current user only:
```bash
pip install --user brainmaze-mefd
```

Or use virtual environment:
```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install brainmaze-mefd
```

### Issue: Out of memory during installation

**Solution**: Disable binary cache:
```bash
pip install --no-cache-dir brainmaze-mefd
```

## Virtual Environments (Recommended)

Using a virtual environment prevents conflicts:

### Using venv

```bash
# Create virtual environment
python -m venv myenv

# Activate it
source myenv/bin/activate  # macOS/Linux
myenv\Scripts\activate      # Windows

# Install package
pip install brainmaze-mefd
```

### Using conda

```bash
# Create environment
conda create -n mefd_env python=3.9

# Activate it
conda activate mefd_env

# Install package
pip install brainmaze-mefd
```

## Docker Installation

For reproducible environments:

```dockerfile
FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    python3-dev

RUN pip install brainmaze-mefd

WORKDIR /app
```

Build and run:

```bash
docker build -t brainmaze_mefd .
docker run -it brainmaze_mefd python
```

## Getting Help

- **Installation issues?** Check [GitHub Issues](https://github.com/mselair/brainmaze_mefd/issues)
- **Questions?** Visit [GitHub Discussions](https://github.com/mselair/brainmaze_mefd/discussions)
- **Documentation**: https://brainmaze-mefd.readthedocs.io
- **Scientific support**: Contact [MSEL Laboratory](http://msel.mayo.edu/)

## Uninstalling

To remove brainmaze_mefd:

```bash
pip uninstall brainmaze-mefd
```

To clean up associated files:

```bash
# Clear cache
pip cache purge

# Remove any test files
rm -rf ~/.brainmaze_mefd/
```

