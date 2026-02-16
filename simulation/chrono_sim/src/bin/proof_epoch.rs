//! # ChronoID Scenario 15: Multi-Epoch Coexistence
//!
//! This proof analyzes the behavior of ChronoID when transitioning between
//! eras (epoch shifts). Since the timestamp field is finite (33 bits),
//! absolute sorting over multiple centuries requires careful management
//! of the "Epoch Counter".
//!
//! This script demonstrates the "Sort Break" that occurs if a counter is
//! reset without an MSB increment, a key caveat for long-term systems.

use chrono_sim::generator;

fn main() {
    println!("🧪 Scenario 15: Multi-Epoch Coexistence\n");

    // --- Phase 1: The End of an Era (Epoch 1) ---
    // Simulating a generator near the 8.5 billion second saturation point.
    let t_epoch1_end = (1u64 << 33) - 2;
    let mut gen1 = generator::Generator::new();
    let id_e1 = gen1.generate_at(t_epoch1_end);

    // --- Phase 2: The New Dawn (Epoch 2) ---
    // Simulating a migration where the library base is moved forward.
    // If we simply reset the timestamp bits to 0, sorting is lost.
    let t_epoch2_start = 100;
    let mut gen2 = generator::Generator::new();
    let id_e2 = gen2.generate_at(t_epoch2_start);

    println!("   > ID (End of Epoch 1): {:x}", id_e1);
    println!("   > ID (Start of Epoch 2): {:x}", id_e2);

    // --- Architectural Audit ---
    // The ChronoID spec notes that "old-epoch IDs naturally sort before new-epoch IDs"
    // ONLY if the migration handles the most significant bits correctly.
    // Without a 65th bit (Epoch Bit), a reset results in id_e2 < id_e1.

    println!("   Checking sorting logic...");

    if id_e2 < id_e1 {
        println!("   ⚠️ NOTE: Default re-epoch without MSB (Bit 64+) increment breaks absolute sorting.");
        println!("   (Old IDs [0x{:x}] sort AFTER new IDs [0x{:x}] if counter resets).", id_e1, id_e2);
        println!("   Solution: Reserve high bits for Epoch versioning or use 128-bit variants.");
    }

    println!("\n✅ VERDICT: Epoch transition logic and sort-break risks documented.");
}
