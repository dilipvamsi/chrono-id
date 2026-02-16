//! # ChronoID Deep Visualization Simulation (Refined)
//!
//! This binary performs empirical simulations to generate data for:
//! 1. Mode A Active Divergence (Distributed collision avoidance under node ID conflict)
//! 2. Mode B Throughput Cliff (Measured SQLite insert latency over millions of rows)

use std::fs::File;
use std::io::Write;
use std::collections::HashSet;
use std::time::Instant;
use rand::Rng;
use rusqlite::{Connection, params};
use chrono_sim::{generator::{Generator, Mode}, Snowflake};
use uuid::Uuid;

fn main() {
    println!("🚀 Starting Formal Empirical Simulations...");

    // Create data directory
    std::fs::create_dir_all("simulation/data").unwrap();

    simulate_active_divergence();
    simulate_throughput_cliff();
    simulate_storage_footprint();

    println!("✅ Formal simulation data generated in simulation/data/");
    
    simulate_routing_efficiency();
}

fn simulate_routing_efficiency() {
    println!("   > Simulating Shard Routing Efficiency (Graph E)...");
    let mut file_e = File::create("simulation/data/routing_efficiency.csv").unwrap();
    writeln!(file_e, "scale,type,time_ms").unwrap();

    let n_empirical = 10_000_000;
    let mut ids = Vec::with_capacity(n_empirical);
    for i in 0..n_empirical {
        let ts = 1600000000 + (i as u64 / 1000);
        let shard = (i % 4096) as u64; 
        let seq = (i % 32768) as u64;
        let id = (ts << 24) | (shard << 12) | seq;
        ids.push(id);
    }

    let mut routing_table = std::collections::HashMap::new();
    for i in 0..4096 {
        routing_table.insert(i as u64, i as u64);
    }

    // 1. Empirical Measurement at 10M
    // Map Lookup
    let start_map = Instant::now();
    let mut _sum = 0u64;
    for id in &ids {
        let logical_key = (*id >> 12) & 0xFFF;
        if let Some(shard) = routing_table.get(&logical_key) {
            _sum += *shard;
        }
    }
    let d_map_10m = start_map.elapsed().as_secs_f64() * 1000.0;

    // Bit-Shift
    let start_shift = Instant::now();
    let mut _sum2 = 0u64;
    for id in &ids {
        let shard = (*id >> 12) & 0xFFF;
        _sum2 += shard;
    }
    let d_shift_10m = start_shift.elapsed().as_secs_f64() * 1000.0;

    // 2. Extrapolate to 1M, 100M, 1B
    let scales = vec![1_000_000.0, 100_000_000.0, 1_000_000_000.0];
    let names = vec!["1M", "100M", "1B"];
    
    for (i, &scale) in scales.iter().enumerate() {
        let factor = scale / n_empirical as f64;
        writeln!(file_e, "{},Map Lookup,{:.3}", names[i], d_map_10m * factor).unwrap();
        writeln!(file_e, "{},Bit-Shift,{:.3}", names[i], d_shift_10m * factor).unwrap();
    }
}

/// Graph A: Entropy Decay / Active Divergence
///
/// We simulate a "Node ID Collision" scenario.
/// If 10,000 uncoordinated nodes are started, they might pick from a limited pool of Node IDs.
/// In a standard Snowflake system, sharing a Node ID = 100% collision chance on same timestamp.
/// In ChronoID, the Persona (Multipliers/Salts) should provide "Active Divergence".
fn simulate_active_divergence() {
    println!("   > Simulating Distributed Active Divergence (Snowflake vs ChronoID)...");
    let mut file = File::create("simulation/data/entropy_decay.csv").unwrap();
    writeln!(file, "ids,standard_distributed,chronoid").unwrap();

    let n_points = vec![10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000];
    let node_pool_size = 1000; // Small pool (1k) to force node ID conflicts in uncoordinated environments
    let ts_ms = 1600000000000; // Fixed timestamp for distributed collision check

    for &k in &n_points {
        let mut snowflake_collisions = 0;
        let mut chrono_collisions = 0;
        let mut rng = rand::thread_rng();

        // 1. Formal Snowflake
        // If two nodes pick the same Machine ID (10 bits) and generate at the same millisecond.
        let mut seen_ids_sn = HashSet::new();
        for _ in 0..k {
            let node_id = rng.gen_range(0..node_pool_size) as u16;
            let mut snow = Snowflake::new(node_id);
            // Sequence is 0 (uncoordinated burst start)
            let id = snow.generate_at(ts_ms, 0);
            if !seen_ids_sn.insert(id) {
                snowflake_collisions += 1;
            }
        }

        // 2. ChronoID (Mode A)
        // Even if Node IDs collide, different salts/multipliers (Persona) provide divergence.
        let mut seen_ids_ch = HashSet::new();
        for _ in 0..k {
            let node_id = rng.gen_range(0..node_pool_size);
            // Each node generates its Persona on start
            let ts_s = ts_ms / 1000;
            let id = chrono_sim::generator::generate_uchrono64s_at(ts_s, node_id as u64);
            if !seen_ids_ch.insert(id) {
                chrono_collisions += 1;
            }
        }

        // Percentage of IDs that collided
        let prob_standard = (snowflake_collisions as f64 / k as f64) * 100.0;
        let prob_chrono = (chrono_collisions as f64 / k as f64) * 100.0;

        writeln!(file, "{},{},{}", k, prob_standard, prob_chrono).unwrap();
    }
}

/// Graph B: Throughput Cliff
fn simulate_throughput_cliff() {
    println!("   > Simulating Mode B Throughput Cliff (Measured SQLite Latency)...");
    let mut file = File::create("simulation/data/throughput_cliff.csv").unwrap();
    writeln!(file, "rows,uuid,chronoid").unwrap();

    let conn_v7 = Connection::open_in_memory().unwrap();
    let conn_chrono = Connection::open_in_memory().unwrap();

    // Use execute_batch to avoid panics on results from PRAGMAs
    conn_v7.execute_batch("PRAGMA journal_mode = OFF;").unwrap();
    conn_chrono.execute_batch("PRAGMA journal_mode = OFF;").unwrap();

    conn_v7.execute("CREATE TABLE r (id BLOB PRIMARY KEY, data TEXT)", []).unwrap();
    conn_chrono.execute("CREATE TABLE r (id INTEGER PRIMARY KEY, data TEXT)", []).unwrap();

    let batch_size = 200_000;
    let total_batches = 10; // 2 Million rows total for speed

    let mut gen = Generator::new_mode_b();

    for b in 1..=total_batches {
        let current_rows = b * batch_size;

        // Benchmark UUIDv7 (Time-ordered 128-bit)
        let start_v7 = Instant::now();
        conn_v7.execute_batch("BEGIN TRANSACTION;").unwrap();
        {
            let mut stmt = conn_v7.prepare_cached("INSERT INTO r (id, data) VALUES (?, ?)").unwrap();
            for _ in 0..batch_size {
                let id = Uuid::now_v7();
                stmt.execute(params![id.as_bytes(), "data"]).unwrap();
            }
        }
        conn_v7.execute_batch("COMMIT;").unwrap();
        let dur_v7 = start_v7.elapsed().as_millis();

        // Benchmark ChronoID (Sequential-leading 64-bit)
        let start_chrono = Instant::now();
        conn_chrono.execute_batch("BEGIN TRANSACTION;").unwrap();
        {
            let mut stmt = conn_chrono.prepare_cached("INSERT INTO r (id, data) VALUES (?, ?)").unwrap();
            for _ in 0..batch_size {
                let id = gen.generate();
                stmt.execute(params![id as i64, "data"]).unwrap();
            }
        }
        conn_chrono.execute_batch("COMMIT;").unwrap();
        let dur_chrono = start_chrono.elapsed().as_millis();

        println!("     Batch {}: UUIDv7={:?}ms, Chrono={:?}ms", b, dur_v7, dur_chrono);
        writeln!(file, "{},{},{}", current_rows, dur_v7, dur_chrono).unwrap();
    }
}

/// Helper to get SQLite index size in bytes
fn get_index_size(conn: &Connection) -> i64 {
    let page_count: i64 = conn.query_row("PRAGMA page_count", [], |r| r.get(0)).unwrap();
    let page_size: i64 = conn.query_row("PRAGMA page_size", [], |r| r.get(0)).unwrap();
    page_count * page_size
}

/// Graph C & D: Storage Footprint
fn simulate_storage_footprint() {
    println!("   > Simulating Storage Footprint (Index Density)...");

    // Graph C: 100M rows (Extrapolated from 1M Row Empirical Measurement)
    let mut file_c = File::create("simulation/data/storage_footprint.csv").unwrap();
    writeln!(file_c, "type,size_gb").unwrap();

    // Graph D: Max 24-bit Capacity (16,777,216 rows) empirical measurement scaled
    let mut file_d = File::create("simulation/data/storage_tenant.csv").unwrap();
    writeln!(file_d, "type,size_mb").unwrap();

    let n_empirical = 100_000; // Smaller sample for faster simulation, stable enough for linear scaling
    let n_empirical = 200_000;

    // 💥 Strategy: Isolate ID storage by using empty payloads and WITHOUT ROWID.
    // We remove the redundant secondary index and focus on the Primary B-Tree.

    // 1. UUIDv7 (128-bit BLOB)
    let size_v7 = {
        let conn = Connection::open_in_memory().unwrap();
        conn.execute_batch("PRAGMA journal_mode = OFF;
             CREATE TABLE r (id BLOB PRIMARY KEY, d TEXT) WITHOUT ROWID;").unwrap();
        let mut stmt = conn.prepare("INSERT INTO r VALUES (?, '')").unwrap();
        for _ in 0..n_empirical {
            stmt.execute(params![Uuid::now_v7().as_bytes()]).unwrap();
        }
        get_index_size(&conn)
    };

    // 2. Snowflake (64-bit INTEGER)
    let size_snow = {
        let conn = Connection::open_in_memory().unwrap();
        conn.execute_batch("PRAGMA journal_mode = OFF;
             CREATE TABLE r (id INTEGER PRIMARY KEY, d TEXT) WITHOUT ROWID;").unwrap();
        let mut stmt = conn.prepare("INSERT INTO r VALUES (?, '')").unwrap();
        let mut snow = Snowflake::new(1);
        for i in 0..n_empirical {
            stmt.execute(params![snow.generate_at(1600000000000 + i, 0) as i64]).unwrap();
        }
        get_index_size(&conn)
    };

    // 3. ChronoID 64-bit (Mode B)
    let size_chrono64 = {
        let conn = Connection::open_in_memory().unwrap();
        conn.execute_batch("PRAGMA journal_mode = OFF;
             CREATE TABLE r (id INTEGER PRIMARY KEY, d TEXT) WITHOUT ROWID;").unwrap();
        let mut stmt = conn.prepare("INSERT INTO r VALUES (?, '')").unwrap();
        let mut gen = Generator::new_mode_b();
        for _ in 0..n_empirical {
            stmt.execute(params![gen.generate() as i64]).unwrap();
        }
        get_index_size(&conn)
    };

    // 4. uchrono32y (32-bit INTEGER, 24-bit Entropy)
    let size_uchrono32 = {
        let conn = Connection::open_in_memory().unwrap();
        conn.execute_batch("PRAGMA journal_mode = OFF;
             CREATE TABLE r (id INTEGER PRIMARY KEY, d TEXT) WITHOUT ROWID;").unwrap();
        let mut stmt = conn.prepare("INSERT INTO r VALUES (?, '')").unwrap();
        let seq = std::sync::atomic::AtomicU64::new(0);
        for _ in 0..n_empirical {
            stmt.execute(params![chrono_sim::generator::generate_chrono32y_mode_b(&seq) as i64]).unwrap();
        }
        get_index_size(&conn)
    };

    // Scale to requested targets
    let gb = 1024.0 * 1024.0 * 1024.0;
    let mb = 1024.0 * 1024.0;

    // Graph C: 100M Rows (Extrapolation)
    let scale_c = 100_000_000.0 / n_empirical as f64;
    writeln!(file_c, "UUIDv7,{:.2}", (size_v7 as f64 * scale_c) / gb).unwrap();
    writeln!(file_c, "Snowflake,{:.2}", (size_snow as f64 * scale_c) / gb).unwrap();
    writeln!(file_c, "ChronoID,{:.2}", (size_chrono64 as f64 * scale_c) / gb).unwrap();

    // Graph D: Max 24-bit (16.7M Rows) Extrapolation
    let scale_d = 16_777_216.0 / n_empirical as f64;
    writeln!(file_d, "UUIDv7,{:.2}", (size_v7 as f64 * scale_d) / mb).unwrap();
    writeln!(file_d, "Snowflake,{:.2}", (size_snow as f64 * scale_d) / mb).unwrap();
    writeln!(file_d, "uchrono32y,{:.2}", (size_uchrono32 as f64 * scale_d) / mb).unwrap();
}
