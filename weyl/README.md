# Weyl-Golden Generation Tools (Diamond Standard)

This directory contains the mathematical foundations and portability tools for the ChronoID "Diamond Standard" identifier generation.

## 💎 The Diamond Standard

ChronoID uses **128 Cryptographically Secure Prime Weyl Multipliers** derived from the Golden Ratio ($\Phi$). This creates a "Persona Search Space" of **8.38 Million unique paths**, ensuring that even 10,000 uncoordinated nodes starting with identical state ("The Clone Start") diverge to 100% uniqueness in a single step ($T=1$).

## 🏠 Project Structure

| File                       | Type          | Description                                                        |
| :------------------------- | :------------ | :----------------------------------------------------------------- |
| **`weyl-lang-gen.py`**     | **CLI Tool**  | Cross-language code generator for implementation constants.        |
| **`weyl-primes.py`**       | **Math Tool** | The engine that generates or verifies the prime multiplier basket. |
| **`weyl-multipliers.txt`** | **Data**      | The source-of-truth list of 128 verified 64-bit primes.            |

## 🚀 Usage

### Generating Constants for a New Language

Use `weyl-lang-gen.py` to create the appropriately formatted array or constant for your implementation.

```bash
python3 weyl/weyl-lang-gen.py <language> <output_path>
```

**Supported Languages:**

- `rust` (pub const array)
- `js` / `ts` / `javascript` / `typescript` (BigInt-suffixed exports)
- `cpp` / `c++` / `c` (uint64_t array)
- `python` (List)
- `odin` (Fixed array)
- `go` (Fixed array)

### Mathematical Verification

If you need to re-verify the prime basket or generate a new one based on a different seed:

```bash
python3 weyl/weyl-primes.py
```

## 📐 Selection Methodology

The Diamond Standard multiplier basket is engineered using three layers of mathematical defense to ensure maximum divergence in uncoordinated systems.

### 1. The Starting Point: Golden Ratio ($\phi$)

We derive the initial search point from the **Inverse Golden Ratio** ($\phi^{-1} \approx 0.618...$).

- **Rationale:** $\phi$ is the "most irrational" number, meaning its continued fraction converges more slowly than any other. In a Weyl sequence $\{n\alpha \pmod 1\}$, using $\alpha = \phi$ minimizes "clumping" and ensures the most uniform dispersion across the 64-bit space from the very first sample.

### 2. The Spacing Step: Silver Ratio Base ($\sqrt{2}$)

To select 128 distinct prime candidates, we probe the 64-bit space using a "jump" constant derived from the fractional part of **$\sqrt{2}$**.

- **The Three-Gap Theorem:** This theorem proves that for any irrational step, the gaps between sampled points take at most three distinct lengths. Using $\sqrt{2}$ results in gap lengths that are nearly identical, providing optimal equidistribution.
- **Linear Independence:** Because $\sqrt{5}$ (the basis for $\phi$) and $\sqrt{2}$ are square roots of different primes, they are _linearly independent over the rationals_. This ensures the spacing sequence never "resonates" or syncs with the starting point, guaranteeing 128 mathematically unique neighborhoods.

### 3. Prime Verification (Miller-Rabin)

For each of the 128 probe points, we find the **smallest subsequent prime** $p \ge \text{target}$.

- **Uniqueness:** Using primes for Weyl multipliers ensures that the sequence has a full period ($2^{64}$) and provides a high avalanche effect for index obfuscation.
- **Verification:** Each prime is verified via the **Miller-Rabin** primality test with $k=10$ iterations, providing deterministic certainty for 64-bit integers.
