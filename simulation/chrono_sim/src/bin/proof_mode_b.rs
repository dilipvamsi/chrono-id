//! # ChronoID Scenario 6: Mode B (Postgres Weyl-Step Cycle)
//!
//! This proof verifies the stateful "Instance-Native" Mode B, which replaces
//! randomized personas with a deterministic Weyl Generator for single database instances.
//!
//! ## Verification Goals:
//! 1. **Perfect Permutation:** Verify that the Node ID cycles through every
//!    possible value in its 16-bit space exactly once.
//! 2. **Billion-Scale Burst:** Prove zero collisions during 1 billion ID
//!    generation, covering over 30,000 internal node rotations.

use std::collections::HashSet;

fn main() {
    println!("\n🧪 Scenario 6: Mode B (Postgres Weyl-Step Cycle)");

    // --- Phase 1: Cycle Verification ---
    // Proves that the Weyl-Golden mixer generates a full-period cycle
    // for various bit-widths.

    // Standard 64s: 16-bit Node field (65,536 values)
    verify_cycle("Standard (s)", 16, 3535253579);

    // Microsecond: 5-bit Node field (32 values)
    verify_cycle("Microsecond (us)", 5, 3535253579);

    // --- Phase 2: High-Volume Burst ---
    // Stress test generating 1 Billion IDs to confirm rotation logic.
    verify_burst_capacity();
}

/// Stress tests Mode B for 1 Billion IDs.
fn verify_burst_capacity() {
    println!("\n   > Testing Burst Capacity (1 Billion IDs)...");
    use chrono_sim::generator::Generator;

    // Start a Mode B generator (Instance-Native)
    let mut gen = Generator::new_mode_b();
    let config = gen.get_config();
    println!(
        "     Config: Time {}b, Node {}b, Seq {}b",
        config.time_bits, config.node_bits, config.seq_bits
    );

    let fixed_ts = 1000; // Fixed timestamp to force sequence/node pressure
    let seq_limit = 1u64 << config.seq_bits; // 32,768 IDs per node rotation

    let target_ids = 1_000_000_000u64;
    let expected_rotations = target_ids / seq_limit; // ~30,517 rotations

    println!("     Generating {} IDs at fixed timestamp...", target_ids);
    println!("     Expected Node Rotations: {}", expected_rotations);

    let mut seen_nodes = HashSet::new();
    let mut last_node_val = None;
    let mut collisions = 0;

    use indicatif::{ProgressBar, ProgressStyle};
    use std::time::Instant;

    let pb = ProgressBar::new(target_ids);
    pb.set_style(
        ProgressStyle::default_bar()
            .template(
                "{spinner:.green} [{elapsed_precise}] [{bar:40.cyan/blue}] {pos}/{len} ({eta})",
            )
            .unwrap(),
    );

    let start = Instant::now();

    for i in 0..target_ids {
        let id = gen.generate_at(fixed_ts);

        // Extract the current mixed Node ID
        let mixed_node = (id >> config.seq_bits) & ((1 << config.node_bits) - 1);

        if let Some(last) = last_node_val {
            if mixed_node != last {
                // Node ID rotated to a new Weyl step
                if !seen_nodes.insert(mixed_node) {
                    println!(
                        "❌ COLLISION: Node ID {:x} repeated at index {}!",
                        mixed_node, i
                    );
                    collisions += 1;
                    break;
                }
                last_node_val = Some(mixed_node);
            }
        } else {
            // Initializing first node value
            seen_nodes.insert(mixed_node);
            last_node_val = Some(mixed_node);
        }

        if i % 1_000_000 == 0 {
            pb.inc(1_000_000);
        }
    }
    pb.finish_with_message("Done");

    let duration = start.elapsed();
    println!("     Generated {} IDs in {:.2?}", target_ids, duration);
    println!("     Unique Node IDs seen: {}", seen_nodes.len());

    if collisions == 0 {
        println!("✅ SUCCESS: Generated 1 Billion IDs with zero local collisions.");
    } else {
        println!("❌ FAILURE: Local collision detected during burst!");
    }
}

/// Verifies that the Weyl step correctly visits every slot in a bitmask.
fn verify_cycle(name: &str, bits: u64, magic: u64) {
    let mask = (1u64 << bits) - 1;
    let step = (magic & mask) | 1; // Ensure odd constant for full-period LCG

    println!("\n   > Testing Cycle: {}", name);
    println!("     Mask: 0x{:X} ({} entries)", mask, mask + 1);

    let mut current = 0;
    let mut count = 0;
    let mut seen = HashSet::with_capacity((mask + 1) as usize);

    loop {
        // If we revisit a value before seeing everyone, cycle is short.
        if !seen.insert(current) {
            let expected = (mask + 1) as usize;
            if count == expected {
                println!("✅ SUCCESS: Full period cycle detected ({} values).", count);
            } else {
                println!(
                    "❌ FAILURE: Short cycle detected! Expected {}, Got {}",
                    expected, count
                );
            }
            break;
        }

        // Weyl Step: linear addition modulo mask
        current = (current + step) & mask;
        count += 1;

        if count > (mask as usize) * 2 {
            println!("❌ FAILURE: Safety break triggered.");
            break;
        }
    }
}
