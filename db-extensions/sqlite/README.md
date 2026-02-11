# Chrono-ID SQLite Extension

A SQLite extension that provides Chrono-ID generation functions.

## Features

- Generates K-sortable unique identifiers directly in SQL.
- Supports all Chrono-ID variants (32/64-bit, various precisions).
- ISO 8601 string conversion.

## Building

Requires a C++20 compiler.

```bash
make
```

This produces `build/chrono_id.so` (or `.dylib` on macOS).

## Loading in SQLite

```sql
.load ./build/chrono_id
```

## Usage

```sql
-- Generate a new ID (defaults to 64ms)
SELECT chrono_new();
-- Or
SELECT chrono64ms();

-- Generate specific variants
SELECT chrono32();
SELECT uchrono64us();

-- Convert to ISO string
SELECT chrono_to_iso(chrono64ms(), '64ms');
-- Result: 2023-10-27T10:00:00.123Z

-- Parse from ISO string
SELECT chrono_from_iso('2023-10-27T10:00:00.123Z', '64ms');
```

## Error Handling

The extension raises SQLite errors (e.g., `OperationalError` in Python) with descriptive messages for:
- Invalid ISO 8601 strings.
- NULL inputs to `from_iso` functions.
- Timestamp underflows.

## Testing

Requires Python 3.

```bash
python3 tests/test_chrono_id_sqlite.py
```
