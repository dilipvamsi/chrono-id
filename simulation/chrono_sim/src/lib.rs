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
