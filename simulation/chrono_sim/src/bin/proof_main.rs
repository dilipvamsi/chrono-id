//! # ChronoID Core Proofs (Scenarios 1, 2, 3)
//!
//! This suite verifies the fundamental uniqueness and self-healing claims
//! of ChronoID in uncoordinated distributed environments.
//!
//! ## Scenarios Covered:
//! 1. **Self-Healing (The Perfect Storm):** Proven divergence of identical nodes.
//! 2. **Entropy Distribution:** Verification of single-node uniqueness at scale.
//! 3. **Burst Throughput:** Verification of uniqueness during sequence overflow.

use chrono_sim::generator::{self, Generator, Persona};
use indicatif::ProgressBar;
use rand::Rng;
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
/// Simulates 10,000 independent nodes starting with IDENTICAL state.
/// We prove that they diverge immediately upon the first persona rotation.
async fn run_scenario_1_self_healing() {
    println!("\n🧪 Scenario 1: Self-Healing (The Perfect Storm)");
    println!("   Simulating 10,000 nodes starting with IDENTICAL NodeID/Salt at T=0.");

    let n_nodes = 10_000;
    let shared_node_id = 12345;
    let shared_salt = 0xDEADBEEF;

    let results = Arc::new(Mutex::new(Vec::with_capacity(n_nodes)));
    let mut handles = Vec::with_capacity(n_nodes);

    println!("   > Spawning {} concurrent generation tasks...", n_nodes);
    let pb = ProgressBar::new(n_nodes as u64);

    for _ in 0..n_nodes {
        let results_clone = results.clone();
        let pb_clone = pb.clone();

        handles.push(tokio::spawn(async move {
            let mut rng = rand::thread_rng();

            // Each node independently picks its first multiplier persona.
            // This is the core 'Anti-Gravity' mechanism: random divergence in shared space.
            let p = Persona {
                node_id: shared_node_id,
                salt: shared_salt,
                multiplier_idx: rng.gen_range(0..64),
            };

            let mut gen = Generator::new_with_persona(p, 0);

            // Generate first ID at T=100
            let id_t0 = gen.generate_at(100);

            // Force rotation and generate next ID at T=160
            gen.rotate_persona();
            let id_t1 = gen.generate_at(160);

            let mut guard = results_clone.lock().unwrap();
            guard.push((id_t0, id_t1));
            pb_clone.inc(1);
        }));
    }

    for h in handles {
        let _ = h.await;
    }
    pb.finish();

    // Analyze Cross-Node Collisions
    let data = results.lock().unwrap();
    let mut unique_t0 = HashSet::new();
    let mut unique_t1 = HashSet::new();

    for (t0, t1) in data.iter() {
        unique_t0.insert(*t0);
        unique_t1.insert(*t1);
    }

    let collisions_t0 = n_nodes - unique_t0.len();
    let collisions_t1 = n_nodes - unique_t1.len();

    println!("   > T=0 (Initial State): {} collisions / {} nodes", collisions_t0, n_nodes);
    println!("   > T+60s (Self-Healed): {} collisions / {} nodes", collisions_t1, n_nodes);

    if collisions_t1 == 0 {
        println!("✅ SUCCESS: System self-healed to 0 collisions via uncoordinated rotation.");
    } else {
        println!("❌ FAILURE: Collisions persisted: {}", collisions_t1);
    }
}

/// Scenario 2: Entropy Distribution
/// Verifies that the internal mixer generates 1M IDs without a single local collision.
async fn run_scenario_2_entropy() {
    println!("\n🧪 Scenario 2: Entropy & Birthday Limits");
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

/// Scenario 3: Burst Throughput
/// Verifies uniqueness when the sequence overflows within a single time bucket.
async fn run_scenario_3_burst() {
    println!("\n🧪 Scenario 3: Burst Throughput & Non-Blocking Persona Rotation");
    println!("   Simulating heavy burst that triggers multiplier rotation...");

    let mut gen = Generator::new();
    let burst_size = 1000;
    let mut unique_ids = HashSet::new();

    // Simulate a very shallow sequence limit to trigger rotation frequently
    let sim_limit = 10;

    for i in 0..burst_size {
        let id = gen.generate();
        unique_ids.insert(id);

        if i > 0 && i % sim_limit == 0 {
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

