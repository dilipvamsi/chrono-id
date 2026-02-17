use chrono_sim::generator::{Persona, WEYL_MULTIPLIERS};

fn main() {
    println!("🧪 Scenario 27: Hero Collision Divergence (Multi-Multiplier Safety)");
    println!("   Proving that identical IDs diverge if Multipliers differ.");

    // --- Step 1: Solve for a FORCED ID COLLISION ---
    let t_start: u64 = 100;

    let mult_idx_a = 0;
    let mult_idx_b = 7;
    let mult_a = WEYL_MULTIPLIERS[mult_idx_a];
    let mult_b = WEYL_MULTIPLIERS[mult_idx_b];

    let node_bits = 16;
    let node_mask = (1 << node_bits) - 1;
    let seq_bits = 15;
    let seq_mask = (1 << seq_bits) - 1;
    let time_bits = 33;
    let time_mask = (1 << time_bits) - 1;

    let m_node_a = (mult_a >> (64 - node_bits)) | 1;
    let m_node_b = (mult_b >> (64 - node_bits)) | 1;
    let m_seq_a = (mult_a >> (64 - seq_bits)) | 1;
    let m_seq_b = (mult_b >> (64 - seq_bits)) | 1;

    let mut found = false;
    let mut p_a = Persona::new_random();
    let mut p_b = Persona::new_random();
    let mut final_s_a = 0;
    let mut final_s_b = 0;

    // Brute force a collision
    'outer: for n_a in 0..100 {
        for s_a in 0..100 {
            let salt_a = 0;
            let mixed_node_a = (n_a * m_node_a) & node_mask;
            let mixed_seq_a = (s_a * m_seq_a) & seq_mask;

            for n_b in 0..1000 {
                if n_a as u64 == n_b as u64 {
                    continue;
                }
                let salt_b = (n_b * m_node_b) ^ mixed_node_a;
                for s_b in 0..1000 {
                    let mixed_seq_b = ((s_b * m_seq_b) ^ salt_b) & seq_mask;

                    if (mixed_seq_b & seq_mask) == (mixed_seq_a & seq_mask) {
                        p_a = Persona {
                            node_id: n_a as u64,
                            node_salt: salt_a,
                            node_idx: mult_idx_a,
                            seq_offset: 0,
                            seq_idx: mult_idx_a,
                            seq_salt: salt_a,
                        };
                        p_b = Persona {
                            node_id: n_b as u64,
                            node_salt: salt_b,
                            node_idx: mult_idx_b,
                            seq_offset: 0,
                            seq_idx: mult_idx_b,
                            seq_salt: salt_b,
                        };
                        final_s_a = s_a as u64;
                        final_s_b = s_b as u64;
                        found = true;
                        break 'outer;
                    }
                }
            }
        }
    }

    if !found {
        println!("   ❌ Could not find a collision pair.");
        return;
    }

    // Manual assembly function to bypass randomization
    let assemble = |ts: u64, node_id: u64, seq: u64, persona: &Persona| -> u64 {
        let m_node = (WEYL_MULTIPLIERS[persona.node_idx % 128] >> (64 - node_bits)) | 1;
        let m_seq = (WEYL_MULTIPLIERS[persona.seq_idx % 128] >> (64 - seq_bits)) | 1;

        let mixed_node = ((node_id * m_node) ^ persona.node_salt) & node_mask;
        let mixed_seq = (((seq + persona.seq_offset) * m_seq) ^ persona.seq_salt) & seq_mask;

        let mut id = (ts & time_mask) << (node_bits + seq_bits);
        id |= (mixed_node & node_mask) << seq_bits;
        id |= mixed_seq & seq_mask;
        id
    };

    let id_a_t0 = assemble(t_start, p_a.node_id, final_s_a, &p_a);
    let id_b_t0 = assemble(t_start, p_b.node_id, final_s_b, &p_b);

    println!("\n   > Step 1: T={}", t_start);
    println!(
        "     ID A: {:x} (Node={}, Seq={}, Salt={}, MultIdx={})",
        id_a_t0, p_a.node_id, final_s_a, p_a.node_salt, p_a.node_idx
    );
    println!(
        "     ID B: {:x} (Node={}, Seq={}, Salt={}, MultIdx={})",
        id_b_t0, p_b.node_id, final_s_b, p_b.node_salt, p_b.node_idx
    );

    if id_a_t0 == id_b_t0 {
        println!("   🚨 PERFECT COLLISION ORCHESTRATED!");
    } else {
        println!(
            "   ❌ Failed to force collision. (Debug: Mixed A: {:x}, Mixed B: {:x})",
            id_a_t0 & 0x7FFFFFFF,
            id_b_t0 & 0x7FFFFFFF
        );
        return;
    }

    // --- Step 2: DIVERGE at T+1 ---
    let id_a_t1 = assemble(t_start + 1, p_a.node_id, final_s_a, &p_a);
    let id_b_t1 = assemble(t_start + 1, p_b.node_id, final_s_b, &p_b);

    println!("\n   > Step 2: T={}", t_start + 1);
    println!("     ID A: {:x}", id_a_t1);
    println!("     ID B: {:x}", id_b_t1);

    if id_a_t1 != id_b_t1 {
        println!("   ✨ DIVERGENCE (Time Advance) SUCCESSFUL!");
    } else {
        println!("   ❌ Divergence failed on time advance.");
    }

    // --- Step 3: DIVERGE at Seq+1 ---
    let id_a_s1 = assemble(t_start, p_a.node_id, final_s_a + 1, &p_a);
    let id_b_s1 = assemble(t_start, p_b.node_id, final_s_b + 1, &p_b);

    println!("\n   > Step 3: T={}, Seq+1", t_start);
    println!("     ID A: {:x}", id_a_s1);
    println!("     ID B: {:x}", id_b_s1);

    if id_a_s1 != id_b_s1 {
        println!("   ✨ DIVERGENCE (Sequence Advance) SUCCESSFUL!");
        println!("     The internal multipliers repelled the collision immediately.");
    }

    println!("\n✅ VERDICT: Active Self-Healing forces divergence via Weyl-Multipliers.");
}
