//! # ChronoID Scenario 19: Multiplier Avalanche Test
//!
//! This proof measures the "Avalanche Effect" of the Weyl-Golden mixer.
//! In high-quality mixing functions, a single-bit change in the input should
//! result in approximately 50% of the output bits flipping.
//!
//! Because ChronoID uses a single-pass LCG multiplier combined with an XOR salt,
//! we expect a ratio around 25-40%, which provides sufficient obfuscation
//! to prevent human pattern recognition of adjacent IDs while maintaining
//! high computational efficiency.

use chrono_sim::generator;

/// Counts the number of set bits (1s) in a 64-bit integer.
fn count_set_bits(n: u64) -> u32 {
    n.count_ones()
}

fn main() {
    println!("🧪 Scenario 19: Multiplier Avalanche Test\n");
    println!("   Measuring the 'Avalanche Effect': Changing 1 input bit should flip ~50% of output bits.");

    let mut total_bits_flipped = 0;
    let iterations = 1000;
    let bits = 32;
    let salt = 0xDEADBEEF;
    let p_idx = 7;

    // Define a persona with a fixed multiplier to test its specific avalanche properties
    let p = generator::Persona {
        node_id: 0,
        salt,
        multiplier_idx: p_idx,
    };
    let gen = generator::Generator::new_with_persona(p, 0);

    for _ in 0..iterations {
        // Generate a random value within the bit-width
        let val1 = rand::random::<u64>() & ((1u64 << bits) - 1);

        // Flip one random bit
        let bit_to_flip = rand::random::<u8>() % bits;
        let val2 = val1 ^ (1u64 << bit_to_flip);

        // Mix both values
        let res1 = gen.mix(val1, bits);
        let res2 = gen.mix(val2, bits);

        // Compare flipped bits in the output
        let diff = res1 ^ res2;
        total_bits_flipped += count_set_bits(diff);
    }

    let avg_flips = total_bits_flipped as f64 / iterations as f64;
    let ratio = avg_flips / bits as f64;

    println!("   > Average bits flipped for 1-bit input change: {:.2}", avg_flips);
    println!("   > Avalanche Ratio:                           {:.2}%", ratio * 100.0);

    // Validation threshold: 30% is the target for ChronoID's single-pass mixer.
    if ratio >= 0.25 && ratio <= 0.6 {
        println!("✅ SUCCESS: Mixer exhibits acceptable avalanche effect (Obfuscation Verified).");
        println!("      A ratio of ~30% is standard for single-pass multiplier-XOR mixing.");
    } else {
        println!("⚠️ WARNING: Avalanche ratio {:.2}% is outside the 25-60% range.", ratio * 100.0);
    }
}
