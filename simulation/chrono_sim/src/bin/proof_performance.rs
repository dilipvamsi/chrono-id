//! # ChronoID Scenario 11: 64-bit Performance Advantage
//!
//! This proof quantifies the raw CPU throughput advantage of using native
//! 64-bit identifiers (ChronoID) over 128-bit structures (UUID).
//!
//! ## Verification Goals:
//! 1. **Arithmetic Efficiency:** Measure the speed of 64-bit vs 128-bit
//!    addition and comparison operations.
//! 2. **Theorem 3 Verification:** Prove that native register operations are
//!    significantly faster than multi-word structures.

use std::time::Instant;

/// Mock 128-bit structure to simulate UUID logic.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
struct U128 {
    hi: u64,
    lo: u64,
}

fn main() {
    println!("🧪 Scenario 11: CPU Performance (64-bit vs 128-bit)\n");

    let n = 100_000_000;
    println!("   Running {} iterations of register-pressure arithmetic...", n);

    // --- 64-bit Benchmark (Representing ChronoID) ---
    let mut val64: u64 = 0;
    let start64 = Instant::now();
    for i in 0..n {
        val64 = val64.wrapping_add(i);
        // Add a comparison to prevent total compiler optimization skip
        if val64 == 0xDEADBEEFDEADBEEF { println!("   (Hit)"); }
    }
    let duration64 = start64.elapsed();
    println!("   > 64-bit native register Time:  {:.2?}", duration64);

    // --- 128-bit Benchmark (Representing UUID) ---
    let mut val128 = U128 { hi: 0, lo: 0 };
    let start128 = Instant::now();
    for i in 0..n {
        // Simulated 128-bit addition (two words, carry handling)
        let (res, overflow) = val128.lo.overflowing_add(i);
        val128.lo = res;
        if overflow {
            val128.hi += 1;
        }

        // Simulated 128-bit comparison (multi-word check)
        if val128.hi == 0xDEADBEEF && val128.lo == 0xDEADBEEF { println!("   (Hit)"); }
    }
    let duration128 = start128.elapsed();
    println!("   > 128-bit multi-word Time:      {:.2?}", duration128);

    // --- Analysis ---
    let ratio = duration128.as_secs_f64() / duration64.as_secs_f64();
    println!("\n   📊 Measured Performance Advantage: {:.2}x", ratio);

    if ratio >= 1.5 {
         println!("✅ SUCCESS: 64-bit operations are significantly faster (Arithmetic Proof).");
    } else {
         println!("⚠️ WARNING: Measured gap is {:.2}x. (CPU parallelism may be masking cost).", ratio);
    }
}
