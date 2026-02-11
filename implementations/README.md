# Chrono-ID Implementations

This directory contains the official library implementations of Chrono-ID across several languages. Every implementation is designed to be **Bit-Parity Consistent**, meaning identical inputs (Time + Random) produce identical 32/64-bit IDs across all platforms.

## 📂 Languages Support

- [**C++**](./cpp) - Header-only template library using C++20.
- [**Python**](./python) - Native Python implementation with 100% test coverage.
- [**JavaScript**](./js) - Support for Node.js and Browser environments.

## 🧪 Cross-Platform Testing

We use a unified testing strategy to ensure that all languages adhere to the same bit layout and error handling logic.

### Global Test Suite
Run the full suite for all languages in a single pass:
```bash
make test
```

### Targeted Testing
Run tests for a specific implementation:
```bash
make test-cpp     # C++ Unit & JSON Parity Tests
make test-python  # Python Unittest & Coverage
make test-js      # Node.js Test Runner
```

### JSON Parity Tests
The cross-platform consistency is verified using a shared JSON test suite found in the root `tests/` directory. Each language includes a dedicated runner to parse and validate these cases.
