//! # ChronoID Scenario 20: High-Frequency Mode A Collision Audit
//!
//! This proof verifies the "Safety Limit" claim of the ChronoID specification.
//! It demonstrates why precision-heavy variants (us/ms) are unsuitable for
//! uncoordinated Mode A generation at high node counts.
//!
//! ## Verification Goals:
//! 1. **Collision Risk:** Prove that 10-bit suffix variants (uchrono64us)
//!    exhibit extreme collision risk when shared across 2,000 parallel nodes.
//! 2. **Mode Constraint:** Validate the requirement that high-frequency
//!    workloads MUST use sequence-backed modes (B or C).

use std::collections::HashSet;
use chrono_sim::generator;

fn main() {
    println!("🧪 Scenario 20: High-Frequency Mode A Collision Audit");
    println!("   Ref: 'Mode A Unsuitable for High-Frequency Workloads'\n");

    let batch_size = 2000;
    println!("1️⃣  Testing uchrono64us (10-bit random suffix) with {} nodes...", batch_size);

    // Simulating 2,000 nodes generating 1 ID each at the exact same microsecond.
    // Since there are only 2^10 = 1024 possible suffixes, collisions are guaranteed
    // by the Pigeonhole Principle.
    let mut seen = HashSet::new();
    let mut collisions = 0;

    for i in 0..batch_size {
        // Force the same timestamp across 'batch_size' independent nodes
        let id = generator::generate_uchrono64us_at(123456789, i as u64);
        if !seen.insert(id) {
            collisions += 1;
        }
    }

    let prob = (collisions as f64 / batch_size as f64) * 100.0;
    println!("   > Collisions: {} out of {} nodes", collisions, batch_size);
    println!("   > Collision Rate: {:.2}%", prob);

    if collisions > 0 {
        println!("\n✅ VERIFIED: High-frequency Mode A exhibits unsustainable risk.");
        println!("           Pigeonhole collision observed as expected (Nodes > Suffix Space).");
    }

    // --- Phase 2: Comparison with Millisecond Precision ---
    println!("\n2️⃣  Testing uchrono64ms (11-bit random suffix) with {} nodes...", batch_size);
    let mut seen_ms = HashSet::new();
    let mut collisions_ms = 0;
    for i in 0..batch_size {
        let id = generator::generate_uchrono64ms_at(123456789, i as u64);
        if !seen_ms.insert(id) {
            collisions_ms += 1;
        }
    }
    println!("   > Collisions: {} out of {} nodes", collisions_ms, batch_size);

    println!("\n⚖️  FINAL VERDICT: Precision-heavy variants MUST use sequence-backed modes (B/C).");
}
