# Chrono-ID C++ Implementation

A header-only C++20 library for generating K-sortable, time-based unique identifiers with sub-second precision up to microseconds.

## Features

- **Header-only**: Single file `include/chrono_id.hpp`.
- **C++20**: Uses modern C++ features.
- **K-Sortable**: IDs are sortable by time.
- **Microsecond Precision**: Supports millisecond and microsecond precision.
- **Custom Epoch**: Configurable epoch for shorter IDs.
- **Randomness**: Includes random bits for collision resistance.
- **No Dependencies**: Standard library only.

## Usage

Include `chrono_id.hpp` in your project.

```cpp
#include "chrono_id.hpp"
#include <iostream>

using namespace chrono_id;

int main() {
    // Generate a 64-bit ID with millisecond precision
    auto id = Chrono64ms();
    std::cout << "ID: " << id.value << std::endl;
    std::cout << "ISO: " << id.to_iso_string() << std::endl;

    // Parse from ISO string
    auto parsed = Chrono64ms::from_iso_string("2023-01-01T12:00:00.123Z");
    std::cout << "Parsed ID: " << parsed.value << std::endl;

    return 0;
}
```

## Error Handling

The library throws `std::invalid_argument` for:
- `nullptr` input strings.
- Invalid ISO 8601 date formats.
- Timestamp underflows (dates before the configured Epoch).
- Dates before 1970-01-01.

## Build and Test

```bash
make test
# or for coverage
make coverage
```

## Variants

- `Chrono32`, `UChrono32`: 32-bit seconds precision.
- `Chrono64`, `UChrono64`: 64-bit seconds precision.
- `Chrono64ms`, `UChrono64ms`: 64-bit milliseconds precision.
- `Chrono64us`, `UChrono64us`: 64-bit microseconds precision.
