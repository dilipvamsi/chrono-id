//! # ChronoID Generator Implementation
//!
//! This module provides the core logic for generating 64-bit and 32-bit ChronoIDs.
//! It supports Three Modes of Operation:
//! - **Mode A (Persona Rotation):** Distributed, uncoordinated safety via frequent salt rotation.
//! - **Mode B (Instance-Native):** Local instance safety via Weyl-sequence permutation of the Node ID.
//! - **Mode C (Managed Registry):** Centralized sequence safety via blocking and shard routing.
//!
//! ## Mathematical Foundations
//! - **Weyl Sequences:** Used for uniform distribution and period guarantees in Mode B and C.
//! - **Golden Ratio (Phi):** Derived multipliers ensure maximum divergence between adjacent generations.
//! - **Hyphenated Hex:** Human-readable formatting for all variants (XXXX-XXXX).

use rand::Rng;
use std::sync::atomic::{AtomicU64, Ordering};
use std::time::{SystemTime, UNIX_EPOCH};

// --- Constants ---

pub use crate::weyl::WEYL_MULTIPLIERS;

// Mode A: Chrono64s (Second Precision) Bit Split Constants
pub const TIME_BITS: u8 = 33;
pub const NODE_BITS: u8 = 16;
pub const SEQ_BITS: u8 = 15;

pub const TIME_MASK: u64 = (1 << TIME_BITS) - 1;
pub const NODE_MASK: u64 = (1 << NODE_BITS) - 1;
pub const SEQ_MASK: u64 = (1 << SEQ_BITS) - 1;

/// Approximate year of expiry for 33 bits second-precision timestamp (starting 2020).
pub const MAX_Y: u64 = 2378;

// --- Structs ---

/// Represents a generator's active "identity" in Mode A.
#[derive(Debug, Clone, Copy)]
pub struct Persona {
    pub node_id: u64,
    pub node_salt: u64,
    pub node_idx: usize,
    pub seq_offset: u64,
    pub seq_idx: usize,
    pub seq_salt: u64,
}

impl Persona {
    /// Generates a new random persona with expanded parameters.
    pub fn new_random() -> Self {
        let mut rng = rand::thread_rng();
        let pool = rng.gen::<u128>();
        Self {
            node_id: (pool as u64) & NODE_MASK,
            node_salt: (pool >> 16) as u64,
            node_idx: ((pool >> 80) as usize) % 128,
            seq_offset: ((pool >> 87) as u64) & SEQ_MASK,
            seq_idx: ((pool >> 103) as usize) % 128,
            seq_salt: (pool >> 110) as u64,
        }
    }
}

/// Supported time precisions for the 64-bit generator.
#[derive(Debug, Clone, Copy)]
pub enum Precision {
    /// Second precision (2020 - 2378).
    Second,
    /// Microsecond precision (2020 - 2305).
    Microsecond,
}

/// Operational modes as defined in the ChronoID specification.
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Mode {
    /// Autonomous Persona (Default). High salt rotation for stateless safety.
    A,
    /// Instance-Native (Stateful). Uses Weyl-sequences for local gapless safety.
    B,
    /// Managed Registry. Centralized assignment and burst blocking.
    C,
}

/// Configuration for the ID generator, defining bit-widths and time units.
#[derive(Debug, Clone, Copy)]
pub struct Config {
    pub time_bits: u8,
    pub node_bits: u8,
    pub seq_bits: u8,
    pub unit_us: u64,
}

impl Config {
    /// Creates a standard configuration for the given precision.
    pub fn new(precision: Precision) -> Self {
        match precision {
            Precision::Second => Self {
                time_bits: 33,
                node_bits: 16,
                seq_bits: 15,
                unit_us: 1_000_000,
            },
            Precision::Microsecond => Self {
                // chrono64us layout
                time_bits: 53,
                node_bits: 5,
                seq_bits: 5,
                unit_us: 1,
            },
        }
    }
}

/// The stateful ChronoID Generator.
pub struct Generator {
    persona: Persona,
    last_ts: AtomicU64,
    sequence: AtomicU64,
    mode: Mode,
    config: Config,
}

impl Generator {
    /// Creates a new default Generator (Mode A, Second precision).
    pub fn new() -> Self {
        let config = Config::new(Precision::Second);
        Self {
            persona: Persona::new_random(),
            last_ts: AtomicU64::new(0),
            sequence: AtomicU64::new(0),
            mode: Mode::A,
            config,
        }
    }

    /// Creates a new Generator with a specific starting identity.
    pub fn new_with_persona(persona: Persona, start_seq: u64) -> Self {
        let config = Config::new(Precision::Second);
        Self {
            persona,
            last_ts: AtomicU64::new(0),
            sequence: AtomicU64::new(start_seq),
            mode: Mode::A,
            config,
        }
    }

    /// Creates a new Generator configured for Mode B (Postgres-style).
    pub fn new_mode_b() -> Self {
        let config = Config::new(Precision::Second);
        Self {
            persona: Persona::new_random(),
            last_ts: AtomicU64::new(0),
            sequence: AtomicU64::new(0),
            mode: Mode::B,
            config,
        }
    }

    /// Creates a new Generator configured for Mode C (Managed Registry).
    pub fn new_mode_c(shard_id: u64) -> Self {
        let mut p = Persona::new_random();
        p.node_id = shard_id;

        let config = Config::new(Precision::Second);
        Self {
            persona: p,
            last_ts: AtomicU64::new(0),
            sequence: AtomicU64::new(0),
            mode: Mode::C,
            config,
        }
    }

    /// Creates a new Generator configured for Mode C with Microsecond precision.
    pub fn new_mode_c_us(shard_id: u64) -> Self {
        let mut p = Persona::new_random();
        p.node_id = shard_id;

        let config = Config::new(Precision::Microsecond);
        Self {
            persona: p,
            last_ts: AtomicU64::new(0),
            sequence: AtomicU64::new(0),
            mode: Mode::C,
            config,
        }
    }

    /// Generates a new 64-bit ChronoID based on the current system time.
    pub fn generate(&mut self) -> u64 {
        let now_us = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_micros() as u64;

        let epoch_us = 1577836800 * 1_000_000;
        let elapsed_us = now_us.saturating_sub(epoch_us);
        let ts_units = elapsed_us / self.config.unit_us;

        let last = self.last_ts.load(Ordering::Relaxed);
        let mut seq = self.sequence.load(Ordering::Relaxed);
        let seq_mask = (1 << self.config.seq_bits) - 1;

        if ts_units > last {
            self.last_ts.store(ts_units, Ordering::Relaxed);
            // Reset sequence to 0; persona.seq_offset provides the randomness
            seq = 0;
            self.sequence.store(seq, Ordering::Relaxed);
        } else if ts_units == last {
            seq = (seq + 1) & seq_mask;
            self.sequence.store(seq, Ordering::Relaxed);

            if seq == 0 {
                match self.mode {
                    Mode::A => self.rotate_persona(),
                    Mode::B => {
                        let mask = (1 << self.config.node_bits) - 1;
                        let magic = 3535253579;
                        let step = (magic & mask) | 1;
                        self.persona.node_id = (self.persona.node_id + step) & mask;
                    }
                    Mode::C => {
                        let ts_start = self.last_ts.load(Ordering::Relaxed);
                        loop {
                            let now_us = SystemTime::now()
                                .duration_since(UNIX_EPOCH)
                                .unwrap()
                                .as_micros() as u64;
                            let current_units =
                                (now_us.saturating_sub(epoch_us)) / self.config.unit_us;

                            if current_units > ts_start {
                                break;
                            }
                            std::hint::spin_loop();
                        }
                        return self.generate();
                    }
                }
            }
        } else {
            // ROLLBACK DETECTED: ts_units < last
            // Force state transition to prevent collision on reused (T, S) coordinates.
            match self.mode {
                Mode::A => self.rotate_persona(),
                Mode::B => {
                    let mask = (1 << self.config.node_bits) - 1;
                    let magic = 3535253579;
                    let step = (magic & mask) | 1;
                    self.persona.node_id = (self.persona.node_id + step) & mask;
                }
                Mode::C => {
                    // Mode C: Block until time catches up to 'last'
                    loop {
                        let now_us = SystemTime::now()
                            .duration_since(UNIX_EPOCH)
                            .unwrap()
                            .as_micros() as u64;
                        let current_units = (now_us.saturating_sub(epoch_us)) / self.config.unit_us;

                        if current_units >= last {
                            break;
                        }
                        std::hint::spin_loop();
                    }
                    return self.generate();
                }
            }
            seq = (seq + 1) & seq_mask;
            self.sequence.store(seq, Ordering::Relaxed);
        }
        self.assemble(ts_units, seq)
    }

    /// Generates a ChronoID for a specific provided timestamp unit.
    pub fn generate_at(&mut self, ts_units: u64) -> u64 {
        let last = self.last_ts.load(Ordering::Relaxed);
        let mut seq = self.sequence.load(Ordering::Relaxed);
        let seq_mask = (1 << self.config.seq_bits) - 1;

        if ts_units > last {
            self.last_ts.store(ts_units, Ordering::Relaxed);
            seq = 0;
            self.sequence.store(seq, Ordering::Relaxed);
        } else if ts_units == last {
            seq = (seq + 1) & seq_mask;
            self.sequence.store(seq, Ordering::Relaxed);
            if seq == 0 {
                match self.mode {
                    Mode::A => self.rotate_persona(),
                    Mode::B => {
                        let mask = (1 << self.config.node_bits) - 1;
                        let magic = 3535253579;
                        let step = (magic & mask) | 1;
                        self.persona.node_id = (self.persona.node_id + step) & mask;
                    }
                    Mode::C => {}
                }
            }
        } else {
            // ROLLBACK DETECTED: ts_units < last
            match self.mode {
                Mode::A => self.rotate_persona(),
                Mode::B => {
                    let mask = (1 << self.config.node_bits) - 1;
                    let magic = 3535253579;
                    let step = (magic & mask) | 1;
                    self.persona.node_id = (self.persona.node_id + step) & mask;
                }
                Mode::C => {} // generate_at is usually used for explicit non-blocking simulation
            }
            seq = (seq + 1) & seq_mask;
            self.sequence.store(seq, Ordering::Relaxed);
        }
        self.assemble(ts_units, seq)
    }

    /// Assembles the final 64-bit ID from components.
    fn assemble(&self, ts: u64, seq: u64) -> u64 {
        let node_mask = (1 << self.config.node_bits) - 1;
        let seq_mask = (1 << self.config.seq_bits) - 1;
        let time_mask = (1 << self.config.time_bits) - 1;

        let mixed_node = if self.mode == Mode::C {
            self.persona.node_id & node_mask
        } else {
            self.mix(
                self.persona.node_id,
                self.config.node_bits,
                self.persona.node_idx,
                self.persona.node_salt,
            )
        };

        let mixed_seq = self.mix(
            seq.wrapping_add(self.persona.seq_offset),
            self.config.seq_bits,
            self.persona.seq_idx,
            self.persona.seq_salt,
        );

        let mut id = (ts & time_mask) << (self.config.node_bits + self.config.seq_bits);
        id |= (mixed_node & node_mask) << self.config.seq_bits;
        id |= mixed_seq & seq_mask;

        id
    }

    /// Returns the current configuration.
    pub fn get_config(&self) -> Config {
        self.config
    }

    /// Performs the core Weyl-Golden bit mixing with specified parameters.
    pub fn mix(&self, val: u64, bits: u8, p_idx: usize, salt: u64) -> u64 {
        let mask = (1u64 << bits) - 1;
        let seed = WEYL_MULTIPLIERS[p_idx % 128];
        let mult = if bits < 64 {
            (seed >> (64 - bits)) | 1
        } else {
            seed
        };
        ((val.wrapping_mul(mult)) ^ salt) & mask
    }

    /// Rotates the entire persona.
    pub fn rotate_persona(&mut self) {
        self.persona = Persona::new_random();
    }

    /// Rotates salt and multiplier indices for both node and sequence.
    pub fn rotate_params_only(&mut self) {
        let mut rng = rand::thread_rng();
        let pool = rng.gen::<u128>();
        self.persona.node_salt = pool as u64;
        self.persona.node_idx = ((pool >> 64) as usize) % 128;
        self.persona.seq_salt = (pool >> 71) as u64;
        self.persona.seq_idx = ((pool >> 80) as usize) % 128;
        self.persona.seq_offset = ((pool >> 87) as u64) & ((1 << self.config.seq_bits) - 1);
    }
}

// --- chrono32y (Tenant ID) Implementation ---

/// Generates a 32-bit ChronoID (Year Precision).
/// Designed for Signed Integers (31-bit limit, MSB=0).
/// Layout: 8 bits Year | 23 bits Entropy.
pub fn generate_chrono32y() -> u32 {
    let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();

    let epoch_secs = 1577836800; // 2020-01-01
    let secs_since_2020 = now.as_secs().saturating_sub(epoch_secs);
    let year_offset = secs_since_2020 / 31_557_600;

    let year = (year_offset & 0xFF) as u32;

    let mut rng = rand::thread_rng();

    // 23-bit entropy ensures the ID remains positive in i32.
    let entropy = rng.gen::<u32>() & 0x7FFFFF;
    (year << 23) | entropy
}

/// Generates a 32-bit ChronoID (Year Precision).
/// Designed for Unsigned Integers (32-bit width).
/// Layout: 8 bits Year | 24 bits Entropy.
pub fn generate_uchrono32y() -> u32 {
    let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
    let epoch_secs = 1577836800;
    let secs_since_2020 = now.as_secs().saturating_sub(epoch_secs);
    let year_offset = secs_since_2020 / 31_557_600;
    let year = (year_offset & 0xFF) as u32;

    let mut rng = rand::thread_rng();
    // 24-bit entropy utilizes full range.
    let entropy = rng.gen::<u32>() & 0xFFFFFF;
    (year << 24) | entropy
}

/// Generates a stateful 32-bit ChronoID using a Weyl-Sequence.
/// Guarantees a full 24-bit cycle (16.7M unique IDs) within a single year tick.
pub fn generate_chrono32y_mode_b(sequence: &AtomicU64) -> u32 {
    let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();

    let epoch_secs = 1577836800;
    let secs_since_2020 = now.as_secs().saturating_sub(epoch_secs);
    let year_offset = secs_since_2020 / 31_557_600;
    let year = (year_offset & 0xFF) as u32;

    // Weyl Step for 24-bit space (derived from Golden Ratio).
    const STEP: u64 = 0x9E377B | 1;
    const MASK: u64 = 0xFFFFFF;

    // Atomic update of the sequence.
    let mut current = sequence.load(Ordering::Relaxed);
    loop {
        let next = (current + STEP) & MASK;
        match sequence.compare_exchange_weak(current, next, Ordering::Relaxed, Ordering::Relaxed) {
            Ok(_) => break,
            Err(x) => current = x,
        }
    }

    (year << 24) | (current as u32)
}

/// Formats a 32-bit integer into a 9-character hyphenated hex string (XXXX-XXXX).
pub fn format_hyphenated_hex(id: u32) -> String {
    let h = format!("{:08X}", id);
    format!("{}-{}", &h[0..4], &h[4..8])
}

/// Helper for simulating uchrono64us (Microsecond) at a specific time.
pub fn generate_uchrono64us_at(ts_us: u64, node_id: u64) -> u64 {
    let mut gen = Generator::new_mode_c_us(node_id);
    gen.generate_at(ts_us)
}

/// Helper for simulating uchrono64ms (Millisecond) at a specific time.
pub fn generate_uchrono64ms_at(ts_ms: u64, node_id: u64) -> u64 {
    let mut gen = Generator::new();
    gen.config = Config::new(Precision::Second); // ms is usually 1000us but let's assume ms units
                                                 // ms variant has 43/11/10 split.
    gen.config.time_bits = 43;
    gen.config.node_bits = 11;
    gen.config.seq_bits = 10;
    gen.config.unit_us = 1000;
    gen.persona.node_id = node_id;
    gen.generate_at(ts_ms)
}

/// Helper for simulating uchrono64s (Second) at a specific time.
pub fn generate_uchrono64s_at(ts_s: u64, node_id: u64) -> u64 {
    let mut gen = Generator::new();
    gen.persona.node_id = node_id;
    gen.generate_at(ts_s)
}

/// Helper for generating a raw 64-bit ID with specific components for second precision.
/// Bypasses stateful sequence logic.
pub fn helper_generate_id_s(ts: u64, node: u64, seq: u64) -> u64 {
    // T(33) | N(16) | S(15)
    (ts << 31) | (node << 15) | seq
}
