# Chrono-ID C++ Implementation

A header-only C++20 library for generating K-sortable, time-based unique identifiers with sub-second precision up to microseconds.

## ✨ Features

- **Header-only**: Single file `include/chrono_id.hpp`.
- **C++20**: Uses modern C++ features like `if constexpr` and `std::chrono`.
- **K-Sortable**: IDs are sortable by time.
- **Bit-Parity**: Full bit-parity with Python and JS versions.
- **Custom Epoch**: Configurable epoch for shorter IDs (Default: 2020-01-01).
- **No Dependencies**: Standard library only.

## 🚀 Usage

Include `chrono_id.hpp` in your project.

```cpp
#include "chrono_id.hpp"
#include <iostream>

using namespace chrono_id;

int main() {
    // 1. Generate a 64-bit ID with millisecond precision
    auto id = Chrono64ms::generate();
    std::cout << "ID (BigInt): " << id.value << std::endl;
    std::cout << "Formatted:   " << id.formatted() << std::endl;  // "C67F-32A1-001F-4BCC"
    std::cout << "ISO:         " << id.to_iso_string() << std::endl;

    // 2. Parse from formatted string (Bidirectional)
    auto parsed = Chrono64ms::from_format("C67F-32A1-001F-4BCC");

    // 3. Generate for specific time and node
    auto old_id = Chrono64s::from_parts(std::chrono::system_clock::now(), 123, 0);

    return 0;
}
```

## 🧪 Build and Test

```bash
make test
# or for cross-platform JSON parity tests
make test-json
```

## 💎 Variants

The implementation supports 40 precision variants across 32-bit and 64-bit families:

- **64-bit Family**: `Chrono64[mo|w|d|h|m|s|ds|cs|ms|us]` and `UChrono64` equivalents.
- **32-bit Family**: `Chrono32[y|hy|q|mo|w|d|h|tm|m|bs]` and `UChrono32` equivalents.

Example naming:

- `UChrono64ms`: Unsigned 64-bit, millisecond precision.
- `Chrono32y`: Signed 32-bit, yearly precision (Member/Tenant IDs).
