# Getting Started with brainmaze_mefd

This guide will help you get up and running with the brainmaze_mefd library in minutes.

## What is MEF 3?

The **Multiscale Electrophysiology Format (MEF)** is a standardized file format for storing electrophysiological recordings. It's designed to handle large volumes of data efficiently while supporting features like:

- Compression (lossless and lossy)
- Encryption with password protection
- Multiple channels
- Time discontinuities
- Data integrity checking (CRC)

## Your First MEF 3 File

### 1. Creating and Writing Data

The simplest way to create a MEF 3 file is with the `MefWriter`:

```python
from brainmaze_mefd import MefWriter
import numpy as np
import time

# Create a new session
writer = MefWriter('my_session.mefd', overwrite=True)

# Generate some sample data (1 minute at 1 kHz)
fs = 1000  # sampling rate in Hz
duration = 60  # seconds
data = np.sin(2 * np.pi * 10 * np.linspace(0, duration, fs * duration))

# Get current time in microseconds (MEF uses UTC microseconds)
start_time = int(time.time() * 1e6)

# Write the data
writer.write_data(data, 'signal_channel', start_time, fs)
writer.write_data(data * 0.5, 'signal_channel_2', start_time, fs)

print("✅ MEF file created: my_session.mefd")
```

### 2. Reading the Data Back

```python
from brainmaze_mefd import MefReader

# Open the session
reader = MefReader('my_session.mefd')

# List available channels
print("Available channels:", reader.channels)
# Output: Available channels: ['signal_channel', 'signal_channel_2']

# Get metadata
fs = reader.get_property('fsamp', 'signal_channel')
unit = reader.get_property('unit', 'signal_channel')
print(f"Sampling rate: {fs} Hz, Units: {unit}")

# Read all data
data = reader.get_data('signal_channel')
print(f"Read {len(data)} samples")

# Read a specific time range (in microseconds)
start = reader.get_property('start_time')[0]
end = start + int(10 * 1e6)  # 10 seconds
segment = reader.get_data('signal_channel', start, end)
print(f"Read 10-second segment: {len(segment)} samples")
```

## Common Workflows

### Converting from EDF Format

Many neuroscience labs use EDF (European Data Format). You can easily convert to MEF 3:

```python
from brainmaze_mefd import MefWriter
import mne
import os

# Read EDF file
edf_path = 'recording.edf'
raw = mne.io.read_raw_edf(edf_path)

# Get basic info
fs = raw.info['sfreq']
start_time = int(raw.annotations.orig_time.timestamp() * 1e6)

# Create MEF writer
output_path = 'recording.mefd'
writer = MefWriter(output_path, overwrite=True)
writer.mef_block_len = int(fs)  # 1 second blocks
writer.max_nans_written = 0

# Convert each channel
for ch_name in raw.ch_names:
    # Get channel-specific unit
    unit = raw._orig_units.get(ch_name, 'V')
    writer.data_units = unit
    
    # Get data and convert to microvolts if needed
    data = raw.get_data(ch_name).squeeze()
    if unit == 'V':
        data *= 1e6  # Convert to microvolts
    
    # Sanitize channel name (remove special characters)
    clean_name = ch_name.replace('/', '_').replace(' ', '_')
    
    # Write to MEF
    writer.write_data(data, clean_name, start_time, fs)

print(f"✅ Converted {edf_path} to {output_path}")
```

### Merging Multiple Recordings

Combine multiple MEF files into one:

```python
from brainmaze_mefd import MefReader, MefWriter
import numpy as np

files = [
    'session_day1.mefd',
    'session_day2.mefd',
    'session_day3.mefd'
]

output = 'session_merged.mefd'
writer = MefWriter(output, overwrite=True)

for file_path in files:
    print(f"Processing {file_path}...")
    reader = MefReader(file_path)
    
    # Get time range
    start = min(reader.get_property('start_time'))
    end = max(reader.get_property('end_time'))
    
    # Copy all channels
    for ch in reader.channels:
        fs = reader.get_property('fsamp', ch)
        unit = reader.get_property('unit', ch)
        data = reader.get_data(ch, start, end)
        
        # Configure writer for this channel
        writer.data_units = unit
        writer.mef_block_len = int(fs)
        
        # Write (append if not first file)
        writer.write_data(data, ch, start, fs)

print(f"✅ Merged {len(files)} files into {output}")
```

### Extracting Time Windows

Extract specific time periods from a recording:

```python
from brainmaze_mefd import MefReader, MefWriter

reader = MefReader('long_recording.mefd')
writer = MefWriter('extracted_segment.mefd', overwrite=True)

# Extract 1 hour starting from the beginning
start_time = reader.get_property('start_time')[0]
duration_seconds = 3600  # 1 hour
end_time = start_time + int(duration_seconds * 1e6)

for ch in reader.channels:
    fs = reader.get_property('fsamp', ch)
    unit = reader.get_property('unit', ch)
    
    # Read the segment
    data = reader.get_data(ch, start_time, end_time)
    
    # Write to new file
    writer.data_units = unit
    writer.mef_block_len = int(fs)
    writer.write_data(data, ch, start_time, fs)

print(f"✅ Extracted {duration_seconds/3600:.1f} hour segment")
```

### Processing Large Files Efficiently

For files larger than available RAM, read in chunks:

```python
from brainmaze_mefd import MefReader
import numpy as np

reader = MefReader('huge_recording.mefd')
channel = 'data_channel'

# Get total duration
start = reader.get_property('start_time')[0]
end = reader.get_property('end_time')[0]
fs = reader.get_property('fsamp', channel)

# Process in 1-minute chunks
chunk_duration = 60  # seconds
chunk_samples = int(fs * chunk_duration)
current_time = start

while current_time < end:
    # Read chunk
    next_time = min(current_time + int(chunk_duration * 1e6), end)
    chunk = reader.get_data(channel, current_time, next_time)
    
    # Process chunk
    mean = np.nanmean(chunk)
    peak = np.nanmax(np.abs(chunk))
    
    print(f"Chunk at {current_time:d}: mean={mean:.2f}, peak={peak:.2f}")
    
    current_time = next_time
```

## Understanding MEF Time Format

MEF uses **microseconds since epoch (uUTC)** for timestamps. The epoch is January 1, 1970, 00:00:00 UTC.

```python
import time
from datetime import datetime

# Convert Python time to MEF time
python_time = time.time()  # seconds since epoch
mef_time = int(python_time * 1e6)  # microseconds since epoch

print(f"Python time: {python_time}")
print(f"MEF time: {mef_time}")

# Convert MEF time back to Python
mef_time = 1700000000000000  # some MEF time
python_time = mef_time / 1e6
dt = datetime.fromtimestamp(python_time)
print(f"Date/time: {dt}")
```

## MEF Writer Options

When creating a `MefWriter`, you can configure several options:

```python
from brainmaze_mefd import MefWriter

writer = MefWriter('session.mefd', overwrite=True)

# Data organization
writer.mef_block_len = 1000  # Samples per block (for compression)
writer.max_nans_written = 100  # Max NaN samples to write

# Metadata
writer.data_units = 'mV'  # Unit of measurement
writer.subject_name = 'Patient_001'
writer.session_description = 'Resting state recording'

# Encryption (optional)
writer.write_password = 'secure_password_1'
writer.read_password = 'secure_password_2'

# File organization
writer.record_offset = 0  # Start offset
writer.section3_dict['GMT_offset'] = -5  # Timezone
```

## MEF Reader Options

When reading MEF files, use `MefReader`:

```python
from brainmaze_mefd import MefReader

# Open with password if encrypted
reader = MefReader('session.mefd', password='password_if_needed')

# Get available properties
print(reader.properties)  # Full properties dict

# Get channel-specific property
fs = reader.get_property('fsamp', 'channel_name')

# List all channels
print(reader.channels)

# Get time range
start = reader.get_property('start_time')[0]
end = reader.get_property('end_time')[0]
```

## Handling Missing Data (NaNs)

MEF supports missing data points. Handle them appropriately:

```python
from brainmaze_mefd import MefReader
import numpy as np

reader = MefReader('session.mefd')
data = reader.get_data('channel')

# Check for missing data
nan_count = np.isnan(data).sum()
print(f"Missing samples: {nan_count} / {len(data)} ({100*nan_count/len(data):.1f}%)")

# Common ways to handle NaNs:

# 1. Replace with mean
data_filled = data.copy()
mean = np.nanmean(data)
data_filled[np.isnan(data_filled)] = mean

# 2. Interpolate
valid_idx = ~np.isnan(data)
data_interp = np.interp(np.arange(len(data)), np.where(valid_idx)[0], data[valid_idx])

# 3. Just remove them
data_clean = data[~np.isnan(data)]
```

## Error Handling

Always wrap file operations in try-except blocks:

```python
from brainmaze_mefd import MefReader, MefWriter

try:
    reader = MefReader('session.mefd', password='wrong_password')
except Exception as e:
    print(f"Error reading file: {e}")

try:
    writer = MefWriter('output.mefd', overwrite=False)
    writer.write_data(data, 'channel', start_time, fs)
except Exception as e:
    print(f"Error writing file: {e}")
```

## Next Steps

- Read the [API Reference](api.md) for detailed function documentation
- Check out [examples/](../examples/) for more complex use cases
- Review the [MEF 3 Specification](MEF_3_Specification.pdf) for format details
- Join our [community discussions](https://github.com/mselair/brainmaze_mefd/discussions)

## Tips & Best Practices

✅ **Do:**
- Use descriptive channel names
- Set proper data units ('mV', 'V', 'µV', etc.)
- Handle missing data appropriately
- Use streaming for large files
- Always close/flush writers when done

❌ **Don't:**
- Use special characters in channel names (use `_` or `-` instead)
- Assume all files are in microvolts (check units!)
- Read entire large files into memory at once
- Mix different sampling rates in one session without care
- Ignore NaN values - they have meaning

## Getting Help

- 📖 Full documentation: https://brainmaze-mefd.readthedocs.io
- 💬 Questions? Use GitHub Discussions
- 🐛 Found a bug? File an issue on GitHub
- 📧 Scientific questions: Contact MSEL Laboratory

