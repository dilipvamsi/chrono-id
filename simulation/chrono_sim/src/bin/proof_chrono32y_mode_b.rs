//! # ChronoID Scenario 10: chrono32y Mode B (Weyl Sequence Cycle)
//!
//! This proof verifies the "Perfect Permutation" guarantee of Mode B for the
//! 32-bit `chrono32y` variant.
//!
//! ## Verification Goals:
//! 1. **Zero Collision Cycle:** Prove that 16.7M IDs can be generated sequentially
//!    without a single duplicate by leveraging a Weyl Sequence.
//! 2. **Full Space Coverage:** Confirm that every possible entropy value in the
//!    2^24 space is visited exactly once.

use std::sync::atomic::{AtomicU64, Ordering};
use chrono_sim::generator;
use std::time::Instant;

fn main() {
    println!("🧪 Scenario 10: chrono32y Mode B (Weyl Sequence)");

    // Objective: Verify that Mode B logic (Weyl Sequence) produces
    // a collision-free permutation of the full 24-bit entropy space.

    // Initialize sequence with an arbitrary start offset
    let seq = AtomicU64::new(12345);

    let total_space = 1 << 24; // 16,777,216 IDs
    println!("   > Testing full 24-bit cycle ({} IDs)...", total_space);

    // Use a boolean vector as a BitSet to track seen IDs (16MB memory footprint)
    let mut seen = vec![false; total_space];

    let start = Instant::now();
    let mut collisions = 0;

    // Generate every possible value in one rotation cycle
    for i in 0..total_space {
        let id_u32 = generator::generate_chrono32y_mode_b(&seq);

        // Extract the 24-bit entropy suffix
        let entropy = (id_u32 & 0xFFFFFF) as usize;

        if seen[entropy] {
            if collisions == 0 {
                println!("❌ FIRST COLLISION at index {}! Value: {:x}", i, entropy);
            }
            collisions += 1;
        } else {
            seen[entropy] = true;
        }
    }

    let duration = start.elapsed();
    println!("   > Generated {} IDs in {:.2?}", total_space, duration);

    if collisions == 0 {
        println!("✅ SUCCESS: Full 24-bit space covered with ZERO collisions.");
        println!("           chrono32y Mode B is a verified perfect permutation.");
    } else {
        println!("❌ FAILURE: {} Collisions detected.", collisions);
    }
}

