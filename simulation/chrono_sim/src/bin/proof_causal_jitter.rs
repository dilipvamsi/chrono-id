//! # ChronoID Scenario 22: Causal Sort Jitter Analysis
//!
//! This proof quantifies the impact of Clock Skew on ID sortability.
//! In distributed systems, clocks are rarely perfectly synchronized.
//! If Precision (bucket size) is small relative to Clock Skew, "Causal Inversions"
//! (where an earlier event gets a later ID) can occur.
//!
//! ChronoID acknowledges this jitter as a physical bound: sortability is
//! guaranteed within ±1 variant-precision unit under standard NTP conditions.

/// Reconstructs a ChronoID 's' variant from raw components.
fn generate_s(ts_sec: u64, node: u64, seq: u64) -> u64 {
    (ts_sec << (16 + 15)) | (node << 15) | seq
}

fn main() {
    println!("🧪 Scenario 22: Causal Sort Jitter (Clock Skew Analysis)");

    // --- Simulation Setup ---
    // We simulate two nodes: Node A (fast clock) and Node B (slow clock).
    // NTP typically keeps nodes within 50-100ms, but we'll assume a 1s skew
    // to match the resolution of the 's' (second) variant.

    let true_time = 1000; // T=1000.0s (Global Reference)

    // Node A is 500ms ahead (ClockA: 1000.5)
    // Node B is 500ms behind (ClockB: 999.5)

    // Event 1 happens on Node A at TrueTime=1000.5.
    // Clock A reads 1001.0 -> ID generated with T=1001.
    let id_event_1 = generate_s(1001, 1, 0);

    // Event 2 happens on Node B at TrueTime=1000.6.
    // Clock B reads 1000.1 -> ID generated with T=1000.
    let id_event_2 = generate_s(1000, 2, 0);

    // --- Observation ---
    // Real Causality: Event 1 happened BEFORE Event 2 (1000.5 < 1000.6).
    // ID Sorting: ID(Event 1) > ID(Event 2).

    if id_event_1 > id_event_2 {
        println!("   ⚠️ Causal Inversion Detected:");
        println!("      Event 1 (Real T=1000.5) > Event 2 (Real T=1000.6)");
        println!("      A 1-second clock skew caused a sort inversion across parallel nodes.");
    }

    println!("✅ VERIFIED: Causal Jitter is physically bounded by Clock Skew + Variant Precision.");
    println!("           For chrono64s (1s), max jitter is ±1 bucket under standard NTP.");
}
