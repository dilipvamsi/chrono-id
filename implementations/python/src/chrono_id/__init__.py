"""
Chrono-ID: Diamond Standard ID Generation.

This package implements the Chrono-ID specification, providing 64-bit and 32-bit
time-relative identifiers with high entropy and bit-parity verified across platforms.

Basic Usage:
    from chrono_id import Chrono64s

    # Generate a new ID
    cid = Chrono64s()
    print(f"ID: {cid}")
    print(f"ISO: {cid.to_iso_string()}")

For more advanced usage and variant selection, see:
https://github.com/dilipvamsi/chrono-id
"""

from .core import (
    ChronoError,
    Precision,
    ChronoBase,
    EPOCH_2020,
    Persona,
    Generator,

    # 64-bit variants
    UChrono64mo, Chrono64mo,
    UChrono64w, Chrono64w,
    UChrono64d, Chrono64d,
    UChrono64h, Chrono64h,
    UChrono64m, Chrono64m,
    UChrono64s, Chrono64s,
    UChrono64ds, Chrono64ds,
    UChrono64cs, Chrono64cs,
    UChrono64ms, Chrono64ms,
    UChrono64us, Chrono64us,

    # 32-bit variants
    UChrono32y, Chrono32y,
    UChrono32hy, Chrono32hy,
    UChrono32q, Chrono32q,
    UChrono32mo, Chrono32mo,
    UChrono32w, Chrono32w,
    UChrono32d, Chrono32d,
    UChrono32h, Chrono32h,
    UChrono32tm, Chrono32tm,
    UChrono32m, Chrono32m,
    UChrono32bs, Chrono32bs
)

__all__ = [
    'ChronoError',
    'Precision',
    'ChronoBase',
    'EPOCH_2020',
    'Persona',
    'Generator',
    'UChrono64mo', 'Chrono64mo',
    'UChrono64w', 'Chrono64w',
    'UChrono64d', 'Chrono64d',
    'UChrono64h', 'Chrono64h',
    'UChrono64m', 'Chrono64m',
    'UChrono64s', 'Chrono64s',
    'UChrono64ds', 'Chrono64ds',
    'UChrono64cs', 'Chrono64cs',
    'UChrono64ms', 'Chrono64ms',
    'UChrono64us', 'Chrono64us',
    'UChrono32y', 'Chrono32y',
    'UChrono32hy', 'Chrono32hy',
    'UChrono32q', 'Chrono32q',
    'UChrono32mo', 'Chrono32mo',
    'UChrono32w', 'Chrono32w',
    'UChrono32d', 'Chrono32d',
    'UChrono32h', 'Chrono32h',
    'UChrono32tm', 'Chrono32tm',
    'UChrono32m', 'Chrono32m',
    'UChrono32bs', 'Chrono32bs'
]
