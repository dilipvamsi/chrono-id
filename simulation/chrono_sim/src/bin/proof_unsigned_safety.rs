//! # ChronoID Scenario 11: Signed Integer Safety (i32 compatibility)
//!
//! This proof verifies the bit-budgeting required to maintain compatibility
//! with signed 32-bit integers (e.g., Java `int`, Postgres `integer`).
//!
//! ## Verification Goals:
//! 1. **Overflow Detection:** Confirm that 24-bit entropy IDs (uchrono32y)
//!    become negative in signed types at the end of the year-range.
//! 2. **Safety Proof:** Confirm that reducing entropy to 23 bits (chrono32y)
//!    guarantees a positive value (MSB=0) for the entire 255-year range.

fn main() {
    println!("🧪 Scenario 11: 23-bit Signed vs 24-bit Unsigned Safety");

    // Simulate Year 255 (The saturation point for 'y' variants)
    let year = 255u32;

    // --- Phase 1: uchrono32y (The Unsigned Trap) ---
    // 8-bit Year | 24-bit Entropy. Year 255 puts a '1' in the most significant bit.
    let ent_24 = 0xABCDEF;
    let id_unsigned = (year << 24) | ent_24;

    println!("\n[uchrono32y] Year 255, 24-bit Entropy");
    println!("Binary: {:032b}", id_unsigned);
    println!("i32:    {}", id_unsigned as i32);

    if (id_unsigned as i32) < 0 {
        println!("   ⚠️  RISK: Value is NEGATIVE in signed systems. Use u32 or bigint.");
    }

    // --- Phase 2: chrono32y (The Signed Shield) ---
    // 8-bit Year | 23-bit Entropy. Year is shifted such that MSB (bit 31) remains 0.
    let ent_23 = 0x7BCDEF;
    let id_signed = (year << 23) | ent_23;

    println!("\n[chrono32y]  Year 255, 23-bit Entropy");
    println!("Binary: {:032b}", id_signed);
    println!("i32:    {}", id_signed as i32);

    if (id_signed as i32) > 0 {
        println!("✅ SUCCESS: Positive Integer even at maximum year range.");
        println!("           ChronoID 23-bit layout fits safely in `i32` / Postgres `integer`.");
    } else {
        println!("❌ FAILURE: MSB safety bit logic broken.");
    }
}
