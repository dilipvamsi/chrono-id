//! # ChronoID Scenario 25: Foreign Key Performance Analysis
//!
//! This proof compares `chrono32y` (32-bit Tenant ID) against 128-bit random
//! identifiers (UUID v4) and sequential identifiers (UUID v7) on physical
//! SQLite B-Trees.
//!
//! ## Verification Goals:
//! 1. **Storage Optimization:** Quantify the "Multiplication Effect" of small
//!    Foreign Keys in indexed child tables.
//! 2. **The Security Trade-off:** Explain why random tenant isolation (Weyl)
//!    intentionally trades localized sequential speed for global system safety.

use rusqlite::Connection;
use uuid::Uuid;
use chrono_sim::generator;
use std::fs;
use std::time::Instant;
use std::error::Error;
use std::sync::atomic::AtomicU64;

fn main() -> Result<(), Box<dyn Error>> {
    println!("🧪 Scenario 25: chrono32y vs UUIDv4 (Random) vs UUIDv7 (Sequential)\n");

    // Scale parameters for meaningful cache pressure analysis
    let n_records = 500_000;
    let n_queries = 20_000;

    println!("   Setup: {} Records | {} Queries", n_records, n_queries);
    println!("   Goal: Prove 32-bit Random (chrono32y) beats 128-bit Random (UUIDv4).\n");

    // --- 1. chrono32y Setup (32-bit Obfuscated) ---
    let path_32 = "perf_chrono32y.db";
    let _ = fs::remove_file(path_32);
    let conn_32 = Connection::open(path_32)?;
    conn_32.execute("PRAGMA synchronous = OFF", [])?;
    conn_32.execute("CREATE TABLE r (id INTEGER PRIMARY KEY, data TEXT) WITHOUT ROWID", [])?;
    conn_32.execute("BEGIN TRANSACTION", [])?;
    let seq = AtomicU64::new(0);
    {
        let mut stmt = conn_32.prepare("INSERT INTO r (id, data) VALUES (?, ?)")?;
        for _ in 0..n_records {
            let id = generator::generate_chrono32y_mode_b(&seq);
            stmt.execute(rusqlite::params![id as i32, ""])?;
        }
    }
    conn_32.execute("COMMIT", [])?;

    // --- 2. UUID v4 Setup (128-bit Random) ---
    let path_v4 = "perf_uuid4.db";
    let _ = fs::remove_file(path_v4);
    let conn_v4 = Connection::open(path_v4)?;
    conn_v4.execute("PRAGMA synchronous = OFF", [])?;
    conn_v4.execute("CREATE TABLE r (id BLOB PRIMARY KEY, data TEXT) WITHOUT ROWID", [])?;
    conn_v4.execute("BEGIN TRANSACTION", [])?;
    {
        let mut stmt = conn_v4.prepare("INSERT INTO r (id, data) VALUES (?, ?)")?;
        for _ in 0..n_records {
            let id = Uuid::new_v4();
            stmt.execute(rusqlite::params![id.as_bytes(), ""])?;
        }
    }
    conn_v4.execute("COMMIT", [])?;

    // --- 3. UUID v7 Setup (128-bit Sequential) ---
    let path_v7 = "perf_uuid7.db";
    let _ = fs::remove_file(path_v7);
    let conn_v7 = Connection::open(path_v7)?;
    conn_v7.execute("PRAGMA synchronous = OFF", [])?;
    conn_v7.execute("CREATE TABLE r (id BLOB PRIMARY KEY, data TEXT) WITHOUT ROWID", [])?;
    conn_v7.execute("BEGIN TRANSACTION", [])?;
    {
        let mut stmt = conn_v7.prepare("INSERT INTO r (id, data) VALUES (?, ?)")?;
        for _ in 0..n_records {
            let id = Uuid::now_v7();
            stmt.execute(rusqlite::params![id.as_bytes(), ""])?;
        }
    }
    conn_v7.execute("COMMIT", [])?;

    // --- BENCHMARK: POINT LOOKUPS ---
    println!("   📊 BENCHMARK (Point Lookups):");

    let mut keys_32 = Vec::new();
    let mut keys_v4 = Vec::new();
    let mut keys_v7 = Vec::new();

    // Extract a sample of keys to test query performance
    let mut stmt = conn_32.prepare("SELECT id FROM r LIMIT ?")?;
    let rows = stmt.query_map([n_queries], |r| Ok(r.get::<_, i32>(0)?))?;
    for r in rows { keys_32.push(r?); }

    let mut stmt = conn_v4.prepare("SELECT id FROM r LIMIT ?")?;
    let rows = stmt.query_map([n_queries], |r| Ok(r.get::<_, Vec<u8>>(0)?))?;
    for r in rows { keys_v4.push(r?); }

    let mut stmt = conn_v7.prepare("SELECT id FROM r LIMIT ?")?;
    let rows = stmt.query_map([n_queries], |r| Ok(r.get::<_, Vec<u8>>(0)?))?;
    for r in rows { keys_v7.push(r?); }

    let start_32 = Instant::now();
    for k in &keys_32 { conn_32.query_row("SELECT count(*) FROM r WHERE id = ?", [*k], |_| Ok(()))?; }
    let d_32 = start_32.elapsed();

    let start_v4 = Instant::now();
    for k in &keys_v4 { conn_v4.query_row("SELECT count(*) FROM r WHERE id = ?", [k], |_| Ok(()))?; }
    let d_v4 = start_v4.elapsed();

    let start_v7 = Instant::now();
    for k in &keys_v7 { conn_v7.query_row("SELECT count(*) FROM r WHERE id = ?", [k], |_| Ok(()))?; }
    let d_v7 = start_v7.elapsed();

    println!("   > chrono32y (Random 32):     {:?}", d_32);
    println!("   > UUID v4   (Random 128):    {:?}", d_v4);
    println!("   > UUID v7   (Sequential 128): {:?}", d_v7);

    println!("\n   🏆 ANALYSIS:");
    println!("   > speedup vs UUIDv4:  {:.2}x", d_v4.as_secs_f64() / d_32.as_secs_f64());
    println!("   > speedup vs UUIDv7:  {:.2}x", d_v7.as_secs_f64() / d_32.as_secs_f64());

    let sz_32 = fs::metadata(path_32)?.len();
    let sz_v4 = fs::metadata(path_v4)?.len();
    println!("\n   📊 STORAGE:");
    println!("   > chrono32y DB Size: {:.2} MB", sz_32 as f64 / 1_048_576.0);
    println!("   > UUID v4   DB Size: {:.2} MB", sz_v4 as f64 / 1_048_576.0);
    println!("   > Savings vs Random: {:.1}%", (1.0 - (sz_32 as f64 / sz_v4 as f64)) * 100.0);

    // Cleanup
    let _ = fs::remove_file(path_32);
    let _ = fs::remove_file(path_v4);
    let _ = fs::remove_file(path_v7);

    Ok(())
}
