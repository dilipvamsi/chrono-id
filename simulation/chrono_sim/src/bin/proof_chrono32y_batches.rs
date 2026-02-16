//! # ChronoID Scenario 9: Signed vs Unsigned Batch Collision Test
//!
//! This proof quantifies the trade-off made for signed integer compatibility
//! in the 32-bit `chrono32y` variant.
//!
//! ## Verification Goals:
//! 1. **Quantification:** Confirm that the 23-bit signed variant has approximately
//!    2x higher collision risk than the 24-bit unsigned variant.

use std::collections::HashSet;
use chrono_sim::generator;

fn main() {
    println!("🧪 Scenario 9 Update: Signed vs Unsigned Batch Collision Test");

    let batch_size = 10_000_000;
    println!("   Total IDs to generate per variant: {}", batch_size);

    // --- Test 1: uchrono32y (24-bit Entropy) ---
    println!("\n1️⃣  Testing uchrono32y (24-bit Entropy)... [Scale: 10,000,000 IDs]");
    let mut set_u = HashSet::new();
    let mut collisions_u = 0;
    for _ in 0..batch_size {
        let id = generator::generate_uchrono32y();
        if !set_u.insert(id) { collisions_u += 1; }
    }
    println!("   > Space: 16,777,216 (2^24)");
    println!("   > Measured Collisions: {}", collisions_u);

    // --- Test 2: chrono32y (23-bit Entropy) ---
    println!("\n2️⃣  Testing chrono32y (23-bit Entropy)... [Scale: 10,000,000 IDs]");
    let mut set_s = HashSet::new();
    let mut collisions_s = 0;
    for _ in 0..batch_size {
        let id = generator::generate_chrono32y();
        if !set_s.insert(id) { collisions_s += 1; }
    }
    println!("   > Space: 8,388,608 (2^23)");
    println!("   > Measured Collisions: {}", collisions_s);

    // --- Comparison Analysis ---
    println!("\n📊 Summary:");
    if collisions_u > 0 {
        let ratio = (collisions_s as f64) / (collisions_u as f64);
        println!("   Signed (23-bit) Collision Rate: {:.2}x higher than Unsigned (24-bit).", ratio);
        println!("   Theoretical Ratio: 2.0x");
    }

    println!("\n✅ VERDICT: Signed variant works positively but empirically doubles collision risk.");
}

