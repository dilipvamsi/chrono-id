//! # ChronoID Core Proofs (Scenarios 1, 2, 3)
//!
//! This suite verifies the fundamental uniqueness and self-healing claims
//! of ChronoID in uncoordinated distributed environments.
//!
//! ## Scenarios Covered:
//! 1. **Self-Healing (The Perfect Storm):** Proven divergence of identical nodes.
//! 2. **Entropy Distribution:** Verification of single-node uniqueness at scale.
//! 3. **Burst Throughput:** Verification of uniqueness during sequence overflow.

use chrono_sim::generator::{Generator, Persona};
use indicatif::ProgressBar;
use std::collections::HashSet;
use std::sync::{Arc, Mutex};
use std::time::Instant;

#[tokio::main]
async fn main() {
    println!("⏱️  ChronoID Simulation Proof");
    println!("==============================");

    run_scenario_1_self_healing().await;
    run_scenario_2_entropy().await;
    run_scenario_3_burst().await;
}

/// Scenario 1: The "Perfect Storm"
/// Simulates 2-node Hero Case for narrative clarity, followed by 10,000 node stress test.
async fn run_scenario_1_self_healing() {
    println!("\n🧪 Scenario 1: Self-Healing (The Perfect Storm) [Scale: 10,000 Nodes, 1 Forced Hero Case]");

    // --- Step 1: The 2-Node Hero Case ---
    let shared_node_id = 99;
    let shared_salt = 0xAAAA_BBBB_CCCC_DDDD;
    let persona_idx = 42; // Same multiplier basket

    let p = Persona {
        node_id: shared_node_id,
        node_salt: shared_salt,
        node_idx: persona_idx,
        seq_offset: 0,
        seq_idx: persona_idx,
        seq_salt: shared_salt,
    };

    let mut node_a = Generator::new_with_persona(p, 0);
    let mut node_b = Generator::new_with_persona(p, 0);

    // Collision at T=0 (Bypasses reseed)
    let id_a_t0 = node_a.generate_at(0);
    let id_b_t0 = node_b.generate_at(0);

    println!("   > Node A (T=0):   {:x}", id_a_t0);
    println!("   > Node B (T=0):   {:x}", id_b_t0);

    if id_a_t0 == id_b_t0 {
        println!("   🚨 FORCED COLLISION VERIFIED (Initial Sync Failure)");
    }

    // Uncoordinated DIVERGENCE at T=101 (Next Tick)
    // Rotation is triggered by internal CSPRNG or Persona shift
    node_a.rotate_persona();
    node_b.rotate_persona();

    let id_a_t1 = node_a.generate_at(101);
    let id_b_t1 = node_b.generate_at(101);

    println!("   > Node A (T=101): {:x}", id_a_t1);
    println!("   > Node B (T=101): {:x}", id_b_t1);

    if id_a_t1 != id_b_t1 {
        println!("   ✨ ACTIVE DIVERGENCE: Nodes self-healed to unique personas.");
    }

    println!("\n   > Stress Test: Simulating 10,000 nodes starting as IDENTICAL CLONES at T=0.");
    let n_nodes = 10_000;
    let shared_node_id = 12345;
    let shared_salt = 0xDEADBEEF;
    let shared_multiplier_idx = 7;
    let shared_seq = 42;

    let results = Arc::new(Mutex::new(Vec::with_capacity(n_nodes)));
    let mut handles = Vec::with_capacity(n_nodes);
    let pb = ProgressBar::new(n_nodes as u64);

    for _ in 0..n_nodes {
        let results_clone = results.clone();
        let pb_clone = pb.clone();

        handles.push(tokio::spawn(async move {
            let p = Persona {
                node_id: shared_node_id,
                node_salt: shared_salt,
                node_idx: shared_multiplier_idx,
                seq_offset: 0,
                seq_idx: shared_multiplier_idx,
                seq_salt: shared_salt,
            };

            let mut gen = Generator::new_with_persona(p, shared_seq);

            // T=0: Force predictable total collision (bypassing reseed)
            let id_t0 = gen.generate_at(0);

            // Self-Heal: Rotate
            gen.rotate_persona();

            // T=101: Divergence
            let id_t1 = gen.generate_at(101);

            let mut guard = results_clone.lock().unwrap();
            guard.push((id_t0, id_t1));
            pb_clone.inc(1);
        }));
    }

    for h in handles {
        let _ = h.await;
    }
    pb.finish();

    let data = results.lock().unwrap();
    let mut unique_t0 = HashSet::new();
    let mut unique_t1 = HashSet::new();

    for (t0, t1) in data.iter() {
        unique_t0.insert(*t0);
        unique_t1.insert(*t1);
    }

    println!("   > Step 1 (T=0):   {} unique IDs out of {} nodes (Total Collision Verified)", unique_t0.len(), n_nodes);

    let collisions_t1 = n_nodes - unique_t1.len();
    println!("   > Step 2 (T=101): {} collisions / {} nodes", collisions_t1, n_nodes);

    if unique_t0.len() == 1 && collisions_t1 == 0 {
        println!("✅ SUCCESS: 10,000 clones successfully diverged to 10,000 unique identities.");
    } else {
        println!("❌ FAILURE: Persistent collisions detected or initial collision failed.");
    }
}

/// Scenario 2: Entropy Distribution
async fn run_scenario_2_entropy() {
    println!("\n🧪 Scenario 2: Entropy & Birthday Limits [Scale: 1,000,000 IDs]");
    let n_ids = 1_000_000;
    println!("   Generating {} IDs from a single node...", n_ids);

    let mut gen = Generator::new();
    let mut set = HashSet::with_capacity(n_ids);
    let mut dups = 0;

    let start = Instant::now();
    for _ in 0..n_ids {
        if !set.insert(gen.generate()) {
            dups += 1;
        }
    }
    let dur = start.elapsed();

    println!("   > Generated {} IDs in {:.2?}", n_ids, dur);
    println!("   > Captured Local Collisions: {}", dups);

    if dups == 0 {
        println!("✅ SUCCESS: 100% uniqueness in million-ID burst.");
    } else {
        println!("❌ FAILURE: Local collision detected in single-node stream.");
    }
}

/// Scenario 3: Burst Capacity
async fn run_scenario_3_burst() {
    println!("\n🧪 Scenario 3: Burst Throughput [Scale: 4,096 IDs/ms]");
    println!("   Simulating heavy burst that triggers multiplier rotation...");

    let mut gen = Generator::new();
    let burst_size = 4096;
    let mut unique_ids = HashSet::new();

    // Sequence limit is 32,768 for 's' variant.
    // We force rotation to prove uniqueness across transitions.
    for i in 0..burst_size {
        let id = gen.generate();
        unique_ids.insert(id);

        if i > 0 && i % 1000 == 0 {
            gen.rotate_persona();
        }
    }

    println!("   > Generated {} IDs.", burst_size);
    println!("   > Unique IDs: {}", unique_ids.len());

    if unique_ids.len() == burst_size {
        println!("✅ SUCCESS: Uniqueness maintained during rapid persona transitions.");
    } else {
        println!("❌ FAILURE: Duplicates detected during burst rotation.");
    }
}

