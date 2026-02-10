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

// --- Universal Random Generator ---
// Tries Web Crypto -> Node Crypto -> Math.random fallback
const getRandomBits = (bits) => {
    const bytesNeeded = Math.ceil(bits / 8);
    let buffer;

    try {
        if (typeof crypto !== 'undefined' && crypto.getRandomValues) {
            // Browser / Modern Node
            buffer = new Uint8Array(bytesNeeded);
            crypto.getRandomValues(buffer);
        } else if (typeof require === 'function') {
            // Legacy Node
            const nodeCrypto = require('crypto');
            buffer = nodeCrypto.randomBytes(bytesNeeded);
        }
    } catch (e) {
        // Fallthrough
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

class UChrono32 extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const days = (ms - EPOCH_2000_MS) / 86400000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(14);
        return ((days & 0x3FFFFn) << 14n) | (r & 0x3FFFn);
    }
    static _unpack(val) {
        const days = val >> 14n;
        return EPOCH_2000_MS + (days * 86400000n);
    }
}

class Chrono32 extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const days = (ms - EPOCH_2000_MS) / 86400000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(13);
        return ((days & 0x3FFFFn) << 13n) | (r & 0x1FFFn);
    }
    static _unpack(val) {
        const days = val >> 13n;
        return EPOCH_2000_MS + (days * 86400000n);
    }
}

class UChrono32h extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const hours = (ms - EPOCH_2000_MS) / 3600000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(11);
        return ((hours & 0x1FFFFFn) << 11n) | (r & 0x7FFn);
    }
    static _unpack(val) {
        const hours = val >> 11n;
        return EPOCH_2000_MS + (hours * 3600000n);
    }
}

class Chrono32h extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const hours = (ms - EPOCH_2000_MS) / 3600000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(10);
        return ((hours & 0x1FFFFFn) << 10n) | (r & 0x3FFn);
    }
    static _unpack(val) {
        const hours = val >> 10n;
        return EPOCH_2000_MS + (hours * 3600000n);
    }
}

class UChrono32m extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const mins = (ms - EPOCH_2000_MS) / 60000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(5);
        return ((mins & 0x7FFFFFFn) << 5n) | (r & 0x1Fn);
    }
    static _unpack(val) {
        const mins = val >> 5n;
        return EPOCH_2000_MS + (mins * 60000n);
    }
}

class Chrono32m extends Chrono32Base {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const mins = (ms - EPOCH_2000_MS) / 60000n;
        const r = rand !== null ? BigInt(rand) : getRandomBits(4);
        return ((mins & 0x7FFFFFFn) << 4n) | (r & 0xFn);
    }
    static _unpack(val) {
        const mins = val >> 4n;
        return EPOCH_2000_MS + (mins * 60000n);
    }
}

// ==========================================
// 64-BIT FAMILY (Epoch 1970)
// ==========================================

class UChrono64 extends ChronoBase {
    static _pack(date, rand = null) {
        const secs = BigInt(Math.floor(date.getTime() / 1000));
        const r = rand !== null ? BigInt(rand) : getRandomBits(28);
        return ((secs & 0xFFFFFFFFFn) << 28n) | (r & 0xFFFFFFFn);
    }
    static _unpack(val) {
        return (val >> 28n) * 1000n;
    }
}

class Chrono64 extends ChronoBase {
    static _pack(date, rand = null) {
        const secs = BigInt(Math.floor(date.getTime() / 1000));
        const r = rand !== null ? BigInt(rand) : getRandomBits(27);
        return ((secs & 0xFFFFFFFFFn) << 27n) | (r & 0x7FFFFFFn);
    }
    static _unpack(val) {
        return (val >> 27n) * 1000n;
    }
}

class UChrono64ms extends ChronoBase {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const r = rand !== null ? BigInt(rand) : getRandomBits(20);
        return ((ms & 0xFFFFFFFFFFFn) << 20n) | (r & 0xFFFFFn);
    }
    static _unpack(val) {
        return val >> 20n;
    }
}

class Chrono64ms extends ChronoBase {
    static _pack(date, rand = null) {
        const ms = BigInt(date.getTime());
        const r = rand !== null ? BigInt(rand) : getRandomBits(19);
        return ((ms & 0xFFFFFFFFFFFn) << 19n) | (r & 0x7FFFFn);
    }
    static _unpack(val) {
        return val >> 19n;
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
} else if (typeof window !== 'undefined') {
    window.ChronoID = {
        UChrono32, Chrono32,
        UChrono32h, Chrono32h,
        UChrono32m, Chrono32m,
        UChrono64, Chrono64,
        UChrono64ms, Chrono64ms
    };
}