# Chrono-ID

**K-Sortable, Time-Ordered Unique Identifiers.**

Chrono-ID is a library of compact, lexicographically sortable identifiers that encode temporal information directly into 32-bit and 64-bit integers. It is designed as a high-performance, space-efficient alternative to UUIDs, specifically optimized for database indexing and distributed systems.

---

## 🏗 Architecture

Chrono-ID uses a bit-packing strategy to combine a high-resolution timestamp with a random entropy pool.

### The Design Philosophy
1. **K-Sortability:** By placing the timestamp in the most significant bits, IDs generated at different times are naturally ordered. This prevents B-Tree index fragmentation in databases like PostgreSQL and SQLite.
2. **Signed Safety:** For all "Signed" versions (`Chrono`), the Most Significant Bit (MSB) is forced to `0`. This ensures the ID remains a positive integer in environments that do not support unsigned types (Postgres, Java, C#, etc.).
3. **Optimized Epochs:**
   - **32-bit family** uses a custom epoch (**January 1, 2000**) to maximize longevity.
   - **64-bit family** uses the standard **Unix Epoch (January 1, 1970)** for global compatibility.

### Bit Layout Examples
*   **Chrono32 (Day):** `[0 (1b)] [Days since 2000 (18b)] [Random (13b)]`
*   **Chrono64ms (Milli):** `[0 (1b)] [Ms since 1970 (44b)] [Random (19b)]`

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

## 📂 Directory Structure

```text
chrono-id/
├── db-extensions/
│   └── postgres/
│       └── chrono_id.sql      # PL/pgSQL implementations
├── implementations/
│   └── python/
│       └── src/
│           └── chrono_id.py   # Python class-based implementation
└── ...
```

---

## 🚀 Getting Started

### Python
The Python implementation allows IDs to act exactly like standard integers while providing temporal decoding methods.

**File:** `implementations/python/src/chrono_id.py`

```python
from chrono_id import Chrono64ms, Chrono32
from datetime import datetime

# Generate a high-precision ID
order_id = Chrono64ms()
print(f"ID: {order_id}")
print(f"Created at: {order_id.get_time()}")

# Generate a space-efficient ID for a specific date
past_date = datetime(2022, 5, 15)
tenant_id = Chrono32.from_time(past_date)
```

### PostgreSQL
Directly integrate Chrono-ID into your database schema as primary key defaults.

**File:** `db-extensions/postgres/chrono_id.sql`

```sql
-- Create table with automatic Chrono-ID
CREATE TABLE users (
    id INTEGER PRIMARY KEY DEFAULT chrono32(),
    email TEXT UNIQUE
);

-- Extract time from ID for analysis without a created_at column
SELECT email, chrono32_get_time(id) as joined_date
FROM users;
```

---

## 🛠 Roadmap

### Database Support (TODO)
- [x] **Postgres:** Implementation via PL/pgSQL.
- [ ] **SQLite:** Implementation via Zig extension (Coming Soon).
- [ ] **ClickHouse:** Native function support.
- [ ] **DuckDB:** Portable SQL implementation.
- [ ] **MySQL:** Stored function implementation.

### Language Support (TODO)
- [x] **Python**
- [ ] **Zig** (Core library + SQLite Extension)
- [ ] **Go**
- [ ] **Rust**
- [ ] **JS / TypeScript**
- [ ] **Lua**
- [ ] **C / C++** (Header-only library)

---

## ⚖ License
MIT License. Feel free to use and contribute!
