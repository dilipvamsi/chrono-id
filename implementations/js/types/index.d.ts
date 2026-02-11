/**
 * Base class for all ChronoID types.
 * Acts as a wrapper around a BigInt.
 */
declare abstract class ChronoBase {
    protected value: bigint;

    /**
     * Creates a new ID using the current system time.
     * @param value Optional initial value to wrap (string, number, or bigint).
     */
    constructor(value?: string | number | bigint);

    /**
     * Creates a new ID instance from a specific Date object.
     * @param date The date to encode into the ID.
     * @param randomVal Optional specific entropy bits (useful for testing).
     */
    static fromTime(date: Date, randomVal?: number | bigint): ChronoBase;

    /**
     * Decodes the timestamp embedded in the ID.
     */
    getTime(): Date;

    /**
     * Returns the underlying BigInt value.
     */
    toBigint(): bigint;

    /**
     * Returns the string representation of the ID.
     */
    toString(): string;

    /**
     * Returns string representation for JSON serialization.
     */
    toJSON(): string;
}

export class Chrono32Base extends ChronoBase {
    /**
     * Returns the Number of BigInt primitive value.
     */
    toNumber(): number;
}

// --- 32-bit Family (Epoch 2000) ---

/** Unsigned 32-bit Day ID. Capacity: 16,384/day. Expires: 2717. */
export class UChrono32 extends Chrono32Base { }

/** Signed 32-bit Day ID. Capacity: 8,192/day. Expires: 2717. */
export class Chrono32 extends Chrono32Base { }

/** Unsigned 32-bit Hour ID. Capacity: 2,048/hr. Expires: 2239. */
export class UChrono32h extends Chrono32Base { }

/** Signed 32-bit Hour ID. Capacity: 1,024/hr. Expires: 2239. */
export class Chrono32h extends Chrono32Base { }

/** Unsigned 32-bit Minute ID. Capacity: 32/min. Expires: 2255. */
export class UChrono32m extends Chrono32Base { }

/** Signed 32-bit Minute ID. Capacity: 16/min. Expires: 2255. */
export class Chrono32m extends Chrono32Base { }


// --- 64-bit Family (Epoch 1970) ---

/** Unsigned 64-bit Second ID. Capacity: 268M/sec. Expires: 4147. */
export class UChrono64 extends ChronoBase { }

/** Signed 64-bit Second ID. Capacity: 134M/sec. Expires: 4147. */
export class Chrono64 extends ChronoBase { }

/** Unsigned 64-bit Millisecond ID. Capacity: 1M/ms. Expires: 2527. */
export class UChrono64ms extends ChronoBase { }

/** Signed 64-bit Millisecond ID. Capacity: 524k/ms. Expires: 2527. */
export class Chrono64ms extends ChronoBase { }

/** Unsigned 64-bit Microsecond ID. Capacity: 1,024/us. Expires: 2540. */
export class UChrono64us extends ChronoBase { }

/** Signed 64-bit Microsecond ID. Capacity: 512/us. Expires: 2540. */
export class Chrono64us extends ChronoBase { }