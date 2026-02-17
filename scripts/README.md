# Chrono-ID Scripts

This directory contains utility scripts for the Chrono-ID project, primarily focused on cross-platform test generation and verification.

## gen_tests.py

This script generates the exhaustive `tests/cross_platform_tests.json` file used for bit-parity verification across all Chrono-ID implementations.

It exercises:

- All **40 Chrono-ID variants** (64-bit and 32-bit families).
- All **128 Weyl multipliers** defined in the Diamond Standard.
- Multiple critical timestamps (Epoch, Far Future, Leap Year).

### Usage

You can run the script manually or via the provided `Makefile`.

```bash
python3 gen_tests.py
```

Note: The script requires the Python implementation to be in the `PYTHONPATH`.
