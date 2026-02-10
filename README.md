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

---

## 📂 Project Structure

- [**Python Implementation**](./implementations/python) - Class-based logic with integer inheritance.
- [**JS / TypeScript Implementation**](./implementations/js) - Node.js and Browser support with Web Crypto.
- [**PostgreSQL Extension**](./db-extensions/postgres) - Native PL/pgSQL functions for ID generation.

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

---

## 🛠 Roadmap

### Database Support
- [x] **Postgres:** Implementation via PL/pgSQL.
- [ ] **SQLite:** Implementation via Zig extension (Coming Soon).
- [ ] **ClickHouse:** Native function support.
- [ ] **DuckDB:** Portable SQL implementation.
- [ ] **MySQL:** Stored function implementation.

### Language Support
- [x] **Python**
- [ ] **Zig** (Core library + SQLite Extension)
- [ ] **Go**
- [ ] **Rust**
- [x] **JS / TypeScript**

---

## ⚖ License
MIT License. Feel free to use and contribute!
