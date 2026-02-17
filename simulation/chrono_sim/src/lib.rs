//! # ChronoID Simulation Suite
//!
//! This crate contains the reference implementation and empirical verification
//! suite for the ChronoID specification.
//!
//! ## Core Components
//! - [`generator`]: Core implementation of 64-bit and 32-bit ID generation logic.
//!
//! ## Verification Suite
//! The simulation binaries in `src/bin` verify the following claims:
//! - **Self-Healing:** Rapid divergence of collided uncoordinated nodes.
//! - **Uniqueness:** Birthday Paradox safety at scale.
//! - **Burst Safety:** Monotonicity and uniqueness during high-frequency ingestion.
//! - **Compatibility:** Bit-compliance with signed-integer constraints.

pub mod generator;
pub mod weyl;

/// Formal implementation of a standard Snowflake ID for benchmarking.
/// Layout: [1 Reserved] [41 Timestamp (ms)] [10 Machine ID] [12 Sequence]
pub struct Snowflake {
    epoch_ms: u64,
    node_id: u16,
    _sequence: u16,
}

impl Snowflake {
    pub fn new(node_id: u16) -> Self {
        Self {
            epoch_ms: 1577836800000,  // 2020-01-01 in millis
            node_id: node_id & 0x3FF, // 10 bits
            _sequence: 0,
        }
    }

    /// Generates a standard 64-bit Snowflake ID at a specific timestamp.
    /// This is used to simulate distributed collision risks.
    pub fn generate_at(&mut self, now_ms: u64, sequence_inject: u16) -> u64 {
        let elapsed = now_ms.saturating_sub(self.epoch_ms);
        let mut id = (elapsed & 0x1FFFFFFFFFF) << 22; // 41 bits
        id |= (u64::from(self.node_id)) << 12; // 10 bits
        id |= u64::from(sequence_inject & 0xFFF); // 12 bits
        id
    }
}
