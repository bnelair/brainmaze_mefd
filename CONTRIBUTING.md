# Contributing to brainmaze_mefd

We welcome contributions from the scientific computing community! This document provides guidelines for contributing to the brainmaze_mefd project.

## Code of Conduct

- Be respectful and inclusive
- Welcome diverse perspectives
- Focus on constructive criticism
- Report inappropriate behavior to maintainers

## Ways to Contribute

### 1. Report Bugs

Found a bug? Please report it:

1. Check [GitHub Issues](https://github.com/mselair/brainmaze_mefd/issues) to see if it's already reported
2. Click "New Issue"
3. Use the bug report template
4. Include:
   - Python version (`python --version`)
   - OS and version
   - Steps to reproduce
   - Expected behavior
   - Actual behavior
   - Error traceback (if applicable)

**Example bug report:**
```
**Describe the bug**
MefReader fails to open encrypted files with special characters in password

**To Reproduce**
1. Create encrypted MEF file with password "test@pwd#123"
2. Try to open with MefReader
3. Observe error

**Expected behavior**
File should open successfully

**Environment:**
- Python 3.9
- macOS 12.0
- brainmaze_mefd 1.0.0

**Traceback:**
[Paste full traceback here]
```

### 2. Suggest Features

Have an idea for improvement?

1. Check [GitHub Discussions](https://github.com/mselair/brainmaze_mefd/discussions)
2. Start a new discussion
3. Describe the feature and use case
4. Discuss implementation approach

### 3. Improve Documentation

Documentation improvements are always welcome:

- Fix typos
- Add clarifications
- Improve examples
- Add new tutorials

See [docs/](../docs/) directory to edit documentation.

### 4. Submit Code

#### Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/mselair/brainmaze_mefd.git
cd brainmaze_mefd

# Create virtual environment
python -m venv dev_env
source dev_env/bin/activate  # macOS/Linux
dev_env\Scripts\activate      # Windows

# Install in development mode with dev dependencies
pip install -e ".[dev]"

# Install pre-commit hooks
pre-commit install
```

#### Development Workflow

1. **Create a branch** for your feature/fix:
   ```bash
   git checkout -b feature/my-feature
   # or
   git checkout -b fix/my-bug
   ```

2. **Make your changes** following code style guidelines

3. **Write tests** for new functionality:
   ```bash
   pytest tests/
   ```

4. **Check code quality**:
   ```bash
   # Type checking
   mypy src/

   # Linting
   flake8 src/

   # Code formatting
   black src/
   ```

5. **Commit with clear messages**:
   ```bash
   git commit -m "fix: resolve memory leak in MefReader

   - Fix buffer not being freed in destructor
   - Add test case for large file handling
   - Update changelog"
   ```

6. **Push and create Pull Request**:
   ```bash
   git push origin feature/my-feature
   ```

#### Code Style Guidelines

We follow these standards:

- **Python**: [PEP 8](https://www.python.org/dev/peps/pep-0008/)
- **C++**: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- **Line length**: 100 characters
- **Indentation**: 4 spaces (Python), 2 spaces (C++)

**Python example:**
```python
def read_channel_data(reader: MefReader, channel_name: str,
                      start_time: int = None,
                      end_time: int = None) -> np.ndarray:
    """Read data from a MEF channel.
    
    Args:
        reader: MefReader instance
        channel_name: Name of channel to read
        start_time: Start time in microseconds (optional)
        end_time: End time in microseconds (optional)
    
    Returns:
        Array of channel data
    
    Raises:
        ValueError: If channel doesn't exist
    """
    if channel_name not in reader.channels:
        raise ValueError(f"Channel '{channel_name}' not found")
    
    return reader.get_data(channel_name, start_time, end_time)
```

**C++ example:**
```cpp
class MefReader {
public:
  // Constructor with detailed documentation
  explicit MefReader(const std::string& file_path,
                     const std::string& password = "");
  
  // Method documentation
  std::vector<double> GetData(const std::string& channel_name,
                              int64_t start_time = -1,
                              int64_t end_time = -1);
};
```

#### Testing Requirements

All new code must include tests:

```python
# tests/test_my_feature.py
import pytest
import numpy as np
from brainmaze_mefd import MefWriter, MefReader

def test_read_channel_data():
    """Test reading channel data."""
    # Arrange
    writer = MefWriter('test.mefd', overwrite=True)
    data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    
    # Act
    writer.write_data(data, 'test_ch', 0, 1000)
    reader = MefReader('test.mefd')
    result = reader.get_data('test_ch')
    
    # Assert
    np.testing.assert_array_almost_equal(result, data)

def test_invalid_channel():
    """Test reading non-existent channel raises error."""
    reader = MefReader('test.mefd')
    
    with pytest.raises(ValueError):
        reader.get_data('nonexistent_channel')
```

Run tests:
```bash
# Run all tests
pytest

# Run with coverage
pytest --cov=brainmaze_mefd

# Run specific test
pytest tests/test_my_feature.py::test_read_channel_data
```

#### Documentation Requirements

All public APIs must have docstrings:

**Python docstring format (Google style):**
```python
def my_function(param1: str, param2: int) -> dict:
    """Short description of what the function does.
    
    Longer description explaining the function in more detail.
    Can span multiple paragraphs.
    
    Args:
        param1: Description of param1
        param2: Description of param2
    
    Returns:
        Description of return value
    
    Raises:
        ValueError: When param1 is empty
        TypeError: When param2 is not int
    
    Example:
        >>> result = my_function('test', 42)
        >>> result['success']
        True
    """
    pass
```

**C++ documentation (Doxygen style):**
```cpp
/// @brief Short description
/// 
/// Longer description explaining the function.
///
/// @param[in] param1 Description of param1
/// @param[in] param2 Description of param2
/// @return Description of return value
/// @throws std::invalid_argument When input is invalid
void my_function(const std::string& param1, int param2);
```

### 5. Performance Improvements

If you're optimizing code:

1. Provide benchmarks showing improvement
2. Include before/after timing data
3. Ensure no regression in other areas
4. Update documentation if algorithm changes

Example benchmark:
```python
import time
import numpy as np

# Benchmark old implementation
data = np.random.randn(1000000)
start = time.time()
result_old = old_implementation(data)
old_time = time.time() - start

# Benchmark new implementation  
start = time.time()
result_new = new_implementation(data)
new_time = time.time() - start

print(f"Old: {old_time:.4f}s")
print(f"New: {new_time:.4f}s")
print(f"Improvement: {old_time/new_time:.2f}x faster")
```

## Pull Request Process

1. **Update documentation** for any new features
2. **Add tests** for all new code
3. **Update CHANGELOG.md** with your changes
4. **Pass all CI checks** (tests, linting, coverage)
5. **Get code review** from maintainers
6. **Address review comments** promptly

### Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement

## Related Issues
Fixes #(issue number)

## Testing
- [ ] Added tests
- [ ] All tests pass
- [ ] Tested on macOS
- [ ] Tested on Linux
- [ ] Tested on Windows

## Checklist
- [ ] Code follows style guidelines
- [ ] Documentation updated
- [ ] No breaking changes
- [ ] Added to CHANGELOG.md
```

## Release Process

### Versioning

We follow [Semantic Versioning](https://semver.org/):
- **MAJOR**: Breaking API changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

### Release Checklist

1. Update version in `__init__.py` and `pyproject.toml`
2. Update `CHANGELOG.md`
3. Create release commit: `git commit -m "Release v1.2.3"`
4. Tag release: `git tag v1.2.3`
5. Push: `git push origin main && git push --tags`
6. Build and upload to PyPI: `python -m build && twine upload dist/*`

## Community

- 💬 [GitHub Discussions](https://github.com/mselair/brainmaze_mefd/discussions)
- 🐛 [Issue Tracker](https://github.com/mselair/brainmaze_mefd/issues)
- 📧 Scientific questions: Contact [MSEL Laboratory](http://msel.mayo.edu/)
- 📖 [Documentation](https://brainmaze-mefd.readthedocs.io)

## Recognition

Contributors will be recognized in:
- [CONTRIBUTORS.md](CONTRIBUTORS.md)
- Release notes
- GitHub repository

## Resources

- [MEF 3 Specification](docs/MEF_3_Specification.pdf)
- [Python Packaging Guide](https://packaging.python.org/)
- [Pytest Documentation](https://docs.pytest.org/)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Questions?

Feel free to ask! Open an issue or start a discussion.

---

**Thank you for contributing to brainmaze_mefd!** 🙏

