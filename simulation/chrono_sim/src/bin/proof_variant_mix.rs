//! # ChronoID Scenario 12: Variant Isolation Proof
//!
//! This proof verifies the "Variant Isolation" requirement. Different
//! precision variants (e.g., second vs. microsecond) must NEVER be mixed
//! in the same ID field/column, as their bit-masks are incompatible and
//! will result in deterministic collisions.
//!
//! ## Verification Goals:
//! 1. **Collision Proof:** Prove that a 'second' variant ID can represent
//!    the exact same bit pattern as a 'microsecond' variant ID.

/// Mock function for 's' variant bit layout.
fn generate_s(ts: u64, node: u64, seq: u64) -> u64 {
    (ts << (16 + 15)) | (node << 15) | seq
}

/// Mock function for 'ms' variant bit layout.
fn generate_ms(ts: u64, node: u64, seq: u64) -> u64 {
    (ts << (11 + 10)) | (node << 10) | seq
}

fn main() {
    println!("🧪 Scenario 12: Variant Mixing Analysis");
    println!("   Validating architectural isolation between precision families.\n");

    // Sample ID from a Microsecond generator (T=1000, N=1, S=1)
    let id_ms = generate_ms(1000, 1, 1);

    // Search for a Second-precision ID that produces the exact same 64 bits
    let mut collision_found = false;
    for n in 0..65536 {
        for s in 0..32768 {
            let id_s = generate_s(1, n, s);
            if id_s == id_ms {
                println!("   💥 COLLISION FOUND!");
                println!("   ms(T=1000ms, N=1, S=1) == s(T=1s, N={}, S={})", n, s);
                collision_found = true;
                break;
            }
        }
        if collision_found {
            break;
        }
    }

    if collision_found {
        println!("\n✅ VERIFIED: Different precision variants collide when sharing a column.");
        println!("           This proves the 'Variant Isolation' constraint (Section 11.6).");
    }
}
