# Chrono-ID Python Implementation

A Python library for generating K-Sortable 32-bit and 64-bit identifiers.

## ✨ Features

- **Integer Inheritance**: Every ID class inherits from Python's `int`, allowing them to be used in any mathematical context.
- **Bit-Parity**: 100% bit-parity with JS and C++ implementations.
- **Hyphenated Hex Standard**: IDs automatically format as `XXXX-XXXX` (32-bit) or `XXXX-XXXX-XXXX-XXXX` (64-bit) for high human readability.
- **Bidirectional Parsing**: ID classes can parse their hyphenated string representation directly via the constructor or `from_format()`.
- **Time Decoding**: All IDs expose a `.get_time()` method and `.to_iso_string()`.
- **UTC Consistent**: Internally uses UTC for all calculations to avoid timezone-overlap bugs.
- **Zero Dependencies**: Uses standard library `secrets`, `datetime`, and `time`.

## 🚀 Usage

```python
from chrono_id import Chrono64ms, Chrono32y, Chrono64s

# 1. Generate current ID (64-bit Millisecond precision)
id = Chrono64ms()
print(id)               # C67F-32A1-001F-4BCC (hyphenated hex)
print(int(id))          # 14303492038421033 (raw integer)
print(id.get_time())    # 2023-10-27 12:45:01.234

# 2. Generate for a specific time (32-bit Year precision)
from datetime import datetime, timezone
dt = datetime(2022, 1, 1, tzinfo=timezone.utc)
old_id = Chrono32y.from_time(dt)
print(old_id)           # 0202-0000

# 3. Parse from string (Bidirectional)
parsed_id = Chrono64ms.from_format("C67F-32A1-001F-4BCC")
# or simply use the constructor:
also_parsed = Chrono64ms("C67F-32A1-001F-4BCC")

# 4. Use as an integer
if id > old_id:
    print("IDs are sortable!")
```

## 🧪 Testing

We use `unittest` and `coverage` inside a localized virtual environment.

```bash
# Run unit tests
make test

# Run cross-platform JSON parity tests
make test-json
```

## 💎 Variants

The implementation supports 40 precision variants across 32-bit and 64-bit families:

- **64-bit Family**: `Chrono64[mo|w|d|h|m|s|ds|cs|ms|us]` and `UChrono64` equivalents.
- **32-bit Family**: `Chrono32[y|hy|q|mo|w|d|h|tm|m|bs]` and `UChrono32` equivalents.
