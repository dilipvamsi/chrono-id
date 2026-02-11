# Chrono-ID

**K-Sortable, Time-Ordered Unique Identifiers.**

Chrono-ID is a library of compact, lexicographically sortable identifiers that encode temporal information directly into 32-bit and 64-bit integers. It is designed as a high-performance, space-efficient alternative to UUIDs, specifically optimized for database indexing and distributed systems.

---

## 🏗 Architecture

Chrono-ID uses a bit-packing strategy to combine a high-resolution timestamp with a random entropy pool.

### The Design Philosophy
1. **K-Sortability:** By placing the timestamp in the most significant bits, IDs generated at different times are naturally ordered. This prevents B-Tree index fragmentation.
2. **UTC Consistency:** All implementations use **UTC** internals. This avoids "Epoch Boundary" overlap bugs and ensures consistency across distributed systems.
3. **Signed Safety:** For all "Signed" versions (`Chrono`), the Most Significant Bit (MSB) is forced to `0`. This ensures the ID remains a positive integer in environments that do not support unsigned types (Postgres, Java, etc.).
4. **Optimized Epochs:**
   - **32-bit family** uses a custom epoch (**January 1, 2000**) to maximize longevity.
   - **64-bit family** uses the standard **Unix Epoch (January 1, 1970)** for global compatibility.

### Bit Layout Table

| Identifier      | Total Bits | MSB (Sign) | Time Bits  | Random Bits | Precision |
| :---            | :---       | :---       | :---       | :---        | :---      |
| **Chrono32**    | 32         | `0` (1b)   | 18b (Day)  | 13b         | Day       |
| **UChrono32**   | 32         | -          | 18b (Day)  | 14b         | Day       |
| **Chrono32h**   | 32         | `0` (1b)   | 21b (Hour) | 10b         | Hour      |
| **UChrono32h**  | 32         | -          | 21b (Hour) | 11b         | Hour      |
| **Chrono32m**   | 32         | `0` (1b)   | 27b (Min)  | 4b          | Minute    |
| **UChrono32m**  | 32         | -          | 27b (Min)  | 5b          | Minute    |
| **Chrono64**    | 64         | `0` (1b)   | 36b (Sec)  | 27b         | Second    |
| **UChrono64**   | 64         | -          | 36b (Sec)  | 28b         | Second    |
| **Chrono64ms**  | 64         | `0` (1b)   | 44b (Ms)   | 19b         | Milli     |
| **UChrono64ms** | 64         | -          | 44b (Ms)   | 20b         | Milli     |
| **Chrono64us**  | 64         | `0` (1b)   | 54b (us)   | 9b          | Micro     |
| **UChrono64us** | 64         | -          | 54b (us)   | 10b         | Micro     |

---

## ⏳ Longevity & Capacity

| Identifier     | Type     | Precision | Max IDs      | Last Year | Epoch |
| :---           | :---     | :---      | :---         | :---      | :---  |
| **Chrono32**   | Signed   | Day       | 8,192 / Day  | **2717**  | 2000  |
| **UChrono32**  | Unsigned | Day       | 16,384 / Day | **2717**  | 2000  |
| **Chrono32h**  | Signed   | Hour      | 1,024 / Hour | **2239**  | 2000  |
| **UChrono32h** | Unsigned | Hour      | 2,048 / Hour | **2239**  | 2000  |
| **Chrono32m**  | Signed   | Minute    | 16 / Minute  | **2255**  | 2000  |
| **UChrono32m** | Unsigned | Minute    | 32 / Minute  | **2255**  | 2000  |
| **Chrono64**   | Signed   | Second    | 134M / Sec   | **4147**  | 1970  |
| **UChrono64**  | Unsigned | Second    | 268M / Sec   | **4147**  | 1970  |
| **Chrono64ms** | Signed   | Milli     | 524k / Ms    | **2527**  | 1970  |
| **UChrono64ms**| Unsigned | Milli     | 1M / Ms      | **2527**  | 1970  |
| **Chrono64us** | Signed   | Micro     | 512 / us     | **2540**  | 1970  |
| **UChrono64us**| Unsigned | Micro     | 1,024 / us   | **2540**  | 1970  |

---

## 📈 Log Stream Analysis

Chrono-ID is exceptionally well-suited for high-throughput log streams due to its **K-Sortable** nature and compact binary representation.

### Why it works:
1. **Append-Only Performance:** Being naturally ordered by time, IDs are always inserted at the end of B-Tree indexes. This prevents random I/O and expensive page splits common with standard UUIDs.
2. **Compact Storage:** A 64-bit integer significantly reduces index volume compared to 128-bit UUIDs or string-based ISO timestamps, leading to better cache utilization.
3. **Implicit Time Indexing:** Range queries can be performed directly on the ID (`ID >= target_start AND ID <= target_end`), often eliminating the need for a separate `created_at` index.

### Precision vs. Entropy:
- **Chrono64us (Precision First):** Provides microsecond granularity, which is ideal for high-precision event ordering on a single node or small clusters. By dedicating 54 bits to time, it leaves 9-10 bits for random entropy (512-1024 IDs/us). This is perfect for high-resolution logging where sub-millisecond sequencing is the priority.
- **Chrono64ms (Scale First):** Specifically designed for massive distributed deployments. By using millisecond precision, it frees up **19-20 bits of entropy** (over 1 million unique IDs per millisecond). This makes it practically immune to global collisions in large-scale server fleets across thousands of nodes, while still maintaining excellent K-sortability for database indexing.

---

## 🎯 Selection Guide

| Scenario | Recommended ID | Rationale |
| :--- | :--- | :--- |
| **Distributed Systems** | `Chrono64ms` | High entropy (20-bit) minimizes global collision risk across many nodes. |
| **High-Precision Logging**| `Chrono64us` | Microsecond precision for perfect event sequencing on a single node. |
| **Standard DB Identity** | `Chrono64` | Balanced longevity (4147 AD) and ordering for primary keys. |
| **Mobile/Low-Bandwidth** | `Chrono32` | 4-byte footprint saves bandwidth and storage while remaining time-ordered. |
| **Hourly/Batch Tasks**  | `Chrono32h` | Efficiently group IDs by hour with zero looking up. |
| **Postgres / Java**      | `Chrono*` (Signed) | Use "Chrono" versions to avoid MSB issues in signed-only environments. |

---

## 📖 Use Case Examples

| Identifier | Real-World Use Case | Why? |
| :--- | :--- | :--- |
| **Chrono32** | **Daily Batch IDs** | IDs generated per-batch per-day; only 4 bytes needed. |
| **Chrono32h** | **IoT Sensor Data** | Hourly readings for millions of sensors; compact storage is vital. |
| **Chrono32m** | **Blog Comments** | Sorted chronologically; minute-level precision is sufficient. |
| **Chrono64** | **Database Primary Keys**| Standard choice for production tables; lasts over 2000 years. |
| **Chrono64ms**| **Distributed Messaging**| High uniqueness required across many global server regions. |
| **Chrono64us**| **Financial Transactions**| High-frequency events where microsecond ordering is critical. |

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
