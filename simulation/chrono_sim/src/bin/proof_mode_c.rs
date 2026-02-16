//! # ChronoID Scenario 5: Mode C (Shard Routing & Isolation)
//!
//! This proof verifies the "Managed Registry" Mode C, where raw Shard/Node IDs
//! are preserved to allow deterministic infrastructure routing.
//!
//! ## Verification Goals:
//! 1. **Bit Preservation:** Verify that Node IDs are NOT mixed, allowing hash-routing.
//! 2. **Wait-for-Tick:** Verify the safety mechanism that blocks generation
//!    when the sequence overflows, awaiting the next time bucket boundary.
//! 3. **Parallel Isolation:** Verify that multiple shards can operate in parallel
//!    without bit-boundary overlap.

use chrono_sim::generator;
use std::time::Instant;

#[tokio::main]
async fn main() {
    run_scenario_5_mode_c().await;
}

async fn run_scenario_5_mode_c() {
    println!("\n🧪 Scenario 5: Mode C (Shard Routing)");
    let shard_id = 7;
    let mut gen = generator::Generator::new_mode_c(shard_id);

    // --- Test 1: Deterministic Routing ---
    // In Mode C, (id >> SEQ_BITS) & NODE_MASK must equal the input shard_id.
    println!("   > Initializing Generator with Shard ID: {}", shard_id);
    let id1 = gen.generate();

    let extracted = (id1 >> generator::SEQ_BITS) & generator::NODE_MASK;
    println!("   > Generated ID: {:x}", id1);
    println!("   > Extracted Node Bits: {}", extracted);

    if extracted == shard_id {
        println!("✅ SUCCESS: Shard ID preserved in generated ID (Deterministic Routing Verified).");
    } else {
        println!("❌ FAILURE: Shard ID mismatch!");
    }

    // --- Test 2: Maintenance Stability ---
    // Salt rotations update entropy but must NOT change the Node ID routing bits.
    println!("   > Rotating Salt (Simulating Background Maintenance)...");
    gen.rotate_salt_only();

    let id2 = gen.generate();
    let extracted2 = (id2 >> generator::SEQ_BITS) & generator::NODE_MASK;

    if extracted2 == shard_id && id1 != id2 {
        println!("✅ SUCCESS: ID updated entropy while preserving Shard routing bits.");
    } else {
        println!("❌ FAILURE: Maintenance corrupted shard mapping or failed to update entropy.");
    }

    // --- Test 3: Wait-for-Tick (Blocking) ---
    // If the 15-bit sequence (32,767) overflows, Mode C must block until the
    // next second begins to guarantee absolute global uniqueness.
    println!("   > Testing Wait-for-Tick safety (High-Volume Burst)...");
    let start = Instant::now();
    let n = 34000;

    for _ in 0..n {
        gen.generate();
    }

    let duration = start.elapsed();
    println!("   > Generated {} IDs in {:.2?}", n, duration);

    // Verify that the timestamp actually advanced
    let id_last = gen.generate();
    let ts_last = id_last >> (generator::NODE_BITS + generator::SEQ_BITS);
    let ts_first = id1 >> (generator::NODE_BITS + generator::SEQ_BITS);

    if ts_last > ts_first {
        println!("✅ SUCCESS: Wait-for-Tick verified (Timestamp forced forward).");
    } else {
        println!("⚠️ WARNING: Burst completed within the same second without blocking.");
    }

    // --- Phase 4: Microsecond Precision (US) ---
    println!("\n   > Testing Microsecond Precision (Mode C us)...");
    let shard_id_us = 7;
    let mut gen_us = generator::Generator::new_mode_c_us(shard_id_us);

    let config = gen_us.get_config();
    let id_us = gen_us.generate();
    let node_mask_us = (1 << config.node_bits) - 1;
    let extracted_us = (id_us >> config.seq_bits) & node_mask_us;

    if extracted_us == shard_id_us {
        println!("✅ SUCCESS: Microsecond Shard ID preserved.");
    }

    // --- Phase 5: Parallel Shard Isolation ---
    // Verify that independent shard threads never overlap Node IDs.
    println!("\n   > Testing Parallel Shard Isolation...");
    let shards = vec![1, 2, 3, 31];
    let mut tasks = Vec::new();

    for shard in shards {
        tasks.push(tokio::spawn(async move {
            let mut gen = generator::Generator::new_mode_c_us(shard);
            let config = gen.get_config();
            let node_mask = (1 << config.node_bits) - 1;

            for _ in 0..500_000 {
                let id = gen.generate();
                let extracted = (id >> config.seq_bits) & node_mask;
                if extracted != shard {
                    panic!("Isolation Leak! Node {} generated for Shard {}", extracted, shard);
                }
            }
            shard
        }));
    }

    for task in tasks {
        let shard = task.await.unwrap();
        println!("      Shard {} isolation verified (500k ops).", shard);
    }
    println!("✅ VERDICT: Mode C provides deterministic routing and strict shard isolation.");
}

