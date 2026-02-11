# Chrono-ID

`Chrono-ID` is a suite of **K-Sortable identifiers** designed using "Trade-off Engineering." Every ID type balances three competing factors: **Storage Size**, **Temporal Precision**, and **Collision Resistance (Randomness)**.

---

## 🏗 Technical Specifications

Chrono-ID uses a bit-packing strategy to combine high-resolution timestamps with random entropy. All variants are K-sortable and optimized for storage and performance.

## 🏗 Visual Architecture

The following diagram illustrates the bit-packing strategy for the **Chrono64ms** variant (Millisecond Precision).

```text
 0      1                                          44 45                  63
+------+---------------------------------------------+-----------------------+
| Sign |     Timestamp (44 bits - Milliseconds)      |    Random (19 bits)   |
| (0)  |       (Epoch 1970 - Valid to 2527)          |       (Entropy)       |
+------+---------------------------------------------+-----------------------+
```

---

## 🛡️ Collision Safety (The Birthday Problem)

Chrono-ID is engineered for high-performance sorting, but like all K-sortable IDs (ULID/UUIDv7), it has a finite random pool per temporal unit.

| Variant | Random Bits | Collision Limit (1% Prob)* | Recommended Usage |
| :--- | :--- | :--- | :--- |
| **Chrono64 (Sec)** | 27b | ~1,640 IDs / second | Global SaaS User IDs |
| **Chrono64ms (Ms)** | 19b | ~102 IDs / millisecond | E-commerce Orders |
| **Chrono64us (us)** | 9b | ~3 IDs / microsecond | HFT / Micro-telemetry |
| **Chrono32w (Wk)** | 17b | ~51 IDs / week | Weekly Batch Reports |
| **Chrono32 (Day)** | 13b | ~12 IDs / day | Low-Volume Daily Logs |
| **Chrono32h (Hr)** | 10b | ~4 IDs / hour | IoT Periodic Status |
| **Chrono32m (Min)** | 4b | < 2 IDs / minute | Personal Meta-data |

> [!NOTE] 
> **Methodology:** Limits are calculated using the *Birthday Paradox* approximation: $n \approx \sqrt{2 \cdot N \cdot \ln(\frac{1}{1-p})}$.
> Switching to an **Unsigned variant** (`UChrono*`) adds 1 extra bit of entropy, increasing the collision safety limit by **~41%** (factor of $\sqrt{2}$).

> [!TIP]
> **Safety Recommendation:** If your application generates more than 100 IDs per millisecond on a single node, use **Chrono64us (Microsecond)** to spread the entropy across a wider temporal window.

> [!CAUTION]
> **Distributed Systems & 32-bit Types:**
> Using `Chrono32`, `Chrono32h`, or `Chrono32m` for purely random, decentralized ID generation is **highly discouraged** for any production traffic. These variants have small entropy pools (4-13 bits) and are primarily designed to be used with a **centralized sequencer** (like a DB auto-increment) to ensure local uniqueness.

> [!WARNING]
> **Silent Data Corruption:**
> Exceeding the *Collision Limit* within a single temporal unit (e.g., generating 200 IDs in 1ms with `Chrono64ms`) will eventually result in a collision. If your database does not have a `UNIQUE` constraint on the ID column, this could result in **silent data loss** or overwrites. Always use `UNIQUE` constraints with K-Sortable IDs.

### **Is it "Good Enough?" (Understanding the Trade-offs)**

Chrono-ID is built on **Trade-off Engineering**. It does not aim to provide the "infinite" uniqueness of a 128-bit UUID, but rather the **"Optimal Efficiency"** required for high-volume storage.

1.  **For 99% of Web Apps:** `Chrono64ms` is mathematically resilient. 100 IDs per millisecond translates to **100,000 requests per second** (on a single node). Most applications operate well below this average.
2.  **The "Micro-Burst" Reality:** While your *average* TPS might be 5k, real-world systems experience bursts. A single 32-core server executing concurrent threads, or a GC (Garbage Collection) pause that "releases" queued requests all at once, can easily generate 100+ IDs in the same millisecond. 
3.  **Horizontal Scale:** If you have 1,000 pods generating IDs, the probability of two pods hitting the same millisecond is nearly 100% at any significant load. In this "Global Sync" scenario, the 19-20 bits of entropy in `Chrono64ms` become critical.
4.  **For The 32-bit Family:** These are **Specialized IDs**. 
    *   `Chrono32` (Daily) is **not** meant for random generation at scale. It is meant for systems where you have a **Sequencer** (like a database auto-increment) and you want to pack that sequence into the random bits to gain K-Sortability and a Date-prefix in 4 bytes.
    *   If you generate 32-bit IDs randomly, they are only "good enough" for low-volume personal data or batch-processed records.

**The Verdict:** If you value **Index Performance** and **Storage Density** (saving 50%-75% space over UUIDs), Chrono-ID is the professional choice. You simply select the precision that matches your traffic.

---

### **1. The 64-Bit Family (BigInt Compatibility)**
These 8-byte identifiers are the modern replacement for auto-incrementing integers, offering massive random pools for global scale.

| Identifier      | Precision     | Time Bits | Random Bits | Max IDs / Unit | Lifespan | Epoch |
| :---            | :---          | :---      | :---        | :---           | :---      | :---  |
| **Chrono64**    | 1 Second      | 36b       | 27b         | 134 Million    | 4147 AD   | 1970  |
| **UChrono64**   | 1 Second      | 36b       | 28b         | 268 Million    | 4147 AD   | 1970  |
| **Chrono64ms**  | 1 Millisecond | 44b       | 19b         | 524,288        | 2527 AD   | 1970  |
| **UChrono64ms** | 1 Millisecond | 44b       | 20b         | 1.04 Million   | 2527 AD   | 1970  |
| **Chrono64us**  | 1 Microsecond | 54b       | 9b          | 512            | 2540 AD   | 1970  |
| **UChrono64us** | 1 Microsecond | 54b       | 10b         | 1,024          | 2540 AD   | 1970  |

#### **Detailed Use Cases:**
*   **Chrono64 — "The Global Vault":** Highest entropy (~28 bits). Ideal for public IDs and global account identifiers. Collision safe up to **~1,600 IDs / second** per region.
*   **Chrono64ms — "The Sync Standard":** The industry benchmark for web applications. Collision safe up to **~100 IDs / millisecond**, supporting peaks of 100k IDs/sec.
*   **Chrono64us — "The High-Res Streamer":** Microsecond precision for high-frequency telemetry. Supports micro-burst ordering for **CDC and event logs** where millisecond resolution is too coarse.

### **2. The 32-Bit Family (Compact Storage)**
These 4-byte identifiers save **50% storage space** compared to BigInt. Calibrated for a **200-year minimum lifespan** (Epoch 2000–2200).

| Identifier      | Precision     | Time Bits | Random Bits | Max IDs / Unit | Lifespan | Epoch |
| :---            | :---          | :---      | :---        | :---           | :---      | :---  |
| **Chrono32w**   | 1 Week        | 14b       | 17b         | 131,072        | 2314 AD   | 2000  |
| **UChrono32w**  | 1 Week        | 14b       | 18b         | 262,144        | 2314 AD   | 2000  |
| **Chrono32**    | 1 Day         | 18b       | 13b         | 8,192          | 2716 AD   | 2000  |
| **UChrono32**   | 1 Day         | 18b       | 14b         | 16,384         | 2716 AD   | 2000  |
| **Chrono32h**   | 1 Hour        | 21b       | 10b         | 1,024          | 2239 AD   | 2000  |
| **UChrono32h**  | 1 Hour        | 21b       | 11b         | 2,048          | 2239 AD   | 2000  |
| **Chrono32m**   | 1 Minute      | 27b       | 4b          | 16             | 2255 AD   | 2000  |
| **UChrono32m**  | 1 Minute      | 27b       | 5b          | 32             | 2255 AD   | 2000  |

#### **Detailed Use Cases:**
*   **Chrono32w — "The Cycle Master":** Optimized for weekly payroll and inventory batches. Massive 17-18 bit random pool supports heavy bursts of **~50 random IDs / week** or thousands of sequenced IDs.
*   **Chrono32 — "The Day Ranger":** The "sweet spot" for invoices and records. Best for low volume (**~12 random IDs / day**) or high-volume **Sequenced IDs** (e.g. daily auto-increment).
*   **Chrono32h — "The Hourly Watcher":** Perfect for IoT heartbeats and periodic background tasks. Supports **~4 IDs / hour** for low-power reporting nodes.
*   **Chrono32m — "The Low-Traffic Lite":** High-precision 4-byte ID for **Human-paced events** (personal notes, saves). Safe only for very low frequency transactions (< 2 IDs / minute).

---

## 🆚 Competitive Comparison: Chrono-ID vs. UUIDv7 & ULID

| Feature            | Chrono-ID (64-bit)            | UUIDv7                       | ULID                         |
| :----------------- | :---------------------------- | :--------------------------- | :--------------------------- |
| **Storage Size**   | **8 Bytes** (Int64)           | 16 Bytes (Binary)            | 16 Bytes (String/Binary)     |
| **Native DB Type** | `BIGINT`                      | `UUID` or `BINARY(16)`       | `CHAR(26)` or `BINARY(16)`   |
| **Indexing Speed** | **Fastest** (B-Tree Integers) | Fast (B-Tree Binary)         | Slower (B-Tree Strings)      |
| **Human Readable** | Integer (e.g., `12345...`)    | Hex (Hyphenated)             | Base32 (Crockford)           |
| **Configurability**| **High** (Bit-splits/Epochs)  | Low (Fixed RFC 4122)         | Low (Fixed Spec)             |
| **32-Bit Support** | **Yes** (4-byte variants)     | No                           | No                           |
| **Entropy (64ms)** | **19 Bits**                   | 74 Bits                      | 80 Bits                      |

### **Why Choose Chrono-ID?**
1.  **50% Storage Savings:** Moving from 16-byte UUIDs to 8-byte IDs can save terabytes in high-volume indices.
2.  **Legacy Integration:** 32-bit Chrono-IDs allow you to move from auto-increment integers without changing database schemas.
3.  **K-Sortability:** All Chrono-IDs are chronologically sortable, simplifying time-series partitioning and ordering.

---

## 🛡️ Implementation Details

### Signed vs. Unsigned (`Chrono*` vs `UChrono*`)
*   **`Chrono*` (Signed):** Recommended for **PostgreSQL, SQLite, and Java**. The library forces the first bit to `0` to prevent "Negative ID" issues in languages lacking native unsigned types.
*   **`UChrono*` (Unsigned):** Recommended for **Rust, C++, and NoSQL**. This unlocks the sign bit, doubling capacity or temporal range.

---

## 💡 Best Practices & Technical Advisory

### 1. **The "Predictability" Rule**
Chrono-ID is NOT a replacement for secure, unguessable tokens. Because the first bits represent time, an attacker can guess the prefix of an ID generated at a specific time.
*   **DO NOT** use for: Password reset tokens, session IDs, or private API keys.
*   **DO** use for: Database Primary Keys, order numbers, and public resource identifiers (if guessing is fine).

### 2. **Database Performance**
Chrono-ID is designed to be the **B-Tree's Best Friend**.
*   **Primary Key:** Always use Chrono-ID as your `PRIMARY KEY`. Its K-Sortability ensures that new records are appended to the end of the B-Tree index, preventing "Index Fragmentation" and page splits.
*   **Partitioning:** Use the time bits for efficient range-based partitioning or sharding without needing an extra `created_at` column.

### 3. **The 32-bit Safe Mode**
If you are using a 32-bit variant (`Chrono32`) in a high-traffic environment:
*   **Sequence Injection:** Instead of passing a random value, pass a local atomic counter or a database sequence into the library.
    ```cpp
    // C++ Example: Injecting a sequence for 100% collision safety
    uint64_t my_seq = get_next_sequence();
    auto id = Chrono32::from_iso_string("2024-01-01T00:00:00Z", my_seq);
    ```

---

## 🛡️ Error Handling

Chrono-ID implements a standardized, cross-platform exception model:

1. **`ChronoError`**: A dedicated named error class is available in C++, Python, and JavaScript.
2. **Unified Messages**: All implementations throw/raise identical messages for common failures:
   - **Null Input**: `"Input string is null"`
   - **Invalid Format**: `"Invalid ISO 8601 format"`
   - **Underflow**: `"Timestamp underflow: Date is before Epoch..."`
3. **Strict Boundaries**: 32-bit types strictly reject dates before **January 1, 2000**.

---

## 🧪 Verification & Consistency

We maintain a **Single Source of Truth** for correctness:

- **JSON Test Suite**: A shared `tests/cross_platform_tests.json` file contains hundreds of valid and error cases.
- **Native Runners**: Every language implementation includes a runner that parses this JSON.
- **Monotonic High-Res (JS)**: Uses `process.hrtime.bigint()` (Node.js) and `performance.now()` (Browser) for drift-free precision.

---

## 📂 Project Structure

- [**Python Implementation**](./implementations/python) - Class-based logic with integer inheritance.
- [**JS / TypeScript Implementation**](./implementations/js) - Node.js and Browser support with Web Crypto.
- [**C++ Implementation**](./implementations/cpp) - Header-only library using `std::chrono`. [Read Documentation](./implementations/cpp/README.md)
- [**PostgreSQL Extension**](./db-extensions/postgres) - Native PL/pgSQL functions for ID generation.
- [**SQLite Extension**](./db-extensions/sqlite) - Native C++ extension for ID generation. [Read Documentation](./db-extensions/sqlite/README.md)

---

## 🚀 Getting Started

### Python
```python
from chrono_id import Chrono64ms
# Generate a high-precision ID
id = Chrono64ms()
print(f"ID: {id} | Created at: {id.get_time()}")
```

### JavaScript
```javascript
const ChronoID = require('chrono-id');
// Works in Node.js and Browsers
const id = new ChronoID.Chrono64ms();
console.log(id.toString(), id.getTime());
```

### PostgreSQL
```sql
-- Generate 10k unique IDs
SELECT chrono64ms() FROM generate_series(1, 10000);
```

### C++
```cpp
#include "chrono_id.hpp"
#include <iostream>

int main() {
    chrono_id::UChrono64us id;
    std::cout << "ID: " << id.value << std::endl;
    return 0;
}
```

---

## 🛠 Roadmap

### Database Support
- [x] **Postgres:** Implementation via PL/pgSQL.
- [x] **SQLite:** Implementation via C++ extension.
- [ ] **ClickHouse:** Native function support.
- [ ] **DuckDB:** Portable SQL implementation.
- [ ] **MySQL:** Stored function implementation.

### Language Support
- [x] **Python**
- [x] **C++** (Header only library)
- [ ] **Go**
- [ ] **Rust**
- [x] **JS / TypeScript**

---

## ⚖ License
MIT License. Feel free to use and contribute!
