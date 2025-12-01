# API Reference - brainmaze_mefd

Complete reference for the brainmaze_mefd Python API.

## Table of Contents

- [MefWriter](#mefwriter) - Writing MEF 3 files
- [MefReader](#mefreader) - Reading MEF 3 files
- [Exceptions](#exceptions) - Error handling
- [Constants](#constants) - MEF format constants
- [Utilities](#utilities) - Helper functions

---

## MefWriter

Class for writing MEF 3 files.

### Constructor

```python
MefWriter(path, overwrite=True, password1=None, password2=None)
```

**Parameters:**
- `path` (str): File path for the MEF session (should end with `.mefd`)
- `overwrite` (bool, default=True): If True, overwrite existing file. If False, append data.
- `password1` (str, optional): Write password for encryption
- `password2` (str, optional): Read password for encryption

**Returns:** MefWriter instance

**Example:**
```python
from brainmaze_mefd import MefWriter

# Create new file
writer = MefWriter('session.mefd', overwrite=True)

# Append to existing file
writer = MefWriter('session.mefd', overwrite=False)

# With encryption
writer = MefWriter('session.mefd', overwrite=True, 
                   password1='write_pwd', password2='read_pwd')
```

### Properties

#### mef_block_len

Get or set the MEF block length (samples per block for compression).

```python
writer.mef_block_len = 1000  # 1 second blocks at 1 kHz

# This affects compression ratio and processing speed
# Typical values: 1000-10000
```

**Type:** int  
**Default:** 1000

#### max_nans_written

Maximum number of NaN (missing) samples to write in a segment.

```python
writer.max_nans_written = 100  # Allow up to 100 missing samples

# Use 0 to disallow any missing data
writer.max_nans_written = 0
```

**Type:** int  
**Default:** 0

#### data_units

Data unit of measurement for the channel.

```python
writer.data_units = 'mV'      # Millivolts
writer.data_units = 'µV'      # Microvolts
writer.data_units = 'V'       # Volts
writer.data_units = 'A'       # Amps
```

**Type:** str  
**Default:** 'V'

#### record_offset

Offset for record numbering.

```python
writer.record_offset = 0
```

**Type:** int  
**Default:** 0

#### section3_dict

Dictionary of session-level metadata properties.

```python
writer.section3_dict['GMT_offset'] = -5  # Timezone (hours)
writer.section3_dict['subject_name'] = 'Patient_001'
writer.section3_dict['session_notes'] = 'Resting state'
```

**Type:** dict

### Methods

#### write_data()

Write data to a channel in the MEF file.

```python
write_data(data, channel_name, start_uutc, sampling_freq, 
           precision=None, reload_metadata=True, new_segment=False)
```

**Parameters:**
- `data` (numpy.ndarray): Data to write (1D array of floats)
- `channel_name` (str): Name of the channel
- `start_uutc` (int): Start time in microseconds since epoch
- `sampling_freq` (float): Sampling frequency in Hz
- `precision` (int, optional): Decimal precision (auto-detected if None)
- `reload_metadata` (bool, default=True): Reload metadata after writing
- `new_segment` (bool, default=False): Create new segment (for time discontinuities)

**Returns:** None

**Raises:**
- `ValueError`: If data is invalid or channel name contains invalid characters
- `IOError`: If write fails

**Example:**
```python
import numpy as np
import time

writer = MefWriter('session.mefd', overwrite=True)

# Write 60 seconds of data at 1 kHz
fs = 1000
duration = 60
data = np.random.randn(fs * duration)
start_time = int(time.time() * 1e6)

writer.write_data(data, 'channel_1', start_time, fs)

# Append more data
new_data = np.random.randn(fs * 30)
new_start = start_time + int(60 * 1e6)
writer.write_data(new_data, 'channel_1', new_start, fs)

# Create new segment (time gap)
gap_start = new_start + int(120 * 1e6)  # 2 minutes gap
segment_data = np.random.randn(fs * 30)
writer.write_data(segment_data, 'channel_1', gap_start, fs, new_segment=True)
```

---

## MefReader

Class for reading MEF 3 files.

### Constructor

```python
MefReader(path, password=None)
```

**Parameters:**
- `path` (str): Path to MEF session file
- `password` (str, optional): Password if file is encrypted

**Returns:** MefReader instance

**Raises:**
- `FileNotFoundError`: If file doesn't exist
- `ValueError`: If password is incorrect or missing

**Example:**
```python
from brainmaze_mefd import MefReader

# Open unencrypted file
reader = MefReader('session.mefd')

# Open encrypted file
reader = MefReader('session.mefd', password='my_password')
```

### Properties

#### channels

List of all channel names in the session.

```python
reader = MefReader('session.mefd')
print(reader.channels)
# Output: ['channel_1', 'channel_2', 'channel_3']
```

**Type:** list[str]  
**Read-only:** Yes

#### properties

Dictionary of all session-level properties and metadata.

```python
all_props = reader.properties
print(all_props.keys())

# Access specific properties
print(all_props['session_name'])
```

**Type:** dict  
**Read-only:** Yes

### Methods

#### get_data()

Read data from a channel.

```python
get_data(channel_name, start_time=None, end_time=None)
```

**Parameters:**
- `channel_name` (str): Name of channel to read
- `start_time` (int, optional): Start time in microseconds since epoch. None = beginning
- `end_time` (int, optional): End time in microseconds since epoch. None = end

**Returns:** numpy.ndarray of float64

**Raises:**
- `ValueError`: If channel doesn't exist
- `IOError`: If read fails

**Example:**
```python
reader = MefReader('session.mefd')

# Read entire channel
data = reader.get_data('channel_1')
print(f"Total samples: {len(data)}")

# Read specific time range
start = reader.get_property('start_time')[0]
end = start + int(60 * 1e6)  # 60 seconds
segment = reader.get_data('channel_1', start, end)
print(f"60-second segment: {len(segment)} samples")

# Read last 10 seconds
end_time = reader.get_property('end_time')[0]
start_time = end_time - int(10 * 1e6)
last_segment = reader.get_data('channel_1', start_time, end_time)
```

#### get_property()

Read a property value from a channel or session.

```python
get_property(property_name, channel_name=None)
```

**Parameters:**
- `property_name` (str): Name of property to retrieve
- `channel_name` (str, optional): Channel name. If None, returns session-level property.

**Returns:** Property value (type varies)

**Raises:**
- `ValueError`: If property doesn't exist

**Common Properties:**
- `'fsamp'` - Sampling frequency (Hz)
- `'unit'` - Data unit (e.g., 'mV', 'V')
- `'start_time'` - Start time in microseconds
- `'end_time'` - End time in microseconds
- `'num_samples'` - Total number of samples
- `'session_name'` - Name of the session
- `'subject_name'` - Subject/patient name
- `'recording_date'` - Recording start date

**Example:**
```python
reader = MefReader('session.mefd')

# Channel properties
fs = reader.get_property('fsamp', 'channel_1')
unit = reader.get_property('unit', 'channel_1')
num_samples = reader.get_property('num_samples', 'channel_1')

print(f"Sampling rate: {fs} Hz")
print(f"Unit: {unit}")
print(f"Total samples: {num_samples}")
print(f"Duration: {num_samples/fs/3600:.2f} hours")

# Session properties
session_name = reader.get_property('session_name')
subject_name = reader.get_property('subject_name')

print(f"Session: {session_name}")
print(f"Subject: {subject_name}")
```

---

## Exceptions

### MefException

Base exception for all MEF-related errors.

```python
from brainmaze_mefd import MefException

try:
    reader = MefReader('invalid_file.mefd')
except MefException as e:
    print(f"MEF error: {e}")
```

### InvalidPasswordError

Raised when incorrect password is provided.

```python
from brainmaze_mefd import InvalidPasswordError

try:
    reader = MefReader('encrypted_file.mefd', password='wrong_password')
except InvalidPasswordError:
    print("Incorrect password!")
```

### InvalidFileError

Raised when file is not a valid MEF 3 file.

```python
from brainmaze_mefd import InvalidFileError

try:
    reader = MefReader('not_a_mef_file.txt')
except InvalidFileError:
    print("Not a valid MEF file!")
```

---

## Constants

MEF 3 format constants and enumerations.

### Compression Types

```python
from brainmaze_mefd import CompressionType

CompressionType.NONE          # No compression
CompressionType.LOSSLESS      # Lossless compression
CompressionType.LOSSY         # Lossy compression (variable rate)
```

### Data Types

```python
from brainmaze_mefd import DataType

DataType.INT8        # Signed 8-bit integer
DataType.INT16       # Signed 16-bit integer
DataType.INT32       # Signed 32-bit integer
DataType.INT64       # Signed 64-bit integer
DataType.FLOAT32     # 32-bit floating point
DataType.FLOAT64     # 64-bit floating point
```

### Record Types

```python
from brainmaze_mefd import RecordType

RecordType.SESSION      # Session record
RecordType.CHANNEL      # Channel record
RecordType.SEGMENT      # Segment record
RecordType.ANNOTATION   # Annotation record
```

---

## Utilities

### create_pink_noise()

Generate pink noise for testing.

```python
from brainmaze_mefd import create_pink_noise

# Generate 60 seconds of pink noise at 1 kHz
data = create_pink_noise(fs=1000, duration=60, low_bound=-10, up_bound=10)
```

**Parameters:**
- `fs` (int): Sampling frequency in Hz
- `duration` (float): Duration in seconds
- `low_bound` (float): Lower bound for values
- `up_bound` (float): Upper bound for values

**Returns:** numpy.ndarray of pink noise

### get_mef_version()

Get the MEF library version.

```python
from brainmaze_mefd import get_mef_version

version = get_mef_version()
print(f"MEF version: {version}")
```

**Returns:** Version string

### validate_mef_file()

Validate if a file is a valid MEF 3 file.

```python
from brainmaze_mefd import validate_mef_file

is_valid = validate_mef_file('session.mefd')
if is_valid:
    print("Valid MEF file!")
else:
    print("Invalid MEF file!")
```

**Parameters:**
- `path` (str): Path to file

**Returns:** bool

---

## Complete Example

Here's a complete example using the API:

```python
import numpy as np
import time
from brainmaze_mefd import MefWriter, MefReader

# === WRITING ===
print("Creating MEF file...")
writer = MefWriter('example_session.mefd', overwrite=True)
writer.data_units = 'mV'
writer.mef_block_len = 1000
writer.max_nans_written = 0

# Generate test data
fs = 1000  # 1 kHz
duration = 120  # 2 minutes
start_time = int(time.time() * 1e6)

# Channel 1: Sine wave
t = np.linspace(0, duration, fs * duration)
data1 = np.sin(2 * np.pi * 10 * t)  # 10 Hz sine

# Channel 2: Random noise
data2 = np.random.randn(fs * duration) * 0.5

writer.write_data(data1, 'sine_wave', start_time, fs)
writer.write_data(data2, 'noise', start_time, fs)

print(f"✅ Wrote {duration} seconds of data to example_session.mefd")

# === READING ===
print("\nReading MEF file...")
reader = MefReader('example_session.mefd')

# List channels
print(f"Channels: {reader.channels}")

# Read properties
for ch in reader.channels:
    fs = reader.get_property('fsamp', ch)
    unit = reader.get_property('unit', ch)
    samples = reader.get_property('num_samples', ch)
    duration = samples / fs / 60  # minutes
    print(f"  {ch}: {fs} Hz, {unit}, {samples} samples ({duration:.2f} min)")

# Read data
sine_data = reader.get_data('sine_wave')
noise_data = reader.get_data('noise')

print(f"\nSine wave stats:")
print(f"  Mean: {np.mean(sine_data):.4f}")
print(f"  Std: {np.std(sine_data):.4f}")
print(f"  Min: {np.min(sine_data):.4f}")
print(f"  Max: {np.max(sine_data):.4f}")

# Read specific segment
seg_start = start_time
seg_end = start_time + int(60 * 1e6)  # First 60 seconds
segment = reader.get_data('sine_wave', seg_start, seg_end)
print(f"\nFirst 60 seconds: {len(segment)} samples")

print("\n✅ All operations completed successfully!")
```

---

## See Also

- [Getting Started Guide](getting_started.md)
- [Installation Guide](installation.md)
- [MEF 3 Specification](MEF_3_Specification.pdf)

