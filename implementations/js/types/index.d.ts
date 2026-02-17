/**
 * Chrono-ID TypeScript Definitions (Diamond Standard)
 *
 * All ID variants wrap a native BigInt and provide time-ordered uniqueness.
 */

export class ChronoError extends Error {}

export interface Persona {
  node_id: number;
  node_salt: number;
  node_idx: number;
  seq_salt: number;
  seq_idx: number;
  seq_offset: number;
  last_rotate: number;
}

export abstract class ChronoBase {
  protected value: bigint;

  constructor(value?: string | number | bigint);

  /**
   * Generates a new ID using autonomous state.
   * If `dt` is provided, the ID will be pinned to that timestamp.
   */
  static generate(dt?: Date): ChronoBase;

  /** Decodes the timestamp embedded in the ID. */
  getTime(): Date;

  /** Returns the raw timestamp component value (precision-specific). */
  getTimestamp(): bigint;

  /** Returns formatted hex representation (e.g. 0123-4567-89AB-CDEF). */
  formatted(): string;

  /** ISO 8601 representation of the embedded timestamp. */
  toISOString(): string;

  /** Returns the underlying value as a BigInt. */
  toBigInt(): bigint;

  /** Returns the decimal string representation of the ID. */
  toString(): string;

  /** Returns the decimal string representation for JSON serialization. */
  toJSON(): string;

  /** Primitive value for numeric contexts. */
  valueOf(): bigint;
}

/**
 * Base class for 32-bit variants.
 * Provides the `toNumber()` method as 32-bit values are safe for JavaScript Numbers.
 */
export abstract class Chrono32Base extends ChronoBase {
  /** Returns the underlying value as a Number. */
  toNumber(): number;
}

// --- 32-bit Family (Epoch 2020) ---

/** Yearly precision: ~255 years capacity. */
export class UChrono32y extends Chrono32Base {}
export class Chrono32y extends Chrono32Base {}

/** Half-Yearly precision. */
export class UChrono32hy extends Chrono32Base {}
export class Chrono32hy extends Chrono32Base {}

/** Quarterly precision. */
export class UChrono32q extends Chrono32Base {}
export class Chrono32q extends Chrono32Base {}

/** Monthly precision: ~341 years capacity. */
export class UChrono32mo extends Chrono32Base {}
export class Chrono32mo extends Chrono32Base {}

/** Weekly precision. */
export class UChrono32w extends Chrono32Base {}
export class Chrono32w extends Chrono32Base {}

/** Daily precision: ~359 years capacity. */
export class UChrono32d extends Chrono32Base {}
export class Chrono32d extends Chrono32Base {
  static fromTime(date: Date, randomVal?: number | bigint): Chrono32d;
}

/** Hourly precision: ~477 years capacity. */
export class UChrono32h extends Chrono32Base {}
export class Chrono32h extends Chrono32Base {}

/** Ten-Minute precision. */
export class UChrono32tm extends Chrono32Base {}
export class Chrono32tm extends Chrono32Base {}

/** Minute precision: ~510 years capacity. */
export class UChrono32m extends Chrono32Base {}
export class Chrono32m extends Chrono32Base {}

/** Big-Second (Epoch-only) precision: ~136 years capacity. */
export class UChrono32bs extends Chrono32Base {}
export class Chrono32bs extends Chrono32Base {}

// --- 64-bit Family (Epoch 2020) ---

/** Monthly precision. */
export class UChrono64mo extends ChronoBase {}
export class Chrono64mo extends ChronoBase {}

/** Weekly precision. */
export class UChrono64w extends ChronoBase {}
export class Chrono64w extends ChronoBase {}

/** Daily precision. */
export class UChrono64d extends ChronoBase {}
export class Chrono64d extends ChronoBase {}

/** Hourly precision. */
export class UChrono64h extends ChronoBase {}
export class Chrono64h extends ChronoBase {}

/** Minute precision. */
export class UChrono64m extends ChronoBase {}
export class Chrono64m extends ChronoBase {}

/** Second precision: Expires 2292 (Signed) / 2412 (Unsigned). */
export class UChrono64s extends ChronoBase {
  static fromTime(date: Date, randomVal?: number | bigint): UChrono64s;
}
export class Chrono64s extends ChronoBase {
  static fromTime(date: Date, randomVal?: number | bigint): Chrono64s;
}

/** Deci-Second precision. */
export class UChrono64ds extends ChronoBase {}
export class Chrono64ds extends ChronoBase {}

/** Centi-Second precision. */
export class UChrono64cs extends ChronoBase {}
export class Chrono64cs extends ChronoBase {}

/** Millisecond precision: Expires 2298. */
export class UChrono64ms extends ChronoBase {
  static fromTime(date: Date, randomVal?: number | bigint): UChrono64ms;
}
export class Chrono64ms extends ChronoBase {
  static fromTime(date: Date, randomVal?: number | bigint): Chrono64ms;
}

/** Microsecond precision: Expires 2305. */
export class UChrono64us extends ChronoBase {
  static fromTime(
    date: Date,
    randomVal?: number | bigint,
    us_offset?: bigint,
  ): UChrono64us;
}
export class Chrono64us extends ChronoBase {
  static fromTime(
    date: Date,
    randomVal?: number | bigint,
    us_offset?: bigint,
  ): Chrono64us;
}
