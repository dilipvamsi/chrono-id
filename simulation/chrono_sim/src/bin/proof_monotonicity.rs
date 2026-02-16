//! # ChronoID Scenario 24: Monotonicity Violation Audit
//!
//! This proof audits the fundamental sortability claims of ChronoID.
//! While ChronoID is "Eventually Sortable" (via the Timestamp), it is NOT
//! "Strictly Monotonic". Sequence overflows cause the Node ID to rotate,
//! which may lead to newer IDs sorting before older ones within the same time bucket.
//!
//! This script reproducing a sequence-rotation sort inversion.

use chrono_sim::generator::{self, Persona};

fn main() {
    println!("🧪 Scenario 24: Monotonicity Violation Audit");
    println!("   Proving that Sequence Overflow breaks Strict Monotonicity.");

    // --- Simulation Setup ---
    // We simulate a burst at a fixed timestamp T.
    // Persona 1 (P1) is active for indices 0..MAX.
    // Persona 2 (P2) is rotated in at MAX+1.

    let t = 1000;

    // P1: High Node ID (100)
    let p1 = Persona { node_id: 100, salt: 0, multiplier_idx: 0 };
    // P2: Low Node ID (50) - simulating a rotation to a smaller value
    let p3 = Persona { node_id: 50, salt: 0, multiplier_idx: 0 };

    // Last ID of Persona 1 (at max sequence)
    // Variant 's': S=15 bits => 32,767
    let seq_max = 32767;
    let id_a = generator::helper_generate_id_s(t, p1.node_id, seq_max);

    // First ID of Persona 2 (after sequence reset and rotation)
    let id_c = generator::helper_generate_id_s(t, p3.node_id, 0);

    println!("   ID(T, Node=100, Seq=MAX): {}", id_a);
    println!("   ID(T, Node=50,  Seq=0):   {}", id_c);

    // --- Monotonicity Check ---
    // Even though ID_C was generated AFTER ID_A, it should have a smaller absolute value
    // because its Node ID segment (50) is smaller than ID_A's (100).

    if id_c < id_a {
        println!("✅ VERIFIED: Monotonicity is NOT strictly guaranteed across sequence overflows.");
        println!("           When the sequence wraps and Node ID rotates to a lower value,");
        println!("           the new ID [C] sorts BEFORE the old ID [A].");
    } else {
        println!("❌ FAILURE: Could not reproduce monotonicity break. (Check bit shifts).");
    }

    println!("\n✅ VERDICT: ChronoID is 'Eventually Sortable' by time, but local monotonicity is bounded.");
}
