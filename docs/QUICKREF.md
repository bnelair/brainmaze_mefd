# Quick Reference Guide

Fast lookup for common brainmaze_mefd tasks.

## Installation

```bash
pip install brainmaze-mefd
```

## Basic Operations

### Create and Write MEF File

```python
from brainmaze_mefd import MefWriter
import numpy as np
import time

writer = MefWriter('session.mefd', overwrite=True)
writer.data_units = 'mV'
writer.mef_block_len = 1000

data = np.random.randn(60000)  # 60 seconds at 1 kHz
start_time = int(time.time() * 1e6)

writer.write_data(data, 'channel_1', start_time, 1000)
```

### Read MEF File

```python
from brainmaze_mefd import MefReader

reader = MefReader('session.mefd')

# List channels
print(reader.channels)

# Read all data
data = reader.get_data('channel_1')

# Read time range (microseconds)
start = reader.get_property('start_time')[0]
end = start + int(60 * 1e6)
segment = reader.get_data('channel_1', start, end)
```

### Get Metadata

```python
reader = MefReader('session.mefd')

fs = reader.get_property('fsamp', 'channel_1')        # Sampling rate
unit = reader.get_property('unit', 'channel_1')        # Data unit
samples = reader.get_property('num_samples', 'channel_1')  # Total samples
start = reader.get_property('start_time', 'channel_1')     # Start time
end = reader.get_property('end_time', 'channel_1')         # End time
```

## Common Workflows

### Merge Files

```python
from brainmaze_mefd import MefReader, MefWriter

writer = MefWriter('merged.mefd', overwrite=True)

for file_path in ['file1.mefd', 'file2.mefd']:
    reader = MefReader(file_path)
    for ch in reader.channels:
        data = reader.get_data(ch)
        fs = reader.get_property('fsamp', ch)
        writer.write_data(data, ch, 0, fs)
```

### Convert EDF to MEF

```python
from brainmaze_mefd import MefWriter
import mne

raw = mne.io.read_raw_edf('file.edf')
writer = MefWriter('output.mefd', overwrite=True)
writer.mef_block_len = int(raw.info['sfreq'])

for ch in raw.ch_names:
    data = raw.get_data(ch).squeeze() * 1e6
    writer.write_data(data, ch, 0, raw.info['sfreq'])
```

### Extract Time Window

```python
reader = MefReader('session.mefd')
writer = MefWriter('segment.mefd', overwrite=True)

start = reader.get_property('start_time')[0]
end = start + int(3600 * 1e6)  # 1 hour

for ch in reader.channels:
    fs = reader.get_property('fsamp', ch)
    data = reader.get_data(ch, start, end)
    writer.write_data(data, ch, start, fs)
```

### Append Data

```python
writer = MefWriter('session.mefd', overwrite=False)

# Add more data to existing file
new_data = np.random.randn(60000)
start_time = int(time.time() * 1e6)
writer.write_data(new_data, 'channel_1', start_time, 1000)
```

### Create Segments (Time Gaps)

```python
writer = MefWriter('session.mefd', overwrite=False)

# Write data in separate segment
gap_time = int(time.time() * 1e6) + int(3600 * 1e6)  # 1 hour gap
data = np.random.randn(60000)
writer.write_data(data, 'channel_1', gap_time, 1000, new_segment=True)
```

## Data Processing Examples

### Handle Missing Data

```python
import numpy as np

data = reader.get_data('channel_1')

# Find missing data
missing_idx = np.isnan(data)
missing_pct = np.sum(missing_idx) / len(data) * 100

# Replace with mean
data[missing_idx] = np.nanmean(data)

# Or interpolate
valid_idx = ~missing_idx
data = np.interp(np.arange(len(data)), np.where(valid_idx)[0], data[valid_idx])
```

### Signal Processing

```python
from scipy import signal

data = reader.get_data('channel_1')
fs = reader.get_property('fsamp', 'channel_1')

# Low-pass filter
sos = signal.butter(4, 100, 'low', fs=fs, output='sos')
filtered = signal.sosfilt(sos, data)

# Downsample
downsampled = signal.decimate(data, 4)
```

### Spectral Analysis

```python
from scipy.signal import welch
import matplotlib.pyplot as plt

data = reader.get_data('channel_1')
fs = reader.get_property('fsamp', 'channel_1')

# Compute power spectral density
f, Pxx = welch(data, fs=fs, nperseg=fs*4)

plt.semilogy(f, Pxx)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Power')
plt.show()
```

## Configuration Options

### Writer Configuration

```python
writer = MefWriter('session.mefd', overwrite=True)

# Data organization
writer.mef_block_len = 1000          # Samples per block
writer.max_nans_written = 100        # Max missing samples

# Metadata
writer.data_units = 'mV'             # Unit of measurement
writer.subject_name = 'Patient_001'  # Subject identifier
writer.session_description = 'Recording notes'

# File organization
writer.record_offset = 0             # Start offset
writer.section3_dict['GMT_offset'] = -5  # Timezone
```

## Error Handling

```python
from brainmaze_mefd import MefReader, MefException

try:
    reader = MefReader('session.mefd')
    data = reader.get_data('channel_1')
except FileNotFoundError:
    print("File not found!")
except ValueError as e:
    print(f"Invalid data: {e}")
except MefException as e:
    print(f"MEF error: {e}")
```

## Performance Tips

1. **For large files**: Use streaming with time ranges
   ```python
   # Good - reads only 1 second
   data = reader.get_data('ch', start, start + int(1e6))
   
   # Bad - reads entire file
   data = reader.get_data('ch')
   ```

2. **For writing**: Set appropriate block length
   ```python
   writer.mef_block_len = int(fs)  # 1 second blocks (balanced)
   ```

3. **For multiple channels**: Process sequentially
   ```python
   for ch in reader.channels:
       data = reader.get_data(ch, start, end)
       # process...
   ```

## Common Mistakes

❌ **Don't:**
- Assume all files are in microvolts
- Read entire files into memory for huge datasets
- Use special characters in channel names
- Ignore NaN values

✅ **Do:**
- Check `get_property('unit')` before processing
- Use time ranges when reading large files
- Use alphanumeric and underscores for channel names
- Handle missing data explicitly

## Time Format

MEF uses **microseconds since epoch (uUTC)**.

```python
import time
from datetime import datetime

# Python time to MEF time
python_time = time.time()  # seconds
mef_time = int(python_time * 1e6)  # microseconds

# MEF time to Python time
mef_time = 1700000000000000
python_time = mef_time / 1e6
dt = datetime.fromtimestamp(python_time)
```

## Useful Properties

| Property | Type | Example |
|----------|------|---------|
| `'fsamp'` | float | 1000.0 (Hz) |
| `'unit'` | str | 'mV' |
| `'start_time'` | int | 1700000000000000 (µs) |
| `'end_time'` | int | 1700003600000000 (µs) |
| `'num_samples'` | int | 3600000 |
| `'compression'` | str | 'none', 'lossless', 'lossy' |
| `'session_name'` | str | 'session_name' |
| `'subject_name'` | str | 'Patient_001' |

## Resources

- 📖 [Full Documentation](https://brainmaze-mefd.readthedocs.io)
- 📚 [API Reference](docs/api.md)
- 🚀 [Getting Started](docs/getting_started.md)
- 🔧 [Installation Guide](docs/installation.md)
- 🐍 [Examples](examples/)

## Getting Help

- 💬 [GitHub Discussions](https://github.com/mselair/brainmaze_mefd/discussions)
- 🐛 [Report Issues](https://github.com/mselair/brainmaze_mefd/issues)
- 📧 Scientific: Contact [MSEL Laboratory](http://msel.mayo.edu/)

---

**Quick Tip:** Save this page as a bookmark for quick reference!

