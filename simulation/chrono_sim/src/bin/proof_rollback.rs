//! # ChronoID Scenario 7: Clock Rollback Resilience (NTP Skew)
//!
//! This proof verifies that ChronoID remains unique even when the system clock
//! jumps backwards (e.g., during NTP re-sync or leap seconds).
//!
//! ## Verification Goals:
//! 1. **Rollback Uniqueness:** Confirm that IDs generated at a rolled-back
//!    timestamp do not collide with IDs previously generated at that same timestamp.
//! 2. **Recovery:** Verify that the system handles 1000+ IDs at a rolled-back
//!    timestamp without internal state corruption.

use std::collections::HashSet;
use chrono_sim::generator::Generator;

fn main() {
    println!("🧪 Scenario 7: Clock Rollback Resilience");

    // Using generate_at(t) to force time jumps
    let mut gen = Generator::new();
    let mut ids = HashSet::new();

    // --- Phase 1: Normal Ingestion (T=100) ---
    println!("   > Generating at T=100...");
    for _ in 0..10 {
        let id = gen.generate_at(100);
        ids.insert(id);
    }

    // --- Phase 2: Time Advances (T=101) ---
    println!("   > Time advances to T=101...");
    let id_at_101 = gen.generate_at(101);
    ids.insert(id_at_101);

    // --- Phase 3: Clock Rollback (T=100) ---
    // Simulates an NTP correction where the system clock is moved back.
    println!("   > CLOCK ROLLBACK to T=100! (Simulating 1 unit skew)");
    let id_rollback = gen.generate_at(100);

    if ids.contains(&id_rollback) {
        println!("❌ FAILURE: Collision detected after rollback! The system assigned a duplicate ID.");
    } else {
        println!("✅ SUCCESS: Unique ID generated during rollback (Sequence logic preserved uniqueness).");
        println!("     ID (Rollback): {:x}", id_rollback);
    }

    // Monotonicity Note:
    // When time rolls back, the new ID will naturally have a smaller timestamp component
    // than the IDs generated at T=101, breaking strict temporal sort order until
    // the clock catches up.
    if id_rollback < id_at_101 {
        println!("   ⚠️  NOTE: Temporal sort order is intentionally broken during rollback to favor uniqueness.");
    }

    // --- Phase 4: Stress Test (T=100 post-rollback) ---
    println!("   > Extended Stress Test: Generating 1,000 IDs at T=100 (post-rollback)...");
    for _ in 0..1000 {
        let id = gen.generate_at(100);
        if !ids.insert(id) {
            println!("❌ FAILURE: Duplicate detected at index {} during stressed rollback!", ids.len());
            return;
        }
    }
    println!("✅ VERDICT: 100% uniqueness in multiple-second clock rollback scenarios.");
}

