# Why Chrono-ID?

Chrono-ID was created to solve a specific problem in distributed systems: **Generating 64-bit, k-sortable unique IDs without central coordination.**

Existing solutions force a compromise:

- **UUIDv4/v7**: Safe without coordination, but too large (128-bit) and slow for databases.
- **Snowflake**: Efficient (64-bit), but requires complex manual coordination (ZooKeeper/Etcd) to assign Machine IDs.

## The Solution: Auto-Distributed Safety

Chrono-ID uses a **Deterministic Permutation** algorithm combined with a **"Safe Prime Basket"** defense. This allows nodes to self-assign identities randomly with a near-zero collision probability, effectively giving you **Snowflake performance with UUID ergonomics.**

## Comparison with Industry Standards

| Feature              | **Chrono-ID**                    | **UUIDv7**             | **Twitter Snowflake**           |
| :------------------- | :------------------------------- | :--------------------- | :------------------------------ |
| **Bit Size**         | **64-bit** (Database Friendly)   | 128-bit (Bloated)      | 64-bit                          |
| **Generation Speed** | **~35ns** (Native Int Ops)       | ~90ns (Complex logic)  | **~32ns**                       |
| **Coordination**     | **Zero Config** (Auto-Discovery) | Zero Config            | **Manual** (ZooKeeper required) |
| **Throughput**       | **Unbounded** (Burst Rotation)   | Limited by Randomness  | Capped (Blocks on overflow)     |
| **Safety Model**     | Deterministic + Probabilistic    | Probabilistic (Random) | Deterministic (Hard-coded)      |
| **K-Sortable**       | **Yes**                          | Yes                    | Yes                             |

## Performance & Variants

Chrono-ID avoids the "one-size-fits-all" trap by offering specialized variants. Whether you need **64-bit persistence** or **32-bit ephemeral efficiency**, there is a mathematically optimized variant.

### Space vs. Time Trade-offs

| Variant Family | Bits | Time Precision   | Sequence Space             | Ideal Use Case                          |
| :------------- | :--- | :--------------- | :------------------------- | :-------------------------------------- |
| **Chrono64**   | 64   | Seconds          | **Extreme** (Quadrillions) | Database Primary Keys, Distributed Jobs |
| **Chrono64ms** | 64   | **Milliseconds** | High (Billions)            | High-Frequency Logs, Event Tracing      |
| **Chrono32**   | 32   | Days             | Moderate                   | Customer Support Tickets, Short Refs    |
| **Chrono32h**  | 32   | Hours            | Low                        | Ephemeral Tokens, Cache Keys            |

### Throughput Capabilities

Unlike Snowflake, which blocks when the sequence limit is reached, Chrono-ID uses **"Burst Rotation"** to unlock effectively infinite throughput by rotating identities mid-millisecond.

| Metric               | **Chrono64**          | **Chrono64ms** | **Chrono64us**        |
| :------------------- | :-------------------- | :------------- | :-------------------- |
| **Natural Capacity** | 32,768 / sec          | 512 / ms       | 512 / µs              |
| **Burst Capacity**   | **134 Million / sec** | **500k+ / ms** | **512 / µs (Capped)** |

> [!NOTE]
> For the mathematical proof of safety and the algorithm details, see [MATHEMATICAL_PROOF.md](./MATHEMATICAL_PROOF.md).
