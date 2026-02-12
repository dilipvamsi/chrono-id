# Chrono-ID: Next-Generation K-Sortable Identifiers

`Chrono-ID` is a suite of **K-Sortable identifiers** designed using "Trade-off Engineering." It provides the **uncoordinated safety** of UUIDv7 with the **storage efficiency** of 64-bit integers and the **unlimited throughput** of Snowflake IDs.

> [!IMPORTANT]
> **Foundation Architecture:** Chrono-ID is built on the **"Dynamic Persona" model**. This architecture utilizes prime-based permutations and stateful rotation to provide infinite burst throughput and significantly higher collision resistance than standard ULID or Snowflake implementations.
>
> Read the full mathematical proof: [**MATHEMATICAL_PROOF.md**](./proof/MATHEMATICAL_PROOF.md).

---

## 🏗 Visual Architecture (Chrono64ms)

Unlike traditional IDs that use raw random bits, Chrono-ID divides entropy into an obfuscated **Node ID** and a permuted **Sequence**.

```text
 0      1                                          44 45          54 55          63
+------+---------------------------------------------+--------------+--------------+
| Sign |     Timestamp (44 bits - Milliseconds)      |  Node (10b)  |  Seq (9b)    |
| (0)  |       (Epoch 1970 - Valid to 2527)          | (Obfuscated) | (Permuted)   |
+------+---------------------------------------------+--------------+--------------+
                                                      \____________  ____________/
                                                                   \/
                                                   "The Persona" (Stateful Entropy)
```

### The "Persona" Transformation
Every generator maintains a state vector $\sigma$ consisting of a Node ID, specific Prime Baskets, and an XOR Salt. The final ID is assembled using:
- **Node Obfuscation:** $f_N(id) = (id \times P_{node}) \pmod{M_N}$
- **Sequence Permutation:** $f_S(c) = ((c \times P_{seq}) \pmod{M_S}) \oplus \chi_{salt}$

---

## 🚀 Key Feature: Burst Rotation (The "Emergency Valve")

Standard Snowflake implementations block or sleep if the sequence limit is reached. **Chrono-ID never blocks.**

If a traffic spike exhausts the sequence counter within a single time window:
1. The generator instantly **rotates its Persona** (picks a new random Node ID, Prime, and Salt).
2. The sequence resets, and generation continues immediately in the same time-slice.
3. This effectively unlocks the Node bits to act as overflow sequence bits, allowing throughput of **100M+ IDs/second** on a single node.

---

## 📊 Master Capacity & Safety Table

This table assumes **Uncoordinated (Random) Mode**, prioritizing maximum collision resistance (Prime Error $< 2.5\%$ and minimum 2 baskets are picked).

| Variant         | Bits | Time Bits | Node Bits | **Node Baskets** | Seq Bits | **Seq Baskets** | **Uncoordinated Risk / Entropy** | **Time Safety** | **Burst Throughput** | **Cluster Safety** |
| :-------------- | :--- | :-------- | :-------- | :--------------- | :------- | :-------------- | :------------------------------- | :-------------- | :------------------- | :----------------- |
| **UChrono64**   | 64   | 36        | 13        | 31               | 15       | 50              | **~13.6 Quadrillion**            | ~2,177 Years    | 268 Million / s      | ✅ **Safe**        |
| **Chrono64**    | 63   | 36        | 12        | 13               | 15       | 50              | **~2.8 Quadrillion**             | ~2,177 Years    | 134 Million / s      | ✅ **Safe**        |
| **UChrono64ms** | 64   | 44        | 11        | 8                | 9        | 3               | **~12.8 Billion**                | ~557 Years      | 1 Million / ms       | ✅ **Safe**        |
| **Chrono64ms**  | 63   | 44        | 10        | 6                | 9        | 3               | **~4.8 Billion**                 | ~557 Years      | 524,288 / ms         | ✅ **Safe**        |
| **UChrono64us** | 64   | 54        | 1         | 1 (Fixed)        | 9        | 3               | **~1.5 Million**                 | ~571 Years      | 1,024 / µs           | ⚠️ **Conditional** |
| **Chrono64us**  | 63   | 54        | 0         | 1 (Fixed)        | 9        | 3               | **~786,432**                     | ~571 Years      | 512 / µs             | ⚠️ **Conditional** |
| **UChrono32w**  | 32   | 14        | 11        | 8                | 7        | 2               | **~536 Million**                 | ~314 Years      | 262,144 / week       | ✅ **Safe**        |
| **Chrono32w**   | 31   | 14        | 10        | 6                | 7        | 2               | **~201 Million**                 | ~314 Years      | 131,072 / week       | ✅ **Safe**        |
| **UChrono32**   | 32   | 18        | 9         | 3                | 5        | 2               | **~3.1 Million**                 | ~717 Years      | 16,384 / day         | ⚠️ **Conditional** |
| **Chrono32**    | 31   | 18        | 8         | 2                | 5        | 2               | **~524,288**                     | ~717 Years      | 8,192 / day          | ⚠️ **Conditional** |
| **UChrono32h**  | 32   | 21        | 7         | 2                | 4        | 2               | **~131,072**                     | ~239 Years      | 2,048 / hour         | ⚠️ **Conditional** |
| **Chrono32h**   | 31   | 21        | 6         | 2                | 4        | 2               | **~65,536**                      | ~239 Years      | 1,024 / hour         | ❌ **Unsafe**      |
| **UChrono32m**  | 32   | 27        | 1         | 1 (Fixed)        | 4        | 2               | **~1,024**                       | ~255 Years      | 32 / min             | ❌ **Unsafe**      |
| **Chrono32m**   | 31   | 27        | 0         | 1 (Fixed)        | 4        | 2               | **~512**                         | ~255 Years      | 16 / min             | ❌ **Unsafe**      |

---

## 🛡️ Safety Classifications

### ✅ Safe (Cluster Ready)
*Variants: UChrono64, Chrono64, UChrono64ms, Chrono64ms, UChrono32w.*
Designed for large, uncoordinated clusters (Serverless, Kubernetes). Random Node ID assignment is safe with near-zero collision risk.

### ⚠️ Conditional (Small Clusters / Single Writer)
*Variants: Chrono64us, UChrono32, UChrono32h.*
Safe for single-writer systems or small clusters (2-10 nodes). For larger deployments, you **must** assign fixed Node IDs via configuration.

### ❌ Unsafe (Coordinated Only)
*Variants: Chrono32h, UChrono32m, Chrono32m.*
These have very low entropy. They are designed for single-node use cases (e.g., local session IDs). Distributed use requires a central coordinator.

---

## 🆚 Competitive Comparison

| Feature | **Chrono-ID** | UUIDv7 | Snowflake (Twitter) |
| :--- | :--- | :--- | :--- |
| **Storage Size** | **8 Bytes** (Int64) | 16 Bytes (Binary) | 8 Bytes (Int64) |
| **Coordination** | **None (Self-Healing)** | None | **Required** (Zookeeper/Etcd) |
| **Burst Capacity** | **Infinite (Rotation)** | N/A | **Capped** (Blocking) |
| **Public Safety** | **Obfuscated** | Readable | Exposed Counter |
| **Native DB Type** | `BIGINT` | `UUID` | `BIGINT` |

---

## 💡 Best Practices

### 1. **Generator Lifecycle (Singleton Pattern)**
Because Chrono-ID is now **stateful** (it maintains a "Persona" and a sequence counter), you should **not** instantiate a new generator for every ID.
- **Recommendation:** Create a single, long-lived instance of the generator for your application process and reuse it.
- **Why:** Re-instantiating every time forces unnecessary re-calculation of primes and salts, and risks sequence collisions if the system clock has low resolution.

### 2. **Handling Sequentiality vs. Guessability**
Chrono-ID is "Topology Hiding." This means that while IDs are K-sortable (they increase over time), they are **not numerically sequential** (ID `N+1` is not the next ID).
- **Security:** This prevents "ID Scraping" (where a competitor guesses your total order volume by incrementing an ID).
- **Important:** These are still time-prefixed. If an attacker knows a resource was created at exactly `12:00:01`, they can narrow down the potential ID range. For sensitive session tokens, always use a full 128-bit CSPRNG.

### 3. **Database Indexing & Sharding**
- **Clustered Indexes:** Always use Chrono-ID as your `PRIMARY KEY`. The time-leading nature ensures that the B-Tree index is "Append-Only," preventing expensive page splits and fragmentation.
- **Locality:** Since the first 36-44 bits are time, IDs created in the same time window stay physically close in the database, dramatically speeding up range queries (e.g., `WHERE id > Chrono64::from_date('2024-01-01')`).

### 4. **Clock Drift Management**
Like all time-based IDs, Chrono-ID assumes a monotonic clock.
- **Backward Drift:** If your system clock moves backward (e.g., via NTP sync), most implementations will throw a `ClockDriftError`.
- **Recommendation:** Use a "Wait-on-Clock" policy or ensure your server uses a "slew" based time sync rather than "stepping" the clock.

---

## 🛠 Project Status & Roadmap

**Current Status:** Under active initial development. We are implementing the first production-ready libraries based on the mathematical specification.

- [x] **Mathematical Foundation:** Completed.
- [ ] **Python:** Under construction (Stateful generator).
- [ ] **JS / TypeScript:** Under construction (Node.js & Browser).
- [ ] **C++:** Under construction (Header-only with Prime Baskets).
- [ ] **Database Extensions:** Implementing native logic for Postgres and SQLite.

---

## ⚖ License
MIT License. Feel free to use and contribute!
