/**
 * Chrono-ID (Diamond Standard): Type-safe, K-Sortable, Time-Ordered Unique Identifiers.
 *
 * This is a single-file portable implementation matching the Python "Diamond Standard" core.
 * Features:
 * - Epoch 2020-01-01
 * - Weyl-Golden Self-Healing Mixer
 * - 40 variants across 32-bit and 64-bit families
 */

(function (root, factory) {
  if (typeof define === "function" && define.amd) {
    define([], factory);
  } else if (typeof module === "object" && module.exports) {
    module.exports = factory();
  } else {
    root.ChronoID = factory();
  }
})(typeof self !== "undefined" ? self : this, function () {
  // --- Constants ---
  /** Unix timestamp for the Diamond Standard Epoch: 2020-01-01T00:00:00Z */
  const EPOCH_2020 = 1577836800;
  /** Gregorian year for the Epoch */
  const EPOCH_YEAR = 2020;

  /**
   * Custom error class for Chrono-ID specific failures.
   */
  class ChronoError extends Error {
    constructor(message) {
      super(message);
      this.name = "ChronoError";
    }
  }

  // --- Weyl Multipliers (Golden Ratio Primes) ---
  /**
   * 128 prime multipliers derived from the Golden Ratio (Phi) and Silver Ratio (sqrt(2)).
   * These provide uniform distribution across rotation-symmetric neighborhoods in the
   * 64-bit entropy space, ensuring bit-parity with the Python implementation.
   */
  // prettier-ignore
  const WEYL_MULTIPLIERS = [
    0x9e3779b97f4a7c55n, 0x084160217307455fn, 0x724b468966c40e29n, 0xdc552cf15a80d73dn,
    0x465f13594e3da051n, 0xb068f9c141fa6951n, 0x1a72e02935b7324bn, 0x847cc6912973fb51n,
    0xee86acf91d30c45dn, 0x5890936110ed8d9dn, 0xc29a79c904aa56b5n, 0x2ca46030f8671f7bn,
    0x96ae4698ec23e935n, 0x00b82d00dfe0b1c1n, 0x6ac21368d39d7a8bn, 0xd4cbf9d0c75a43e7n,
    0x3ed5e038bb170c95n, 0xa8dfc6a0aed3d5a9n, 0x12e9ad08a2909eb1n, 0x7cf39370964d67b1n,
    0xe6fd79d88a0a30b7n, 0x510760407dc6f9cfn, 0xbb1146a87183c2c7n, 0x251b2d1065408be9n,
    0x8f25137858fd54d7n, 0xf92ef9e04cba1defn, 0x6338e0484076e71dn, 0xcd42c6b03433b011n,
    0x374cad1827f0791dn, 0xa15693801bad4215n, 0x0b6079e80f6a0b09n, 0x756a60500326d41bn,
    0xdf7446b7f6e39d1fn, 0x497e2d1feaa06635n, 0xb3881387de5d2f37n, 0x1d91f9efd219f857n,
    0x879be057c5d6c153n, 0xf1a5c6bfb9938a6bn, 0x5ba9ad27ad505375n, 0xc5b9938fa10d1c5dn,
    0x2fc379f794c9e573n, 0x99cd605f8886aee9n, 0x03d746c77c43776bn, 0x6de12d2f7000414fn,
    0xd7eb139763bd09a1n, 0x41f4f9ff5779d297n, 0xabfee0674b369bafn, 0x1608c6cf3ef364ddn,
    0x8012ad3732b02dc1n, 0xea1c939f266cf721n, 0x54267a071a29bfb3n, 0xbe30606f0de688f3n,
    0x283a46d701a351e5n, 0x92442d3ef5601abdn, 0xfc4e13a6e91ce40fn, 0x6657fa0edcd9accdn,
    0xd061e076d09675d9n, 0x3a6bc6dec4533eddn, 0xa475ad46b81007e7n, 0x0e7f93aeabccd139n,
    0x78897a169f899a17n, 0xe293607e93466303n, 0x4c9d46e687032c27n, 0xb6a72d4e7abff54fn,
    0x20b113b66e7cbe91n, 0x8abafa1e62398725n, 0xf4c4e08655f65035n, 0x5ecec6ee49b31947n,
    0xc8d8ad563d6fe241n, 0x32e293be312cab69n, 0x9cec7a2624e9744bn, 0x06f6608e18a63d4dn,
    0x710046f60c6306afn, 0xdb0a2d5e001fcf61n, 0x451413c5f3dc98b3n, 0xaf1dfa2de7996193n,
    0x1927e095db562a87n, 0x8331c6fdcf12f37fn, 0xed3bad65c2cfbcc7n, 0x574593cdb68c858fn,
    0xc14f7a35aa494ea1n, 0x2b59609d9e0617b3n, 0x9563470591c2e0dbn, 0xff6d2d6d857fa9c3n,
    0x697713d5793c72c1n, 0xd380fa3d6cf93bc3n, 0x3d8ae0a560b60525n, 0xa794c70d5472ce01n,
    0x119ead75482f96e1n, 0x7ba893dd3bec601dn, 0xe5b27a452fa92947n, 0x4fbc60ad2365f21bn,
    0xb9c647151722bb17n, 0x23d02d7d0adf842bn, 0x8dda13e4fe9c4d19n, 0xf7e3fa4cf2591655n,
    0x61ede0b4e615df23n, 0xcbf7c71cd9d2a89bn, 0x3601ad84cd8f7175n, 0xa00b93ecc14c3a3fn,
    0x0a157a54b5090345n, 0x741f60bca8c5cc3dn, 0xde2947249c829575n, 0x48332d8c903f5e77n,
    0xb23d13f483fc279bn, 0x1c46fa5c77b8f063n, 0x8650e0c46b75b98dn, 0xf05ac72c5f32827bn,
    0x5a64ad9452ef4bafn, 0xc46e93fc46ac149dn, 0x2e787a643a68ddbbn, 0x988260cc2e25a6d9n,
    0x028c473421e26f99n, 0x6c962d9c159f3911n, 0xd6a01404095c01b3n, 0x40a9fa6bfd18cad5n,
    0xaab3e0d3f0d593dbn, 0x14bdc73be4925d13n, 0x7ec7ada3d84f25d7n, 0xe8d1940bcc0beed1n,
    0x52db7a73bfc8b7d7n, 0xbce560dbb3858119n, 0x26ef4743a7424a1bn, 0x90f92dab9aff12fdn,
    0xfb0314138ebbdc2dn, 0x650cfa7b8278a55dn, 0xcf16e0e376356e0fn, 0x3920c74b69f23717n,
  ];

  const _WEYL_CACHE = new Map();

  /**
   * Precomputes and caches bit-extracted multipliers for specific node/sequence widths.
   * This ensures the mixing algorithm stays in a safe Prime-Modulo space.
   * @param {number} bits - Number of bits allocated for the component.
   * @returns {BigInt[]} Array of 128 extracted multipliers.
   * @private
   */
  function _getWeylMults(bits) {
    if (!_WEYL_CACHE.has(bits)) {
      if (bits <= 0) {
        _WEYL_CACHE.set(bits, new Array(128).fill(1n));
      } else {
        const shift = BigInt(64 - bits);
        _WEYL_CACHE.set(
          bits,
          WEYL_MULTIPLIERS.map((m) => (m >> shift) | 1n),
        );
      }
    }
    return _WEYL_CACHE.get(bits);
  }

  // --- Precisions ---
  const Precision = {
    Y: 0,
    HY: 1,
    Q: 2,
    MO: 3,
    W: 4,
    D: 5,
    H: 6,
    TM: 7,
    M: 8,
    BS: 9,
    S: 10,
    DS: 11,
    CS: 12,
    MS: 13,
    US: 14,
  };

  const TS_COMPUTE = [
    (dt) => BigInt(dt.getUTCFullYear() - EPOCH_YEAR),
    (dt) =>
      BigInt(
        (dt.getUTCFullYear() - EPOCH_YEAR) * 2 + (dt.getUTCMonth() < 6 ? 0 : 1),
      ),
    (dt) =>
      BigInt(
        (dt.getUTCFullYear() - EPOCH_YEAR) * 4 +
          Math.floor(dt.getUTCMonth() / 3),
      ),
    (dt) => BigInt((dt.getUTCFullYear() - EPOCH_YEAR) * 12 + dt.getUTCMonth()),
    (dt) =>
      BigInt(
        Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 604800),
      ),
    (dt) =>
      BigInt(
        Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 86400),
      ),
    (dt) =>
      BigInt(Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 3600)),
    (dt) =>
      BigInt(Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 600)),
    (dt) =>
      BigInt(Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 60)),
    (dt) =>
      BigInt(Math.floor((Math.floor(dt.getTime() / 1000) - EPOCH_2020) / 2)),
    (dt) => BigInt(Math.floor(dt.getTime() / 1000) - EPOCH_2020),
    (dt) =>
      BigInt(Math.floor(dt.getTime() / 1000) - EPOCH_2020) * 10n +
      BigInt(Math.floor(dt.getUTCMilliseconds() / 100)),
    (dt) =>
      BigInt(Math.floor(dt.getTime() / 1000) - EPOCH_2020) * 100n +
      BigInt(Math.floor(dt.getUTCMilliseconds() / 10)),
    (dt) =>
      BigInt(Math.floor(dt.getTime() / 1000) - EPOCH_2020) * 1000n +
      BigInt(dt.getUTCMilliseconds()),
    (dt) =>
      BigInt(Math.floor(dt.getTime() / 1000) - EPOCH_2020) * 1000000n +
      BigInt(dt.getUTCMilliseconds()) * 1000n +
      BigInt(dt.__us || 0),
  ];

  const TS_REVERSE = [
    (ts) => {
      const d = new Date(Date.UTC(EPOCH_YEAR + Number(ts), 0, 1));
      return d.getTime() / 1000;
    },
    (ts) => {
      const d = new Date(
        Date.UTC(
          EPOCH_YEAR + Math.floor(Number(ts) / 2),
          Number(ts) % 2 === 0 ? 0 : 6,
          1,
        ),
      );
      return d.getTime() / 1000;
    },
    (ts) => {
      const d = new Date(
        Date.UTC(
          EPOCH_YEAR + Math.floor(Number(ts) / 4),
          (Number(ts) % 4) * 3,
          1,
        ),
      );
      return d.getTime() / 1000;
    },
    (ts) => {
      const d = new Date(
        Date.UTC(EPOCH_YEAR + Math.floor(Number(ts) / 12), Number(ts) % 12, 1),
      );
      return d.getTime() / 1000;
    },
    (ts) => EPOCH_2020 + Number(ts) * 604800,
    (ts) => EPOCH_2020 + Number(ts) * 86400,
    (ts) => EPOCH_2020 + Number(ts) * 3600,
    (ts) => EPOCH_2020 + Number(ts) * 600,
    (ts) => EPOCH_2020 + Number(ts) * 60,
    (ts) => EPOCH_2020 + Number(ts) * 2,
    (ts) => EPOCH_2020 + Number(ts),
    (ts) => EPOCH_2020 + Number(ts) / 10,
    (ts) => EPOCH_2020 + Number(ts) / 100,
    (ts) => EPOCH_2020 + Number(ts) / 1000,
    (ts) => EPOCH_2020 + Number(ts) / 1000000,
  ];

  // --- Crypto ---
  let _crypto_impl = null;
  if (typeof crypto !== "undefined") _crypto_impl = crypto;
  else if (typeof require === "function") {
    try {
      _crypto_impl = require("crypto");
    } catch (e) {}
  }

  /**
   * Securely generates a specified number of random bits using the best available entropy source.
   * @param {number} bits - Number of bits to generate.
   * @returns {BigInt} The generated random value.
   * @private
   */
  function randbits(bits) {
    const bytes = Math.ceil(bits / 8);
    let buf;
    if (_crypto_impl && _crypto_impl.getRandomValues) {
      buf = new Uint8Array(bytes);
      _crypto_impl.getRandomValues(buf);
    } else if (_crypto_impl && _crypto_impl.randomBytes) {
      buf = _crypto_impl.randomBytes(bytes);
    } else {
      buf = new Uint8Array(bytes);
      for (let i = 0; i < bytes; i++) buf[i] = Math.floor(Math.random() * 256);
    }
    let res = 0n;
    for (let i = 0; i < buf.length; i++) {
      res = (res << 8n) | BigInt(buf[i]);
    }
    return res & ((1n << BigInt(bits)) - 1n);
  }

  // --- Persona & Generator ---
  /**
   * Persona represents the unique entropy profile of a generator instance.
   * It uses self-healing Weyl-Golden mixing to distribute collision risk across
   * 128 orthogonal neighborhoods.
   */
  class Persona {
    /**
     * @param {number} [sBits=15] - Sequence bits to determine salt/offset ranges.
     */
    constructor(sBits = 15) {
      this.rotate(sBits);
    }
    /**
     * Forces a full rotation of the entropy salts and offsets.
     * @param {number} [sBits=15]
     */
    rotate(sBits = 15) {
      this._reseed(sBits);
    }
    /** @private */
    _reseed(sBits, ts = null) {
      const pool = randbits(128);
      this.node_id = Number(pool & 0xffffn);
      this.node_salt = Number((pool >> 16n) & 0xffffn);
      this.node_idx = Number((pool >> 32n) & 0x7fn);
      this.seq_salt = Number((pool >> 39n) & 0xffffn);
      this.seq_idx = Number((pool >> 55n) & 0x7fn);
      const offsetMask = BigInt(sBits > 0 ? (1n << BigInt(sBits)) - 1n : 0n);
      this.seq_offset = Number((pool >> 62n) & offsetMask);
      this.last_rotate = ts !== null ? ts : Date.now() / 1000;
    }
  }

  /**
   * Generator handles stateful ID creation, managing timestamp order and sequence overflow.
   */
  class Generator {
    /**
     * @param {typeof ChronoBase} cls - The specific variant class to generate.
     * @param {Persona} [persona=null] - Optional shared or custom persona.
     */
    constructor(cls, persona = null) {
      this.cls = cls;
      this.persona = persona || new Persona(cls.S_BITS);
      this.last_ts = 0n;
      this.sequence = 0n;
      this.seq_mask = cls.S_BITS > 0 ? (1n << BigInt(cls.S_BITS)) - 1n : 0n;
    }
    /**
     * Generates a new ID instance.
     * @param {Date} [dt=null] - Optional target date. Defaults to current time.
     * @returns {ChronoBase}
     */
    generate(dt = null) {
      if (!dt) dt = new Date();
      const ts_unix = dt.getTime() / 1000;
      const ts = this.cls.PRECISION_FUNC(dt);
      if (ts > this.last_ts) {
        this.last_ts = ts;
        this.sequence = 0n;
      } else if (ts === this.last_ts) {
        this.sequence = (this.sequence + 1n) & this.seq_mask;
        if (this.sequence === 0n) this.persona.rotate(this.cls.S_BITS);
      } else {
        this.persona.rotate(this.cls.S_BITS);
        this.sequence = (this.sequence + 1n) & this.seq_mask;
      }
      if (ts_unix - this.persona.last_rotate > 60)
        this.persona._reseed(this.cls.S_BITS, ts_unix);
      return this.cls.fromPersona(
        dt,
        this.persona.node_id,
        Number(this.sequence),
        this.persona,
        ts,
      );
    }
  }

  const _GLOBAL_STATES = new Map();
  function _getGeneratorState(cls) {
    if (!_GLOBAL_STATES.has(cls)) _GLOBAL_STATES.set(cls, new Generator(cls));
    return _GLOBAL_STATES.get(cls);
  }

  // --- Base Class ---
  /**
   * Abstract base class for all Chrono-ID variants.
   * Encapsulates a BigInt value and provides methods for timestamp extraction,
   * formatting, and serialization.
   */
  class ChronoBase {
    /** @type {number} The precision level (0-14) from the Precision enum. */
    static PRECISION = Precision.S;
    /** @type {number} Total bits allocated to the timestamp component. */
    static T_BITS = 0;
    /** @type {number} Total bits allocated to the node entropy component. */
    static N_BITS = 0;
    /** @type {number} Total bits allocated to the sequence entropy component. */
    static S_BITS = 0;
    /** @type {boolean} Whether the ID uses a sign bit (Chrono Family). */
    static SIGNED = true;

    // The following properties are computed once per variant class during initialization:
    /** @type {BigInt} */ static T_MASK;
    /** @type {BigInt} */ static N_MASK;
    /** @type {BigInt} */ static S_MASK;
    /** @type {BigInt} */ static T_SHIFT;
    /** @type {BigInt[]} */ static N_MULT;
    /** @type {BigInt[]} */ static S_MULT;
    /** @type {Function} */ static PRECISION_FUNC;
    /** @type {Function} */ static REVERSE_FUNC;
    /** @type {BigInt} */ static PRECISION_US;
    /** @type {BigInt} */ static RAND_MASK;
    // Legacy support aliases
    /** @type {BigInt} */ static SHIFT;
    /** @type {BigInt} */ static MASK;

    /**
     * Initializes a new ID instance.
     * @param {string|number|BigInt} [value] - Initial value. If omitted, generates a new ID.
     */
    constructor(value) {
      if (value === undefined || value === null)
        return this.constructor.generate();
      if (typeof value === "string") {
        if (
          value.includes("-") ||
          value.includes(" ") ||
          /[a-fA-F]/.test(value)
        ) {
          const clean = value.replace(/-/g, "").replace(/ /g, "");
          try {
            this.value = BigInt("0x" + clean);
          } catch (e) {
            this.value = BigInt(value);
          }
        } else {
          this.value = BigInt(value);
        }
      } else {
        this.value = BigInt(value);
      }
    }

    /**
     * Generates a new ID using the global autonomous generator.
     * @param {Date} [dt=null] - Optional timestamp for the ID.
     * @param {number} [node_id=null] - Optional explicit node ID (requires seq).
     * @param {number} [seq=null] - Optional explicit sequence (requires node_id).
     * @param {Persona} [persona=null] - Optional custom persona for generation.
     * @returns {ChronoBase}
     */
    static generate(dt = null, node_id = null, seq = null, persona = null) {
      if (persona) return new Generator(this, persona).generate(dt);
      if (node_id !== null && seq !== null) {
        return this.fromParts(dt || new Date(), node_id, seq);
      }
      return _getGeneratorState(this).generate(dt);
    }

    /**
     * Low-level constructor using Persona entropy.
     * Implements the core Weyl-Golden mixing algorithm to produce bit-parity with Phoenix/Python.
     * @private
     */
    static fromPersona(dt, node_id, seq, persona, ts = null) {
      if (!dt) throw new ChronoError("Input date is null");
      if (Math.floor(dt.getTime() / 1000) < EPOCH_2020)
        throw new ChronoError(
          "Timestamp underflow: Date is before Epoch (2020-01-01)",
        );
      const ts_val =
        (ts !== null ? BigInt(ts) : this.PRECISION_FUNC(dt)) & this.T_MASK;
      const mix_n =
        ((BigInt(node_id) * this.N_MULT[persona.node_idx]) ^
          BigInt(persona.node_salt)) &
        this.N_MASK;
      const s_val = (BigInt(seq) + BigInt(persona.seq_offset)) & this.S_MASK;
      const mult_s = this.S_MULT[persona.seq_idx];
      const mix_s = ((s_val * mult_s) ^ BigInt(persona.seq_salt)) & this.S_MASK;
      return new this(
        (ts_val << this.T_SHIFT) | (mix_n << BigInt(this.S_BITS)) | mix_s,
      );
    }

    /**
     * Constructs an ID from explicit components with optional mixing.
     * @param {Date} dt
     * @param {number} node_id
     * @param {number} seq
     * @param {number} [p_idx=0] - Index for selecting Weyl multipliers.
     * @param {number|BigInt} [salt=0] - Salt value for rotation.
     * @param {BigInt} [ts=null] - Overridden raw timestamp calculation.
     * @param {Persona} [persona=null] - Optional persona for standard mixing.
     * @returns {ChronoBase}
     */
    static fromParts(
      dt,
      node_id,
      seq,
      p_idx = 0,
      salt = 0,
      ts = null,
      persona = null,
    ) {
      if (persona) return this.fromPersona(dt, node_id, seq, persona, ts);
      const ts_val =
        (ts !== null ? BigInt(ts) : this.PRECISION_FUNC(dt)) & this.T_MASK;
      const mix_n =
        ((BigInt(node_id) * this.N_MULT[p_idx % 128]) ^ BigInt(salt)) &
        this.N_MASK;
      const mix_s =
        ((BigInt(seq) * this.S_MULT[p_idx % 128]) ^ BigInt(salt)) & this.S_MASK;
      return new this(
        (ts_val << this.T_SHIFT) | (mix_n << BigInt(this.S_BITS)) | mix_s,
      );
    }

    /**
     * Constructs an ID from a specific Date and optional random entropy.
     * @param {Date} date - Source date.
     * @param {number|BigInt} [randomVal=null] - Optional entropy bits (NodeID + Sequence).
     * @param {BigInt} [us_offset=0n] - Microsecond offset for US-precision variants.
     * @returns {ChronoBase}
     */
    static fromTime(date, randomVal = null, us_offset = 0n) {
      if (date === null || date === undefined)
        throw new ChronoError("Input date is null");
      if (!(date instanceof Date) || isNaN(date.getTime()))
        throw new ChronoError("Invalid date object");
      const dt = new Date(date.getTime());
      dt.__us = Number(us_offset);
      if (randomVal !== null) {
        const r = BigInt(randomVal);
        const seqMask = (1n << BigInt(this.S_BITS)) - 1n;
        const seq = Number(r & seqMask);
        const n_bits = BigInt(this.N_BITS);
        const node_id =
          n_bits > 0n
            ? Number((r >> BigInt(this.S_BITS)) & ((1n << n_bits) - 1n))
            : 0;
        return this.fromParts(dt, node_id, seq);
      }
      return this.generate(dt);
    }

    /**
     * Parses an ISO 8601 string into a Chrono-ID.
     * Handles microsecond precision if present in the fractional section.
     * @param {string} iso - The ISO string to parse.
     * @param {number|BigInt} [seq=null] - Optional explicit sequence.
     * @returns {ChronoBase}
     */
    static fromISOString(iso, seq = null) {
      if (!iso) throw new ChronoError("Input string is null");

      // Treat naive ISO strings (no Z or +/- offset) as UTC
      let normalizedIso = iso;
      if (!iso.endsWith("Z") && !/[+-]\d{2}(?::?\d{2})?$/.test(iso)) {
        normalizedIso += "Z";
      }

      let us = 0;
      const m = normalizedIso.match(/\.(\d+)/);
      if (m) {
        let frac = m[1].padEnd(6, "0").substring(0, 6);
        us = parseInt(frac) % 1000;
      }
      const dt = new Date(normalizedIso);
      if (isNaN(dt.getTime())) throw new ChronoError("Invalid ISO 8601 format");
      dt.__us = us;
      if (seq !== null) return this.generate(dt, null, Number(seq));
      return this.generate(dt);
    }

    /**
     * Parses a hyphenated hex string into an ID.
     * @param {string} fmt - The formatted string (e.g. 0123-4567-89AB-CDEF).
     * @returns {ChronoBase}
     */
    static fromFormat(fmt) {
      if (!fmt) throw new ChronoError("Formatted input is null");
      return new this(fmt);
    }

    /**
     * Reconstructs the original Date object from the encoded timestamp bits.
     * @returns {Date}
     */
    getTime() {
      const ts_val = this.value >> this.constructor.T_SHIFT;
      const unix = this.constructor.REVERSE_FUNC(ts_val);
      const dt = new Date(unix * 1000);
      if (this.constructor.PRECISION === Precision.US)
        dt.__us = Math.round((unix * 1000000) % 1000);
      return dt;
    }
    /**
     * Returns the raw timestamp component as a BigInt.
     * @returns {BigInt}
     */
    getTimestamp() {
      return this.value >> this.constructor.T_SHIFT;
    }
    /**
     * Returns an ISO 8601 string representation of the encoded timestamp.
     * @returns {string}
     */
    toISOString() {
      const dt = this.getTime();
      const base = dt.toISOString().split(".")[0];
      if (this.constructor.PRECISION === Precision.MS)
        return `${base}.${dt.getUTCMilliseconds().toString().padStart(3, "0")}Z`;
      if (this.constructor.PRECISION === Precision.US) {
        const total_us = dt.getUTCMilliseconds() * 1000 + (dt.__us || 0);
        return `${base}.${total_us.toString().padStart(6, "0")}Z`;
      }
      return `${base}Z`;
    }
    /**
     * Returns a human-friendly hex string with hyphens (e.g. 0000-0000-0000-0000).
     * @returns {string}
     */
    formatted() {
      const width =
        this.constructor.T_BITS +
        this.constructor.N_BITS +
        this.constructor.S_BITS +
        (this.constructor.SIGNED ? 1 : 0);
      const hexLen = width > 32 ? 16 : 8;
      const h = this.value.toString(16).toUpperCase().padStart(hexLen, "0");
      const parts = [];
      for (let i = 0; i < h.length; i += 4) parts.push(h.substring(i, i + 4));
      return parts.join("-");
    }
    /** @returns {BigInt} */
    toBigInt() {
      return this.value;
    }
    /** @returns {string} */
    toString() {
      return this.formatted();
    }
    /** @returns {string} JSON-compatible string. */
    toJSON() {
      return this.value.toString();
    }
    /** @returns {BigInt} */
    valueOf() {
      return this.value;
    }
  }

  /**
   * Base class for 32-bit variants.
   * Provides the `toNumber()` method as 32-bit values are safe for JavaScript Numbers.
   */
  class Chrono32Base extends ChronoBase {
    /** @returns {number} */
    toNumber() {
      return Number(this.value);
    }
  }

  /**
   * Specialized variant base for "Big Second" (BS) types which contain no entropy bits.
   * Useful for coarse time-tracking.
   * @private
   */
  class ChronoBasebs extends Chrono32Base {
    static fromPersona(dt, node_id = 0, seq = 0, persona = null, ts = null) {
      if (!dt) throw new ChronoError("Input date is null");
      if (Math.floor(dt.getTime() / 1000) < EPOCH_2020)
        throw new ChronoError(
          "Timestamp underflow: Date is before Epoch (2020-01-01)",
        );
      const ts_val =
        (ts !== null ? BigInt(ts) : this.PRECISION_FUNC(dt)) & this.T_MASK;
      return new this(ts_val);
    }
  }

  // --- Precision Multi-Variant Definitions ---
  // Each variant defines its precision level, bit-layout, and signedness.
  const VARIANTS = {
    UChrono64mo: { p: Precision.MO, t: 12, n: 26, s: 26, signed: false },
    Chrono64mo: { p: Precision.MO, t: 12, n: 25, s: 26 },
    UChrono64w: { p: Precision.W, t: 14, n: 26, s: 24, signed: false },
    Chrono64w: { p: Precision.W, t: 14, n: 25, s: 24 },
    UChrono64d: { p: Precision.D, t: 17, n: 24, s: 23, signed: false },
    Chrono64d: { p: Precision.D, t: 17, n: 23, s: 23 },
    UChrono64h: { p: Precision.H, t: 21, n: 22, s: 21, signed: false },
    Chrono64h: { p: Precision.H, t: 21, n: 21, s: 21 },
    UChrono64m: { p: Precision.M, t: 27, n: 19, s: 18, signed: false },
    Chrono64m: { p: Precision.M, t: 27, n: 18, s: 18 },
    UChrono64s: { p: Precision.S, t: 33, n: 16, s: 15, signed: false },
    Chrono64s: { p: Precision.S, t: 33, n: 15, s: 15 },
    UChrono64ds: { p: Precision.DS, t: 36, n: 15, s: 13, signed: false },
    Chrono64ds: { p: Precision.DS, t: 36, n: 14, s: 13 },
    UChrono64cs: { p: Precision.CS, t: 40, n: 12, s: 12, signed: false },
    Chrono64cs: { p: Precision.CS, t: 40, n: 11, s: 12 },
    UChrono64ms: { p: Precision.MS, t: 43, n: 11, s: 10, signed: false },
    Chrono64ms: { p: Precision.MS, t: 43, n: 10, s: 10 },
    UChrono64us: { p: Precision.US, t: 53, n: 6, s: 5, signed: false },
    Chrono64us: { p: Precision.US, t: 53, n: 5, s: 5 },
    UChrono32y: { p: Precision.Y, t: 8, n: 13, s: 11, signed: false },
    Chrono32y: { p: Precision.Y, t: 8, n: 12, s: 11 },
    UChrono32hy: { p: Precision.HY, t: 9, n: 12, s: 11, signed: false },
    Chrono32hy: { p: Precision.HY, t: 9, n: 11, s: 11 },
    UChrono32q: { p: Precision.Q, t: 10, n: 11, s: 11, signed: false },
    Chrono32q: { p: Precision.Q, t: 10, n: 10, s: 11 },
    UChrono32mo: { p: Precision.MO, t: 12, n: 10, s: 10, signed: false },
    Chrono32mo: { p: Precision.MO, t: 12, n: 9, s: 10 },
    UChrono32w: { p: Precision.W, t: 14, n: 9, s: 9, signed: false },
    Chrono32w: { p: Precision.W, t: 14, n: 8, s: 9 },
    UChrono32d: { p: Precision.D, t: 17, n: 8, s: 7, signed: false },
    Chrono32d: { p: Precision.D, t: 17, n: 7, s: 7 },
    UChrono32h: { p: Precision.H, t: 22, n: 5, s: 5, signed: false },
    Chrono32h: { p: Precision.H, t: 22, n: 4, s: 5 },
    UChrono32tm: { p: Precision.TM, t: 24, n: 4, s: 4, signed: false },
    Chrono32tm: { p: Precision.TM, t: 24, n: 3, s: 4 },
    UChrono32m: { p: Precision.M, t: 28, n: 2, s: 2, signed: false },
    Chrono32m: { p: Precision.M, t: 28, n: 1, s: 2 },
    UChrono32bs: {
      p: Precision.BS,
      t: 32,
      n: 0,
      s: 0,
      signed: false,
      base: ChronoBasebs,
    },
    Chrono32bs: { p: Precision.BS, t: 31, n: 0, s: 0, base: ChronoBasebs },
  };

  // Create and export classes for each defined variant.
  const Exports = {
    ChronoError,
    Persona,
    Generator,
    ChronoBase,
    Chrono32Base,
    Precision,
  };
  const US_MAP = [
    0n,
    0n,
    0n,
    0n,
    604800000000n,
    86400000000n,
    3600000000n,
    600000000n,
    60000000n,
    2000000n,
    1000000n,
    100000n,
    10000n,
    1000n,
    1n,
  ];

  for (const [name, cfg] of Object.entries(VARIANTS)) {
    // Select base class: Use cfg.base if provided, otherwise Chrono32Base for 32-bit, or ChronoBase for 64-bit.
    const width = cfg.t + cfg.n + cfg.s + (cfg.signed !== false ? 1 : 0);
    const Parent = cfg.base || (width <= 32 ? Chrono32Base : ChronoBase);

    const Cls = class extends Parent {};
    Cls.PRECISION = cfg.p;
    Cls.T_BITS = cfg.t;
    Cls.N_BITS = cfg.n;
    Cls.S_BITS = cfg.s;
    Cls.SIGNED = cfg.signed !== undefined ? cfg.signed : true;

    // Precompute constant properties once at class initialization
    Cls.T_MASK = (1n << BigInt(cfg.t)) - 1n;
    Cls.N_MASK = (1n << BigInt(cfg.n)) - 1n;
    Cls.S_MASK = (1n << BigInt(cfg.s)) - 1n;
    Cls.T_SHIFT = BigInt(cfg.n + cfg.s);
    Cls.N_MULT = _getWeylMults(cfg.n);
    Cls.S_MULT = _getWeylMults(cfg.s);
    Cls.PRECISION_FUNC = TS_COMPUTE[cfg.p];
    Cls.REVERSE_FUNC = TS_REVERSE[cfg.p];
    Cls.PRECISION_US = US_MAP[cfg.p];
    Cls.RAND_MASK = (1n << BigInt(cfg.n + cfg.s)) - 1n;
    Cls.SHIFT = Cls.T_SHIFT; // Legacy support
    Cls.MASK = Cls.T_MASK; // Legacy support

    Object.defineProperty(Cls, "name", { value: name });
    Exports[name] = Cls;
  }
  return Exports;
});
