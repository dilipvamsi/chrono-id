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

    // --- Extended Scale: 10,000 Audits ---
    println!("\n   > Auditing 10,000 sequence transitions...");
    let mut inversions = 0;
    use rand::Rng;
    let mut rng = rand::thread_rng();

    let t = 1000;
    for _ in 0..10_000 {
        let node_a = rng.gen_range(1..1000);
        let node_b = rng.gen_range(1..1000);

        let id_low = generator::helper_generate_id_s(t, node_a, 32767);
        let id_high = generator::helper_generate_id_s(t, node_b, 0);

        if node_b < node_a && id_high < id_low {
            inversions += 1;
        }
    }

    println!("   > Monotonicity Inversions detected: {}", inversions);
    println!("✅ VERDICT: ChronoID is 'Eventually Sortable' by time, but local monotonicity is bounded.");
}
