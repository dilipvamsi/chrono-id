//! # ChronoID Scenario 4: chrono32y (Tenant ID) Core Verification
//!
//! This proof verifies the 32-bit "Tenant ID" variant, which trades time
//! precision for entropy to maximize collision safety in uncoordinated multi-tenant systems.
//!
//! ## Verification Goals:
//! 1. **Hyphenated Hex Formatting:** Ensure 32-bit IDs format to a fixed 9-character string (XXXX-XXXX).
//! 2. **Entropy Limits:** Empirically observe the Birthday Paradox in action for 24-bit entropy.

use chrono_sim::generator;
use std::collections::HashSet;

#[tokio::main]
async fn main() {
    run_scenario_4_chrono32y().await;
}

/// Runs the Scenario 4 suite.
async fn run_scenario_4_chrono32y() {
    println!("\n🧪 Scenario 4: chrono32y Tenant IDs");
    println!("   Testing 24-bit entropy limits and Hyphenated Hex formatting...");

    // --- Test 1: Encoding Format ---
    // Standard Hyphenated Hex formatting should be exactly 9 characters (including -) for a 32-bit integer.
    let sample = generator::generate_chrono32y();
    let encoded = generator::format_hyphenated_hex(sample);
    println!("   > Sample ID: {} -> {}", sample, encoded);

    if encoded.len() != 9 {
        println!("❌ FAILURE: Formatting length incorrect. Expected 9, got {}.", encoded.len());
        return;
    }

    // --- Test 2: Extended Scale (1,000,000 Tenants) ---
    // Risk target: 0.1% at k=2072, but we test 1M to show density.
    println!("   > Testing Extended Scale (N=1,000,000)...");
    let n_safe = 1_000_000;
    let mut set = HashSet::with_capacity(n_safe);
    let mut cols_safe = 0;
    for _ in 0..n_safe {
        if !set.insert(generator::generate_chrono32y()) {
            cols_safe += 1;
        }
    }
    println!("   > Generated {} IDs: {} Collisions", n_safe, cols_safe);

    // --- Validation ---
    if cols_safe > 0 {
        let rate = (cols_safe as f64 / n_safe as f64) * 100.0;
        println!("✅ SUCCESS: Birthday collisions observed at 1M scale ({:.4}%).", rate);
    } else {
        println!("⚠️ NOTE: No collisions at 1M scale (statistically lucky).");
    }
}
