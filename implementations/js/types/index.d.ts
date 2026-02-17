/**
 * Chrono-ID TypeScript Definitions
 *
 * All ID variants wrap a native BigInt and provide time-ordered uniqueness.
 */

export enum Precision {
  Y = 0,
  HY = 1,
  Q = 2,
  MO = 3,
  W = 4,
  D = 5,
  H = 6,
  TM = 7,
  M = 8,
  BS = 9,
  S = 10,
  DS = 11,
  CS = 12,
  MS = 13,
  US = 14,
}

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

  static readonly PRECISION: Precision;
  static readonly T_BITS: number;
  static readonly N_BITS: number;
  static readonly S_BITS: number;
  static readonly SIGNED: boolean;

  constructor(value?: string | number | bigint);

  /**
   * Generates a new ID using autonomous state.
   */
  static generate<T extends ChronoBase>(
    this: new (v?: any) => T,
    dt?: Date,
    node_id?: number,
    seq?: number,
    persona?: Persona,
  ): T;

  /**
   * Constructs an ID from a specific Date and optional random entropy.
   */
  static fromTime<T extends ChronoBase>(
    this: new (v?: any) => T,
    date: Date,
    randomVal?: number | bigint,
  ): T;

  /**
   * Parses an ISO 8601 string into a Chrono-ID.
   */
  static fromISOString<T extends ChronoBase>(
    this: new (v?: any) => T,
    iso: string,
    seq?: number | bigint,
  ): T;

  /**
   * Constructs an ID from explicit components with optional mixing.
   */
  static fromParts<T extends ChronoBase>(
    this: new (v?: any) => T,
    dt: Date,
    node_id: number,
    seq: number,
    p_idx?: number,
    salt?: number | bigint,
    ts?: bigint,
    persona?: Persona,
  ): T;

  /**
   * Parses a hyphenated hex string into an ID.
   */
  static fromFormat<T extends ChronoBase>(
    this: new (v?: any) => T,
    fmt: string,
  ): T;

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

/** Yearly precision: ~256 years (Expiry 2276). */
export class UChrono32y extends Chrono32Base {}
export class Chrono32y extends Chrono32Base {}

/** Half-Yearly precision: ~256 years (Expiry 2276). */
export class UChrono32hy extends Chrono32Base {}
export class Chrono32hy extends Chrono32Base {}

/** Quarterly precision: ~256 years (Expiry 2276). */
export class UChrono32q extends Chrono32Base {}
export class Chrono32q extends Chrono32Base {}

/** Monthly precision: ~341 years (Expiry 2361). */
export class UChrono32mo extends Chrono32Base {}
export class Chrono32mo extends Chrono32Base {}

/** Weekly precision: ~314 years (Expiry 2334). */
export class UChrono32w extends Chrono32Base {}
export class Chrono32w extends Chrono32Base {}

/** Daily precision: ~358 years (Expiry 2378). */
export class UChrono32d extends Chrono32Base {}
export class Chrono32d extends Chrono32Base {}

/** Hourly precision: ~478 years (Expiry 2498). */
export class UChrono32h extends Chrono32Base {}
export class Chrono32h extends Chrono32Base {}

/** Ten-Minute precision: ~319 years (Expiry 2339). */
export class UChrono32tm extends Chrono32Base {}
export class Chrono32tm extends Chrono32Base {}

/** Minute precision: ~510 years (Expiry 2530). */
export class UChrono32m extends Chrono32Base {}
export class Chrono32m extends Chrono32Base {}

/** Bi-Second (2s) precision: ~136 years (Expiry 2156). Extra bit in uchrono32bs goes to NodeID. */
export class UChrono32bs extends Chrono32Base {}
export class Chrono32bs extends Chrono32Base {}

// --- 64-bit Family (Epoch 2020) ---

/** Monthly precision: Expires 2361. */
export class UChrono64mo extends ChronoBase {}
export class Chrono64mo extends ChronoBase {}

/** Weekly precision: Expires 2334. */
export class UChrono64w extends ChronoBase {}
export class Chrono64w extends ChronoBase {}

/** Daily precision: Expires 2378. */
export class UChrono64d extends ChronoBase {}
export class Chrono64d extends ChronoBase {}

/** Hourly precision: Expires 2258. */
export class UChrono64h extends ChronoBase {}
export class Chrono64h extends ChronoBase {}

/** Minute precision: Expires 2275. */
export class UChrono64m extends ChronoBase {}
export class Chrono64m extends ChronoBase {}

/** Second precision: Expires 2292. */
export class UChrono64s extends ChronoBase {}
export class Chrono64s extends ChronoBase {}

/** Deci-Second precision: Expires 2237. */
export class UChrono64ds extends ChronoBase {}
export class Chrono64ds extends ChronoBase {}

/** Centi-Second precision: Expires 2368. */
export class UChrono64cs extends ChronoBase {}
export class Chrono64cs extends ChronoBase {}

/** Millisecond precision: Expires 2298. */
export class UChrono64ms extends ChronoBase {}
export class Chrono64ms extends ChronoBase {}

/** Microsecond precision: Expires 2305. */
export class UChrono64us extends ChronoBase {
  /**
   * Constructs an ID from a specific Date and optional random entropy.
   * Specific to US precision to allow `us_offset`.
   */
  static fromTime<T extends ChronoBase>(
    this: new (v?: any) => T,
    date: Date,
    randomVal?: number | bigint,
    us_offset?: bigint,
  ): T;
}
export class Chrono64us extends ChronoBase {
  /**
   * Constructs an ID from a specific Date and optional random entropy.
   * Specific to US precision to allow `us_offset`.
   */
  static fromTime<T extends ChronoBase>(
    this: new (v?: any) => T,
    date: Date,
    randomVal?: number | bigint,
    us_offset?: bigint,
  ): T;
}
