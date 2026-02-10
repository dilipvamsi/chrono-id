# Chrono-ID Python Implementation

A Python library for generating K-Sortable 32-bit and 64-bit identifiers.

## ✨ Features
- **Integer Inheritance**: Every ID class inherits from Python's `int`, allowing them to be used in any mathematical context.
- **Time Decoding**: All IDs expose a `.get_time()` method.
- **UTC Consistent**: Internally uses UTC for all calculations to avoid timezone-overlap bugs at epoch boundaries.
- **Zero Dependencies**: Uses standard library `secrets`, `datetime`, and `time`.

## 🚀 Usage

```python
from chrono_id import Chrono64ms, Chrono32, Chrono32h

# 1. Generate current ID (64-bit Millisecond precision)
id = Chrono64ms()
print(id)               # 94827349827349
print(id.get_time())    # 2023-10-27 12:45:01.234

# 2. Generate for a specific time (32-bit Day precision)
from datetime import datetime, timezone
dt = datetime(2020, 1, 1, tzinfo=timezone.utc)
old_id = Chrono32.from_time(dt)

# 3. Use as an integer
total = id + 1
if id > old_id:
    print("IDs are sortable!")
```

## 🧪 Testing

We use `unittest` and `coverage` inside a localized virtual environment.

```bash
make test
```

The `Makefile` will automatically:
1. Create a `venv`.
2. Install `coverage`.
3. Run the test suite in `tests/`.
4. Generate a coverage report.
