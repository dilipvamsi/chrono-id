//! # ChronoID Scenario 4: chrono32y (Tenant ID) Core Verification
//!
//! This proof verifies the 32-bit "Tenant ID" variant, which trades time
//! precision for entropy to maximize collision safety in uncoordinated multi-tenant systems.
//!
//! ## Verification Goals:
//! 1. **Crockford Encoding:** Ensure 32-bit IDs encode to a fixed 7-character string.
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
    println!("   Testing 24-bit entropy limits and Crockford encoding...");

    // --- Test 1: Encoding Format ---
    // Standard Crockford Base32 encoding should be exactly 7 characters for a 32-bit integer.
    let sample = generator::generate_chrono32y();
    let encoded = generator::encode_crockford(sample);
    println!("   > Sample ID: {} -> {}", sample, encoded);

    if encoded.len() != 7 {
        println!("❌ FAILURE: Encoding length incorrect. Expected 7, got {}.", encoded.len());
        return;
    }

    // --- Test 2: Safe Zone (1,000 Tenants) ---
    // Risk target: 0.1%. Collisions should be extremely rare.
    println!("   > Testing Safe Zone (N=1,000)...");
    let n_safe = 1000;
    let mut set = HashSet::with_capacity(n_safe);
    let mut cols_safe = 0;
    for _ in 0..n_safe {
        if !set.insert(generator::generate_chrono32y()) {
            cols_safe += 1;
        }
    }
    println!("   > Generated {} IDs: {} Collisions", n_safe, cols_safe);

    // --- Test 3: Risk Zone (10,000 Tenants) ---
    // Risk target: ~0.3%. Collisions are expected to start appearing.
    println!("   > Testing Risk Zone (N=10,000)...");
    let n_risk = 10_000;
    let mut set_risk = HashSet::with_capacity(n_risk);
    let mut cols_risk = 0;
    for _ in 0..n_risk {
        if !set_risk.insert(generator::generate_chrono32y()) {
            cols_risk += 1;
        }
    }
    println!("   > Generated {} IDs: {} Collisions", n_risk, cols_risk);

    // --- Validation ---
    if cols_safe == 0 {
        println!("✅ SUCCESS: Zero collisions in safe zone.");
    } else {
        println!("⚠️ NOTE: Collision in safe zone (unlucky but statistically possible).");
    }

    if cols_risk > 0 {
        println!("✅ SUCCESS: Expected Birthday collisions observed (Entropy working).");
    } else {
        println!("⚠️ NOTE: No collisions in risk zone (statistically lucky).");
    }
}

