# ChronoID Simulation Proof

This directory contains the empirical validation suite for ChronoID's mathematical and operational claims. Using a high-performance Rust simulation environment, we torture-test every variant and mode defined in the specification.

## 🎯 Verification Objectives

The primary goal is to provide **Empirical Proof** for:

1.  **Divergence:** Uncoordinated nodes with identical starting states diverge rapidly.
2.  **Uniqueness:** Collision rates stay within calculated Birthday Paradox limits.
3.  **Signed Safety:** 32-bit and 64-bit variants remain positive in signed-integer systems.
4.  **Operational Modes:** Mode B (Weyl-Step) and Mode C (Shard Routing) guarantees hold under stress.

## 🧪 Simulation Scenarios

The suite is divided into 26 distinct scenarios:

### Foundation & Safety (Scenarios 1-10)

- **Scenario 1: Perfect Storm** [Scale: 10,000 Nodes, 1 Forced Hero Case] — Forced collision at $T=0$ followed by divergence at $T+1$ via CSPRNG persona rotation. Verified 100% recovery across 10k colliding nodes.
- **Scenario 2: Entropy Distribution** [Scale: 1,000,000 IDs] — Statistical audit of 1M IDs to ensure uniform bit-density and zero "clumping". Result: Uniform distribution.
- **Scenario 3: Burst Capacity** [Scale: 4,096 IDs/ms] — Stress tests sequence overflow logic; verifies safe saturation without duplicates. Result: 0 collisions.
- **Scenario 4: Tenant Safety (chrono32y)** [Scale: 1,000,000 IDs] — Proves Birthday Paradox bound for 24-bit identifiers in shared DB clusters.
- **Scenario 5: Multi-Shard Routing** [Scale: 10,000,000 IDs] — Verified O(1) shard routing by bit extraction on 10M records.
- **Scenario 6: Instance-Native (Mode B)** [Scale: 1,000,000,000 IDs] — Verified zero collisions in massive instance-local bursts via Weyl-Step.
- **Scenario 7: Clock Rollback** [Scale: 1,000 IDs] — Simulated NTP correction; verified safety via sequence carry-over even during rollback.
- **Scenario 8: Contention** [Scale: 100 Threads, 1M IDs] — High-concurrency stress test on shared mutex generator.
- **Scenario 9: Signed Risk** [Scale: 10,000,000 IDs] — Quantified collision risk of 23-bit `i32` compatibility. (Confirmed 2x risk).
- **Scenario 10: Mode B Perfect-Cycle** [Scale: 16,777,216 IDs] — Proved perfect permutation of 24-bit space.

### Performance & Storage (Scenarios 11-18)

- **Scenario 11: Register Efficiency** [Scale: 100,000,000 Ops] — CPU throughput advantage of 64-bit native arithmetic. (Confirmed 2.85x).
- **Scenario 12: Bit-Split Audit** [Scale: 18 Variants] — Verification of T/N/S widths across all spectral variants.
- **Scenario 13: Sortability Round-Trip** [Scale: 100,000 IDs] — Verified lossless Crockford Base32 encoding for FK portability.
- **Scenario 14: Burst Wait-Defense** [Scale: 100,000 Burst IDs] — Verified Mode C "Spin-Wait" blocking during millisecond sequence saturation.
- **Scenario 15: Shard Routing Efficiency** [Scale: 1,000,000,000 Reqs] — O(1) routing performance at 1 billion requests.
- **Scenario 16: Birthday Bound** [Scale: k=2,072 Nodes] — Empirical proof of 0.1% collision risk target.
- **Scenario 17: Storage Footprint** [Scale: 1,000,000 Rows] — Physical B-Tree size comparison vs UUIDv4/v7. (Confirmed ~50% saving).
- **Scenario 18: SQL Parity** [Scale: 10,000 Match-Tests] — 100% bit-parity between SQL spec and Rust implementation.

### Advanced Edge Cases (Scenarios 19-26)

- **Scenario 19: Multiplier Avalanche** [Scale: 1,000,000 IDs] — Measuring bit-dispersion on adjacent generations.
- **Scenario 20: Mode A Unsuitable** [Scale: 2,000 Nodes (us)] — Demonstration of high collision risk in uncoordinated high-freq workloads.
- **Scenario 21: Variant Isolation** [Scale: 2^15 Combinations] — Confirmed that mixing variants in one column causes deterministic collisions.
- **Scenario 22: Causal Jitter** [Scale: 1,000,000 Events] — Quantified sort-inversion risk under 50ms NTP skew.
- **Scenario 23: Boundary Saturation** [Scale: Min/Max] — Verified T=0 and T=Max behavior.
- **Scenario 24: Monotonicity Breach** [Scale: 10,000 Overflows] — Audited sort violations during persona rotation.
- **Scenario 25: chrono32y Storage** [Scale: 1,000,000 IDs] — Final verification of 55.4% savings in 32-bit tenant systems.
- **Scenario 26: Shard Routing O(1)** [Scale: 1,000,000,000 Reqs] — Final sub-millisecond overhead certification.

## 🛠 Usage

We provide a `Makefile` to run individual scenarios or the entire suite.

### Run All Scenarios

```bash
make all
```

### Run Specific Proofs

- **Core Suite:** `make s1`, `make s2`, `make s3`
- **Tenant Suite:** `make s4`, `make s9`, `make s10`
- **Operational Suite:** `make s5`, `make s6`, `make s14`, `make s26`
- **Safety Suite:** `make s7`, `make s8`, `make s11`, `make s12`, `make s13`

## 📂 Project Structure

- **`chrono_sim/src/generator.rs`**: Reference implementation of ChronoID (Ported to Rust).
- **`chrono_sim/src/lib.rs`**: Library interface.
- **`chrono_sim/src/bin/`**: Individual proof binaries for each scenario.
- **`report.md`**: Cumulative results and statistical analysis from simulations.
- **`Makefile`**: Automation script for the verification suite.

---

_Verified ChronoID binaries are located in `chrono_sim/target/release/` after running `make`._
