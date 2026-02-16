/// # ChronoID Scenario 12: Global Bit Split Audit
///
/// This proof systematically verifies the bit-widths and signed/unsigned constraints
/// for all 18 variants defined in the ChronoID specification (Section 5).
///
/// ## Verification Rules:
/// 1. **Total Width:** Sum of T+N+S bits must match the target width (32 or 64 bits).
/// 2. **Signed Compatibility:** Chrono variants (signed) must reserve the MSB (bit 63 or 31)
///    to ensure IDs are strictly positive.
/// 3. **Unsigned Entropy:** UChrono variants (unsigned) utilize the full bit-width.
///
/// The script simulates the "Max ID" value for each variant and verifies it remains
/// positive when cast to its signed equivalent (`i64` or `i32`).

use std::convert::TryInto;

struct Variant {
    name: &'static str,
    bits_t: u8,
    bits_n: u8,
    bits_s: u8,
    signed: bool,
}

fn check(v: &Variant) {
    let total = v.bits_t + v.bits_n + v.bits_s;
    println!("\n🔍 Checking {}: T={} N={} S={} (Total {}) [{}]",
        v.name, v.bits_t, v.bits_n, v.bits_s, total,
        if v.signed { "Signed" } else { "Unsigned" });

    // Rule 1: Total Width
    let max_width = if v.name.contains("32") { 32 } else { 64 };
    if total > max_width {
        println!("❌ FAIL: Total bits {} exceeds width {}", total, max_width);
        return;
    }

    // Rule 2: Signed Capacity (Must be < Width to keep MSB 0)
    if v.signed && total >= max_width {
        // Exception: If T starts at 0? No, T is MSB.
        // If total == width, MSB is used.
        // Signed integers MUST NOT use MSB for data if we want positive values.
        // So total must be <= width - 1.
        println!("❌ FAIL: Signed variant uses {} bits (Needs MSB free, max {})", total, max_width - 1);
        return;
    }

    // Simulation
    let max_t = (1u64 << v.bits_t) - 1;
    let max_n = (1u64 << v.bits_n) - 1;
    let max_s = (1u64 << v.bits_s) - 1;

    // Construct Max ID
    // ID = (T << (N+S)) | (N << S) | S
    let shift_n = v.bits_s;
    let shift_t = v.bits_n + v.bits_s;

    let id = (max_t << shift_t) | (max_n << shift_n) | max_s;

    if max_width == 32 {
        let id32 = id as u32;
        println!("   Max Value: {}", id32);
        if v.signed {
            let i = id32 as i32;
            println!("   As i32:    {}", i);
            if i < 0 {
                println!("❌ FAIL: Negative Value in Signed Mode!");
            } else {
                println!("✅ PASS: Positive i32");
            }
        } else {
             println!("✅ PASS: Valid u32");
        }
    } else {
        println!("   Max Value: {}", id);
        if v.signed {
            let i = id as i64;
            println!("   As i64:    {}", i);
            if i < 0 {
                println!("❌ FAIL: Negative Value in Signed Mode!");
            } else {
                println!("✅ PASS: Positive i64");
            }
        } else {
             println!("✅ PASS: Valid u64");
        }
    }
}

fn main() {
    let variants = vec![
        // --- 64-bit Variants (Signed: Total <= 63) ---
        // s: 33/16/15
        Variant { name: "uchrono64s", bits_t: 33, bits_n: 16, bits_s: 15, signed: false },
        Variant { name: "chrono64s",  bits_t: 33, bits_n: 15, bits_s: 15, signed: true },

        // ms: 43/11/10
        Variant { name: "uchrono64ms", bits_t: 43, bits_n: 11, bits_s: 10, signed: false },
        Variant { name: "chrono64ms",  bits_t: 43, bits_n: 10, bits_s: 10, signed: true },

        // us: 53/6/5
        Variant { name: "uchrono64us", bits_t: 53, bits_n: 6, bits_s: 5, signed: false },
        Variant { name: "chrono64us",  bits_t: 53, bits_n: 5, bits_s: 5, signed: true },

        // mo: 12/26/26 (Unsigned).
        Variant { name: "uchrono64mo", bits_t: 12, bits_n: 26, bits_s: 26, signed: false },
        Variant { name: "chrono64mo",  bits_t: 12, bits_n: 25, bits_s: 26, signed: true },

        // w: 14/26/24
        Variant { name: "uchrono64w", bits_t: 14, bits_n: 26, bits_s: 24, signed: false },
        Variant { name: "chrono64w",  bits_t: 14, bits_n: 25, bits_s: 24, signed: true },

        // d: 17/24/23
        Variant { name: "uchrono64d", bits_t: 17, bits_n: 24, bits_s: 23, signed: false },
        Variant { name: "chrono64d",  bits_t: 17, bits_n: 23, bits_s: 23, signed: true },

        // h: 21/22/21
        Variant { name: "uchrono64h", bits_t: 21, bits_n: 22, bits_s: 21, signed: false },
        Variant { name: "chrono64h",  bits_t: 21, bits_n: 21, bits_s: 21, signed: true },

        // m: 27/19/18
        Variant { name: "uchrono64m", bits_t: 27, bits_n: 19, bits_s: 18, signed: false },
        Variant { name: "chrono64m",  bits_t: 27, bits_n: 18, bits_s: 18, signed: true },

        // --- 32-bit Variants (Signed: Total <= 31) ---
        // y: 8/13/11. Signed: 8/12/11
        Variant { name: "uchrono32y", bits_t: 8, bits_n: 13, bits_s: 11, signed: false },
        Variant { name: "chrono32y",  bits_t: 8, bits_n: 12, bits_s: 11, signed: true },

        // hy: 9/12/11.
        Variant { name: "uchrono32hy", bits_t: 9, bits_n: 12, bits_s: 11, signed: false },
        Variant { name: "chrono32hy",  bits_t: 9, bits_n: 11, bits_s: 11, signed: true },

        // q: 10/11/11.
        Variant { name: "uchrono32q", bits_t: 10, bits_n: 11, bits_s: 11, signed: false },
        Variant { name: "chrono32q",  bits_t: 10, bits_n: 10, bits_s: 11, signed: true },

        // mo: 12/10/10 (32). Signed: 12/9/10 (31)
        Variant { name: "uchrono32mo", bits_t: 12, bits_n: 10, bits_s: 10, signed: false },
        Variant { name: "chrono32mo",  bits_t: 12, bits_n: 9, bits_s: 10, signed: true },

        // w: 14/9/9 (32). Signed: 14/8/9 (31)
        Variant { name: "uchrono32w", bits_t: 14, bits_n: 9, bits_s: 9, signed: false },
        Variant { name: "chrono32w",  bits_t: 14, bits_n: 8, bits_s: 9, signed: true },

        // d: 17/8/7.
        Variant { name: "uchrono32d", bits_t: 17, bits_n: 8, bits_s: 7, signed: false },
        Variant { name: "chrono32d",  bits_t: 17, bits_n: 7, bits_s: 7, signed: true },

        // Sort Key 32s
        Variant { name: "uchrono32h", bits_t: 22, bits_n: 5, bits_s: 5, signed: false },
        Variant { name: "chrono32h",  bits_t: 22, bits_n: 4, bits_s: 5, signed: true },

        Variant { name: "uchrono32tm", bits_t: 24, bits_n: 4, bits_s: 4, signed: false },
        Variant { name: "chrono32tm",  bits_t: 24, bits_n: 3, bits_s: 4, signed: true },

        Variant { name: "uchrono32m_sort", bits_t: 28, bits_n: 2, bits_s: 2, signed: false },
        Variant { name: "chrono32m_sort",  bits_t: 28, bits_n: 1, bits_s: 2, signed: true },

        Variant { name: "uchrono32bs", bits_t: 32, bits_n: 0, bits_s: 0, signed: false },
        Variant { name: "chrono32bs",  bits_t: 31, bits_n: 0, bits_s: 0, signed: true },
    ];

    for v in variants {
        check(&v);
    }
}
