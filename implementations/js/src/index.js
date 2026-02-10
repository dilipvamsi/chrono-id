/**
 * Chrono-ID: Type-safe, K-Sortable, Time-Ordered Unique Identifiers.
 *
 * Architecture:
 * - 32-bit Family: Uses Epoch 2000 (Jan 1, 2000) for >200 year longevity.
 * - 64-bit Family: Uses Unix Epoch (Jan 1, 1970) for standard compatibility.
 * - Signed (Chrono): MSB is always 0.
 * - Unsigned (UChrono): Uses full bit range.
 */

// --- Constants ---
const EPOCH_2000_MS = 946684800000n; // Jan 1, 2000 in ms

// --- Crypto Detection (Done once) ---
const _webCrypto = (typeof crypto !== 'undefined' && crypto.getRandomValues) ? crypto :
    (typeof window !== 'undefined' && window.crypto && window.crypto.getRandomValues) ? window.crypto :
        (typeof self !== 'undefined' && self.crypto && self.crypto.getRandomValues) ? self.crypto : null;

let _nodeCrypto = null;
/* v8 ignore next 6 */
if (!_webCrypto && typeof require === 'function') {
    try {
        _nodeCrypto = require('crypto');
    } catch (e) {
        // Fallback to Math.random
    }
}

// --- Universal Random Generator ---
const getRandomBits = (bits) => {
    const bytesNeeded = Math.ceil(bits / 8);
    let buffer;

    if (_webCrypto) {
        // Browser / Modern Node
        buffer = new Uint8Array(bytesNeeded);
        _webCrypto.getRandomValues(buffer);
    } else if (_nodeCrypto) {
        // Legacy Node
        buffer = _nodeCrypto.randomBytes(bytesNeeded);
    }

    let val = 0n;
    if (buffer) {
        // Hex conversion to BigInt
        let hex = '0x';
        for (let i = 0; i < buffer.length; i++) {
            hex += buffer[i].toString(16).padStart(2, '0');
        }
        val = BigInt(hex);
    } else {
        // Fallback: Math.random (Insecure but functional for fallback)
        /* v8 ignore next 1 */
        val = BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER));
    }

    // Mask to exact bit length
    const mask = (1n << BigInt(bits)) - 1n;
    return val & mask;
};

// --- Base Class ---
class ChronoBase {
    constructor(value) {
        // If no value provided, generate one based on current time
        if (value === undefined || value === null) {
            const now = new Date();
            const packed = this.constructor._pack(now);
            this.value = packed;
        } else {
            this.value = BigInt(value);
        }
    }

    /**
     * Creates an ID from a specific Date object.
     * @param {Date} date - The date to encode.
     * @param {number|bigint} [randomVal] - Optional specific random bits.
     */
    static fromTime(date, randomVal = null) {
        if (!(date instanceof Date)) throw new Error("Invalid date object");
        const packed = this._pack(date, randomVal);
        return new this(packed);
    }

    /**
     * Extracts the time from the current ID.
     * @returns {Date}
     */
    getTime() {
        const unixMs = this.constructor._unpack(this.value);
        return new Date(Number(unixMs));
    }

    /**
     * Returns the BigInt primitive value.
     * Allows usage like: Number(id) or id + 1n
     */
    toBigInt() {
        return this.value;
    }

    /**
     * Returns the string representation.
     */
    toString() {
        return this.value.toString();
    }

    /**
     * JSON serialization support.
     * Serializes as string to prevent precision loss in JS numbers.
     */
    toJSON() {
        return this.value.toString();
    }
}

// --- Base Class for 32 bit ---
class Chrono32Base extends ChronoBase {
    /**
     * Returns the Number of BigInt primitive value.
     */
    toNumber() {
        return Number(this.value);
    }
}

// ==========================================
// 32-BIT FAMILY (Epoch 2000)
// ==========================================

// ==========================================
// 32-BIT FAMILY (Epoch 2000)
// ==========================================

class UChrono32 extends Chrono32Base {
    static SHIFT = 14n;
    static MASK = 0x3FFFFn;
    static RAND_MASK = 0x3FFFn;
    static PRECISION_MS = 86400000n; // Day
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class Chrono32 extends Chrono32Base {
    static SHIFT = 13n;
    static MASK = 0x3FFFFn;
    static RAND_MASK = 0x1FFFn;
    static PRECISION_MS = 86400000n; // Day
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class UChrono32h extends Chrono32Base {
    static SHIFT = 11n;
    static MASK = 0x1FFFFFn;
    static RAND_MASK = 0x7FFn;
    static PRECISION_MS = 3600000n; // Hour
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class Chrono32h extends Chrono32Base {
    static SHIFT = 10n;
    static MASK = 0x1FFFFFn;
    static RAND_MASK = 0x3FFn;
    static PRECISION_MS = 3600000n; // Hour
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class UChrono32m extends Chrono32Base {
    static SHIFT = 5n;
    static MASK = 0x7FFFFFFn;
    static RAND_MASK = 0x1Fn;
    static PRECISION_MS = 60000n; // Minute
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class Chrono32m extends Chrono32Base {
    static SHIFT = 4n;
    static MASK = 0x7FFFFFFn;
    static RAND_MASK = 0xFn;
    static PRECISION_MS = 60000n; // Minute
    static EPOCH = EPOCH_2000_MS;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

// ==========================================
// 64-BIT FAMILY (Epoch 1970)
// ==========================================

class UChrono64 extends ChronoBase {
    static SHIFT = 28n;
    static MASK = 0xFFFFFFFFFn;
    static RAND_MASK = 0xFFFFFFFn;
    static PRECISION_MS = 1000n; // Second
    static EPOCH = 0n;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class Chrono64 extends ChronoBase {
    static SHIFT = 27n;
    static MASK = 0xFFFFFFFFFn;
    static RAND_MASK = 0x7FFFFFFn;
    static PRECISION_MS = 1000n; // Second
    static EPOCH = 0n;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class UChrono64ms extends ChronoBase {
    static SHIFT = 20n;
    static MASK = 0xFFFFFFFFFFFn;
    static RAND_MASK = 0xFFFFFn;
    static PRECISION_MS = 1n; // Millisecond
    static EPOCH = 0n;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

class Chrono64ms extends ChronoBase {
    static SHIFT = 19n;
    static MASK = 0xFFFFFFFFFFFn;
    static RAND_MASK = 0x7FFFFn;
    static PRECISION_MS = 1n; // Millisecond
    static EPOCH = 0n;

    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const units = (ms - this.EPOCH) / this.PRECISION_MS;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }
    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH + (units * this.PRECISION_MS);
    }
}

// Export for Node.js or Browser Global
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        UChrono32, Chrono32,
        UChrono32h, Chrono32h,
        UChrono32m, Chrono32m,
        UChrono64, Chrono64,
        UChrono64ms, Chrono64ms
    };
    /* v8 ignore next 9 */
} else if (typeof window !== 'undefined') {
    window.ChronoID = {
        UChrono32, Chrono32,
        UChrono32h, Chrono32h,
        UChrono32m, Chrono32m,
        UChrono64, Chrono64,
        UChrono64ms, Chrono64ms
    };
}