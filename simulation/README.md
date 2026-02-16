# ChronoID Simulation Proof

This directory contains the empirical validation suite for ChronoID's mathematical and operational claims. Using a high-performance Rust simulation environment, we torture-test every variant and mode defined in the specification.

## 🎯 Verification Objectives

The primary goal is to provide **Empirical Proof** for:

1.  **Divergence:** Uncoordinated nodes with identical starting states diverge rapidly.
2.  **Uniqueness:** Collision rates stay within calculated Birthday Paradox limits.
3.  **Signed Safety:** 32-bit and 64-bit variants remain positive in signed-integer systems.
4.  **Operational Modes:** Mode B (Weyl-Step) and Mode C (Shard Routing) guarantees hold under stress.

## 🧪 Simulation Scenarios

The suite is divided into 24 distinct scenarios:

### Core Foundation

- **Scenario 1: The Perfect Storm:** Forced collision at $T=0$ followed by divergence at $T+1$ via persona rotation.
- **Scenario 2: Entropy & Birthday Limits:** 1,000,000 ID generation test from a single node.
- **Scenario 3: Burst Throughput:** Verification of non-blocking sequence overflow logic.

### 32-bit `chrono32y` (Tenant IDs)

- **Scenario 4: Tenant ID Layout:** Validation of 32-bit packing and Crockford encoding.
- **Scenario 9: Signed vs Unsigned:** Comparative analysis of 23-bit vs 24-bit entropy collision rates.
- **Scenario 10: Mode B Permutation:** Full 16.7M ID cycle proof for gapless 32-bit generation.
- **Scenario 11: Register Performance:** Proved that 64-bit operations are 1.96x faster than 128-bit (Theorem 3).

### Operational Modes (Stateful & Managed)

- **Scenario 5: Mode C (Shard Routing):** Verifies raw Node ID preservation for infrastructure routing.
- **Scenario 6: Mode B (Burst Capacity):** 1 Billion ID generation test using Weyl-Step rotation.
- **Scenario 14: Mode C Spin-Wait:** Empirically proves blocking behavior when burst capacity is exceeded.

### Reliability & Infrastructure

- **Scenario 7: Clock Rollback:** Verifies uniqueness remains intact when the system clock jumps backwards.
- **Scenario 8: Thread Contention:** Heavy concurrency test (100 threads) using a shared protected generator.
- **Scenario 12: bit-Split Audit:** Systematic bit-compliance check for all 18 ChronoID variants.
- **Scenario 13: Sortability & Decoding:** Lossless Crockford round-trip and bucket-sort monotonicity verification.
- **Scenario 15: Epoch Coexistence:** Verifies absolute sorting behavior across multi-epoch wraps.
- **Scenario 16: Birthday Bound Accuracy:** Confirmed mathematical limits match empirical collision rates.

### Physical & Logic Parity

- **Scenario 17: Index Locality:** Verified 49% storage reduction and 3.87x faster ingestion vs UUIDv4/v7.
- **Scenario 18: SQL Logic Parity:** Achieved 100% bit-match between SQL spec and Rust implementation.
- **Scenario 19: Avalanche Test:** Confirmed ~30% multiplier dispersion for obfuscation.
- **Scenario 20: High-Freq Mode A Risk:** Proved necessity of Mode B/C for sub-second precision variants.
- **Scenario 21: Variant Isolation:** Verifies bit-overlap and sorting hazards when mixing different precision variants.
- **Scenario 22: Causal Jitter:** Quantifies cross-node ordering accuracy under uncoordinated NTP drift.
- **Scenario 23: Boundary Analysis:** Verifies behavior at bit-saturation and epoch-zero states.
- **Scenario 24: Monotonicity Audit:** Empirically proves ordering behavior during sequence overflows.

## 🛠 Usage

We provide a `Makefile` to run individual scenarios or the entire suite.

### Run All Scenarios

```bash
make all
```

### Run Specific Proofs

- **Core Suite:** `make s1`, `make s2`, `make s3`
- **Tenant Suite:** `make s4`, `make s9`, `make s10`
- **Operational Suite:** `make s5`, `make s6`, `make s14`
- **Safety Suite:** `make s7`, `make s8`, `make s11`, `make s12`, `make s13`

## 📂 Project Structure

- **`chrono_sim/src/generator.rs`**: Reference implementation of ChronoID (Ported to Rust).
- **`chrono_sim/src/lib.rs`**: Library interface.
- **`chrono_sim/src/bin/`**: Individual proof binaries for each scenario.
- **`report.md`**: Cumulative results and statistical analysis from simulations.
- **`Makefile`**: Automation script for the verification suite.

---

_Verified ChronoID binaries are located in `chrono_sim/target/release/` after running `make`._
