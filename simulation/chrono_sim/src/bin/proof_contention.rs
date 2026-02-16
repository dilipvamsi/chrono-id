/// # ChronoID Scenario 8: Thread Contention (Shared Mutex)
///
/// This proof verifies the thread-safety and performance of a shared ChronoID
/// generator protected by a Mutex.
///
/// ## Verification Goals:
/// 1. **Thread Safety:** Ensure multiple threads can safely share a single Generator
///    without internal state corruption.
/// 2. **Uniqueness:** Verify that 1 million IDs generated across 100 concurrent
///    threads contain zero collisions.
/// 3. **Performance:** Measure the throughput impact of Mutex locking in a
///    high-contention environment.

use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Instant;
use chrono_sim::generator::Generator;
use std::collections::HashSet;

fn main() {
    println!("🧪 Scenario 8: Thread Contention (Shared Mutex)");

    // Shared Generator behind a Mutex
    // In a real distributed system, this simulates highly concurrent workers hitting one ID service.
    let gen = Arc::new(Mutex::new(Generator::new()));

    let num_threads = 100;
    // Reduce iterations per thread to keep test fast but significant
    // 100 threads * 10,000 IDs = 1,000,000 IDs total.
    let ids_per_thread = 10_000;

    let start = Instant::now();
    let mut handles = vec![];

    println!("   > Spawning {} threads...", num_threads);

    for _ in 0..num_threads {
        let gen_clone = Arc::clone(&gen);
        handles.push(thread::spawn(move || {
            let mut local_ids = Vec::with_capacity(ids_per_thread);
            for _ in 0..ids_per_thread {
                let mut g = gen_clone.lock().unwrap();
                local_ids.push(g.generate());
            }
            local_ids
        }));
    }

    let mut all_ids = Vec::with_capacity(num_threads * ids_per_thread);
    for h in handles {
        let ids = h.join().unwrap();
        all_ids.extend(ids);
    }

    let duration = start.elapsed();
    let total_ids = all_ids.len();

    println!("   > Generated {} IDs in {:.2?}", total_ids, duration);
    println!("   > Verifying Uniqueness...");

    let unique_count: HashSet<_> = all_ids.iter().collect();
    if unique_count.len() == total_ids {
        println!("✅ SUCCESS: 100% Unique IDs under heavy contention.");
    } else {
        println!("❌ FAILURE: Collisions detected! {} unique vs {} total.", unique_count.len(), total_ids);
    }

    // Throughput Calculation
    let ids_per_sec = (total_ids as f64) / duration.as_secs_f64();
    println!("   > Throughput: {:.2} IDs/sec (with Mutex overhead)", ids_per_sec);
}
