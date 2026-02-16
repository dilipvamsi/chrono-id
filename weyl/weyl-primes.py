"""
ChronoID Diamond Standard Multiplier Generator

This script calculates 128 unique 64-bit prime multipliers derived from the
Golden Ratio (Phi). These multipliers ensure maximum divergence and period
for the Weyl-Golden mixers in uncoordinated distributed systems.
"""

import math
import random
from decimal import Decimal, getcontext

# Set precision high enough for 64-bit integer conversion
getcontext().prec = 30

def is_prime(n, k=10):
    """
    Performs the Miller-Rabin primality test.
    k is the number of iterations; 10 is sufficient for 64-bit numbers.
    """
    if n <= 1: return False
    if n <= 3: return True
    if n % 2 == 0: return False

    # Miller-Rabin primality test
    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2
    for _ in range(k):
        a = random.randint(2, n - 2)
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def get_next_prime(n):
    """
    Finds the smallest prime p >= n.
    """
    if n % 2 == 0: n += 1
    while not is_prime(n):
        n += 2
    return n

# --- Constant Derivation using high-precision Decimal ---

# 1. Starting Point: The Golden Ratio (Phi)
# PHI = floor(2^64 * (sqrt(5)-1)/2)
#
# Rationale:
# The Golden Ratio is the "most irrational" number because its continued fraction
# [1; 1, 1, ...] converges more slowly than any other. In a Weyl sequence {n*alpha},
# using phi ensures that the points are distributed with the lowest possible
# discrepancy (clumping), effectively maximizing the "spread" across the
# 64-bit entropy space from the very first sample.
#
# Derived from: (sqrt(5) - 1) / 2 = 0.6180339887...
phi_inv = (Decimal(5).sqrt() - 1) / 2
PHI = int(phi_inv * Decimal(2**64))

# 2. Spacing Step: The Silver Ratio base (sqrt(2))
# STEP = floor(2^64 * (sqrt(2)-1))
#
# Rationale (Three-Gap Theorem & Silver Ratio):
# When selecting a 'jump' constant for 128 distinct probes, we must avoid
# "Mathematical Resonance". Resonance occurs if the STEP is rationally related
# to the starting PHI, causing the sequence to "clump" into predictable
# patterns rather than visiting fresh regions.
#
# Mathematical Frameworks used:
# A. Three-Gap Theorem: States that for any irrational alpha, the points
#    partition the circle into at most three distinct gap lengths. Choosing
#    sqrt(2) (the Silver Ratio base) results in gap lengths that are nearly
#    identical, providing optimal "Equidistribution".
#
# B. Linear Independence: Because sqrt(5) and sqrt(2) are square roots of
#    different primes, they are linearly independent over the rational numbers.
#    This guarantees that the spacing sequence will never "sync" with the
#    phi-based starting point, ensuring each of the 128 primes is found in a
#    mathematically unique neighborhood.
step_raw = Decimal(2).sqrt() - 1
STEP = int(step_raw * Decimal(2**64))

def main():
    """
    Generates the 128-prime Weyl basket and prints raw hex values.
    """
    mults = []
    current = PHI

    # Generate 128 spacing points and find the nearest subsequent prime for each
    for i in range(128):
        # We wrap at 64-bits
        target = (current + (i * STEP)) & 0xFFFFFFFFFFFFFFFF
        p = get_next_prime(target)
        mults.append(p)

    # Print raw hex values for the toolchain
    for m in mults:
        print(f"0x{m:016X}")

if __name__ == "__main__":
    main()
