# ChronoID: Sortable × Uncoordinated × Compact

ChronoID is a high-performance distributed identifier framework that solves the **ID Generation Trilemma** — the fundamental impossibility of achieving **Sortability**, **Uncoordinated Scaling**, and **Storage Efficiency** simultaneously with existing standards.

| Property              |        UUID v7        |   Twitter Snowflake   |         **ChronoID**         |
| :-------------------- | :-------------------: | :-------------------: | :--------------------------: |
| **Sortable**          |      ✅ ms-order      |      ✅ ms-order      | ✅ Configurable (µs → month) |
| **Uncoordinated**     |       ✅ Random       | ❌ Requires Zookeeper | ✅ Mode A: zero coordination |
| **Storage Efficient** | ❌ 128-bit (16 bytes) |  ✅ 64-bit (8 bytes)  |   ✅ **64-bit or 32-bit**    |

> [!IMPORTANT]
> **How?** ChronoID uses a **Rotating Persona** architecture with **Weyl-Golden self-healing** — leveraging the Birthday Paradox not as a risk factor, but as a calculated safety shield.
>
> 📄 [**Formal Proof**](./proof/proof.md) · 📋 [**Summary**](./proof/summary.md) · 🏆 [**Champion Comparison**](./proof/champion.md)

> [!TIP]
> **Novel Innovation: `chrono32y` — The Tenant ID Problem, Solved.** Tenant IDs appear as Foreign Keys in _every_ child table of a SaaS app. UUID costs 16 bytes per FK × millions of rows = gigabytes wasted. `chrono32y` fits in a **native 32-bit `INT`** (4 bytes), saves **12 bytes per FK**, and encodes as a **7-char Crockford Base32** string (e.g., `8Z5Y03`) for human-readable display. No other ID system offers this.

---

## 🏗 Architecture

### ID Structure

```text
 MSB                                                                    LSB
+--------+--------------------------------------------+-----------+-----------+
|  Sign  |     Timestamp (variant-specific precision) | Node ID   | Sequence  |
|  (0/1) |     (µs, ms, sec, min, hour, day... year)  | (mixed)   | (mixed)   |
+--------+--------------------------------------------+-----------+-----------+
          \___________  ___________/                    \________  ________/
                      \/                                         \/
              Time-Ordered Prefix                     "The Persona" (Entropy Suffix)
                                                    Weyl-Golden obfuscated
```

**Why Node + Sequence split?** A flat random suffix treats all collisions identically. The split enables **different defenses** per collision type:

- **Node collisions** → Weyl-Golden divergence (different multipliers force separation at $T+1$)
- **Sequence collisions** → impossible within one generator (monotonic counter)
- **Both colliding** → persona rotation resamples $N$ independently of $S$

The split also enables **Mode C's embedded routing** — Node bits encode a physical shard.

### Signed vs. Unsigned

| Prefix    | MSB       | Use When                                                                 |
| :-------- | :-------- | :----------------------------------------------------------------------- |
| `chrono`  | Always 0  | PostgreSQL, Java, Go (prevents negative `bigint` / `long`)               |
| `uchrono` | Available | Rust, C++, MySQL, Solidity (extra bit → **Node field**, +41% safe nodes) |

---

## ⚡ Three Operational Modes

ChronoID morphs between three architectural patterns without database migration:

### Mode A: Autonomous Persona (Stateless)

**Best For:** Serverless (Lambda), Edge, IoT, Offline-First Mobile.

- Zero coordination — nodes require no knowledge of each other
- **Self-Healing:** If two nodes collide at time $T$, Weyl-Golden divergence forces their IDs apart at $T+1$ with **98.4% probability**
- **Rotation:** Every 60 seconds, re-rolls Node ID, Salt, and Multiplier via CSPRNG
- **Burst:** On sequence overflow → immediate persona re-roll (never stalls)
- **Clock Skew:** Backward jump → treated as burst event, triggers re-roll

### Mode B: Instance-Native (Stateful)

**Best For:** PostgreSQL/MySQL Primary Keys, High-Frequency Ingestion.

- Ties Node bits to a static Instance ID; uses `nextval` for the sequence
- **B-Tree Optimized:** Strictly monotonically increasing → always appends to the right leaf
- **Rotation:** Every 10 minutes via Weyl-Step (`Node = (Node + C) % Max`)
- **Burst:** On sequence overflow → immediate Weyl-Step rotation (never stalls)
- **Why it wins:** Raw speed of `AUTO_INCREMENT` but globally unique and mergeable

### Mode C: Managed Registry (Topology-Aware)

**Best For:** Sharded Clusters, Multi-Region, Microservices.

- Node bits assigned by Registry (Redis / Etcd) — represent physical Shards or Tenants
- **Embedded Routing:** `Target = (ID >> b_S) & NodeMask` — a single bit-shift, no lookup table
- **Burst:** Spin-waits for next tick (Node cannot change — it encodes routing identity)
- **Why it wins:** Replaces Snowflake with 3.5× longer lifespan and no Zookeeper SPOF

---

## 📊 Variant Reference

### `chrono64` / `uchrono64` — Primary Keys (Mode A Safe)

| Variant | Precision | Bits (T/N/S) | Expiry | Safe Nodes (1-in-1M) | Use Case                |
| :------ | :-------- | :----------: | :----: | :------------------: | :---------------------- |
| **mo**  | Month     |   12/26/26   |  2361  |         94k          | Global SaaS / Billing   |
| **s**   | Second    |   33/16/15   |  2292  |        **65**        | **Standard DB Keys** ⭐ |
| **ds**  | Decisec   |   36/15/13   |  2237  |         732          | High-Freq App Logs      |
| **ms**  | ms        |   43/11/10   |  2298  |  64 (Mode B/C only)  | Real-time Systems       |
| **us**  | µs        |    53/6/5    |  2305  |  2 (Mode B/C only)   | Kernel Events           |

> **Default:** `chrono64s` — 1-second precision, 65 safe nodes (1-in-1M), native `bigint`, 250+ year lifespan.

### `chrono32` / `uchrono32` — Compact Keys

| Variant | Precision | Bits (T/N/S) | Expiry | Safe Nodes (1-in-1k) | Use Case             |
| :------ | :-------- | :----------: | :----: | :------------------: | :------------------- |
| **y**   | Year      |   8/13/11    |  2276  |       **183**        | Tenant / Member IDs  |
| **mo**  | Month     |   12/11/10   |  2361  |          64          | Subscription Billing |
| **bs**  | Bi-Sec    |    32/0/0    |  2292  |         None         | 32-bit Sort Index    |

> [!NOTE]
> **`chrono32y` — The Novel Tenant ID Innovation.**
> Every SaaS app references `tenant_id` in every child table. UUID costs **16 bytes per FK** — `chrono32y` costs **4 bytes**. At 1 billion rows, that's **~12 GB saved**.
>
> - **Year Precision:** Timestamp updates once per year → 24 bits for entropy (~16.7M tenants/year unsigned)
> - **Obfuscated:** Appears random (e.g., `9402115`), hiding growth rates from competitors
> - **Human-Readable:** Encodes as **7-char Crockford Base32** (e.g., `8Z5Y03`) — URL-safe, dictation-friendly
> - **Native INT:** 1-cycle JOINs, best-in-class index performance

---

## 🆚 Competitive Comparison

### vs. Legacy Standards

| Feature            | UUID v7             | Snowflake           | AUTO_INCREMENT  | **ChronoID**              |
| :----------------- | :------------------ | :------------------ | :-------------- | :------------------------ |
| **Storage**        | ❌ 128-bit          | ✅ 64-bit           | ✅ 32/64-bit    | ✅ **64 + 32-bit tier**   |
| **Uncoordinated**  | ✅ Random           | ❌ Zookeeper        | ❌ Local only   | ✅ **Mode A: zero coord** |
| **Self-Healing**   | ❌ Silent duplicate | ❌ Silent duplicate | ❌ Error        | ✅ **98.4% divergence**   |
| **Shard Routing**  | ❌ Lookup           | ❌ Lookup           | ❌ Not possible | ✅ **Mode C: embedded**   |
| **FK Compression** | ❌ 16 bytes/ref     | ❌ 8 bytes          | ✅ 4 bytes      | ✅ **chrono32y: 4 bytes** |
| **Lifespan**       | ✅ Long             | ⚠️ ~69 years        | ✅ Long         | ✅ **250+ years**         |

### vs. Modern Contenders

| Feature          | ULID        | KSUID       | TSID        | Cuid2       | **ChronoID**          |
| :--------------- | :---------- | :---------- | :---------- | :---------- | :-------------------- |
| **Storage**      | ❌ 16 bytes | ❌ 20 bytes | ✅ 8 bytes  | ❌ 16 bytes | ✅ **8 bytes (or 4)** |
| **Native INT**   | ❌ No       | ❌ No       | ✅ `bigint` | ❌ No       | ✅ **`bigint`/`int`** |
| **Self-Healing** | ❌ None     | ❌ None     | ❌ None     | ❌ None     | ✅ **98.4% diverge**  |
| **Polymorphic**  | ❌ Fixed    | ❌ Fixed    | ❌ Fixed    | ❌ Fixed    | ✅ **3 modes**        |
| **FK Tier**      | ❌ 16 bytes | ❌ 20 bytes | ❌ 8 bytes  | ❌ 16 bytes | ✅ **chrono32y: 4B**  |

---

## 💡 Novel Innovations (No Prior Art)

| Innovation                  | What's New                                                                     |
| :-------------------------- | :----------------------------------------------------------------------------- |
| **Self-Healing Collisions** | First ID system with active collision recovery (98.4% divergence at next tick) |
| **Polymorphic Modes**       | 3 architectures, 1 column type — switch without migration                      |
| **µs → Month Precision**    | Trade time granularity for entropy — 12 variants on one spectrum               |
| **Never-Stall Bursts**      | Sequence overflow triggers rotation, not blocking                              |
| **`chrono32y` Tenant ID**   | Purpose-built 32-bit FK — saves 12 bytes/row vs UUID                           |
| **Birthday Shield**         | Persona rotation resets collision probability — paradox as feature             |

---

## 📚 Best Practices

### Generator Lifecycle (Singleton Pattern)

ChronoID generators are **stateful** (they maintain a Persona and sequence counter). Create a **single, long-lived instance** per process and reuse it. Re-instantiating forces unnecessary CSPRNG re-rolls and risks sequence collisions.

### Database Indexing

Always use ChronoID as your `PRIMARY KEY`. The time-leading bits ensure the B-Tree index is **append-only**, preventing page splits. IDs created in the same window stay physically close, dramatically speeding up range queries.

### Clock Skew

ChronoID handles clock skew automatically via persona rotation (Mode A) or monotonic sequences (Mode B/C). **No manual clock management needed.** Sortability may be briefly disrupted, but uniqueness is always preserved.

### Security

ChronoID IDs are **obfuscated** (Weyl-Golden mixing) but **not cryptographically secure**. Don't use them as secret tokens or API keys. For those, use a CSPRNG directly.

---

## 🛠 Project Status

| Component               | Status               |
| :---------------------- | :------------------- |
| Mathematical Foundation | ✅ Complete          |
| Python                  | 🚧 Under development |
| JavaScript / TypeScript | 🚧 Under development |
| C++                     | 🚧 Under development |
| PostgreSQL Extension    | 🚧 Under development |
| SQLite Extension        | 🚧 Under development |

---

## ⚖ License

MIT License. See [LICENSE](./LICENSE).
