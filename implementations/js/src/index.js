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
const EPOCH_2000_US = 946684800000000n; // Jan 1, 2000 in microseconds

/**
 * Custom error class for Chrono-ID library.
 */
class ChronoError extends Error {
    constructor(message) {
        super(message);
        this.name = 'ChronoError';
    }
}

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

// --- Time Detection ---
let getNowMicros;
const isNode = typeof process !== 'undefined' && process.hrtime && process.hrtime.bigint;

if (isNode) {
    // Node.js: Use process.hrtime.bigint() for monotonic microsecond precision
    const START_TIME_MS = BigInt(Date.now());
    const START_HR_NS = process.hrtime.bigint();
    getNowMicros = () => {
        const currentHrNs = process.hrtime.bigint();
        const diffNs = currentHrNs - START_HR_NS;
        return START_TIME_MS * 1000n + diffNs / 1000n;
    };
} else {
    // Browser: Use performance.now() if available, else fallback to Date.now()
    const perf = (typeof globalThis !== 'undefined' && globalThis.performance) ||
        (typeof window !== 'undefined' && window.performance);

    if (perf) {
        const origin = BigInt(Math.floor(perf.timeOrigin || Date.now()));
        getNowMicros = () => {
            // performance.now() returns fractional milliseconds
            return origin * 1000n + BigInt(Math.floor(perf.now() * 1000));
        };
    } else {
        getNowMicros = () => BigInt(Date.now()) * 1000n;
    }
}

// --- Universal Random Generator ---
const getRandomBits = (bits) => {
    const bytesNeeded = Math.ceil(bits / 8);
    let buffer;

    if (_webCrypto) {
        buffer = new Uint8Array(bytesNeeded);
        _webCrypto.getRandomValues(buffer);
    } else if (_nodeCrypto) {
        buffer = _nodeCrypto.randomBytes(bytesNeeded);
    }

    let val = 0n;
    if (buffer) {
        let hex = '0x';
        for (let i = 0; i < buffer.length; i++) {
            hex += buffer[i].toString(16).padStart(2, '0');
        }
        val = BigInt(hex);
    } else {
        /* v8 ignore next 1 */
        val = BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER));
    }

    const mask = (1n << BigInt(bits)) - 1n;
    return val & mask;
};

// --- Base Class ---
class ChronoBase {
    static SHIFT = 0n;
    static MASK = 0n;
    static RAND_MASK = 0n;
    static PRECISION_US = 1000n; // Standard: Milliseconds (1000 us)
    static EPOCH_US = 0n;

    constructor(value) {
        if (value === undefined || value === null) {
            this.value = this.constructor._pack(getNowMicros());
        } else {
            this.value = BigInt(value);
        }
    }

    static fromTime(date, randomVal = null, us_offset = 0n) {
        if (!date) throw new ChronoError("Input date is null");
        if (!(date instanceof Date) || isNaN(date.getTime())) throw new ChronoError("Invalid date object");

        const ms = BigInt(date.getTime());
        const us = (ms * 1000n) + us_offset;
        const epoch_us = this.EPOCH_US;

        if (us < epoch_us) {
            if (this.EPOCH_US > 0n) {
                throw new ChronoError("Timestamp underflow: Date is before Epoch (32-bit types require 2000-01-01 or later)");
            } else {
                throw new ChronoError("Timestamp underflow: Date is before Unix Epoch (1970-01-01)");
            }
        }

        return new this(this._pack(us, randomVal));
    }

    static fromISOString(iso, randomVal = null) {
        if (iso === null || iso === undefined) throw new ChronoError("Input string is null");
        const date = new Date(iso);
        if (isNaN(date.getTime())) throw new ChronoError("Invalid ISO 8601 format");

        // Extract microseconds if present in the string
        let us_offset = 0n;
        const match = iso.match(/\.(\d+)/);
        if (match) {
            let frac = match[1];
            while (frac.length < 6) frac += '0';
            if (frac.length > 6) frac = frac.substring(0, 6);
            us_offset = BigInt(frac) % 1000n; // Only the part beyond millisecond
        }

        return this.fromTime(date, randomVal, us_offset);
    }

    static _pack(us, rand = null) {
        const units = (us - this.EPOCH_US) / this.PRECISION_US;
        const r = rand !== null ? BigInt(rand) : getRandomBits(Number(this.SHIFT));
        return ((units & this.MASK) << this.SHIFT) | (r & this.RAND_MASK);
    }

    static _unpack(val) {
        const units = val >> this.SHIFT;
        return this.EPOCH_US + (units * this.PRECISION_US);
    }

    getTime() {
        const unixUs = this.constructor._unpack(this.value);
        return new Date(Number(unixUs / 1000n));
    }

    to_iso_string() {
        const dt = this.getTime();
        const base = dt.toISOString().split('.')[0];
        const unixUs = this.constructor._unpack(this.value);

        if (this.constructor.PRECISION_US === 1n) {
            const us = Number(unixUs % 1000000n);
            return `${base}.${us.toString().padStart(6, '0')}Z`;
        } else if (this.constructor.PRECISION_US === 1000n) {
            const ms = Number((unixUs / 1000n) % 1000n);
            return `${base}.${ms.toString().padStart(3, '0')}Z`;
        }
        return `${base}Z`;
    }

    get_timestamp() {
        return this.value >> this.constructor.SHIFT;
    }

    toBigInt() { return this.value; }
    toString() { return this.value.toString(); }
    toJSON() { return this.value.toString(); }
}

// --- Base Class for 32 bit ---
class Chrono32Base extends ChronoBase {
    static EPOCH_US = EPOCH_2000_US;
    toNumber() { return Number(this.value); }
}

// ==========================================
// 32-BIT FAMILY (Epoch 2000)
// ==========================================

class UChrono32 extends Chrono32Base {
    static SHIFT = 14n;
    static MASK = 0x3FFFFn; // 18b
    static RAND_MASK = 0x3FFFn; // 14b
    static PRECISION_US = 86400000000n; // Day
}

class Chrono32 extends Chrono32Base {
    static SHIFT = 13n;
    static MASK = 0x3FFFFn; // 18b
    static RAND_MASK = 0x1FFFn; // 13b
    static PRECISION_US = 86400000000n; // Day
}

class UChrono32h extends Chrono32Base {
    static SHIFT = 11n;
    static MASK = 0x1FFFFFn;
    static RAND_MASK = 0x7FFn;
    static PRECISION_US = 3600000000n; // Hour
}

class Chrono32h extends Chrono32Base {
    static SHIFT = 10n;
    static MASK = 0x1FFFFFn;
    static RAND_MASK = 0x3FFn;
    static PRECISION_US = 3600000000n; // Hour
}

class UChrono32m extends Chrono32Base {
    static SHIFT = 5n;
    static MASK = 0x7FFFFFFn;
    static RAND_MASK = 0x1Fn;
    static PRECISION_US = 60000000n; // Minute
}

class Chrono32m extends Chrono32Base {
    static SHIFT = 4n;
    static MASK = 0x7FFFFFFn;
    static RAND_MASK = 0xFn;
    static PRECISION_US = 60000000n; // Minute
}

class UChrono32w extends Chrono32Base {
    static SHIFT = 18n;
    static MASK = 0x3FFFn; // 14b
    static RAND_MASK = 0x3FFFFn; // 18b
    static PRECISION_US = 604800000000n; // Week
}

class Chrono32w extends Chrono32Base {
    static SHIFT = 17n;
    static MASK = 0x3FFFn; // 14b
    static RAND_MASK = 0x1FFFFn; // 17b
    static PRECISION_US = 604800000000n; // Week
}

// ==========================================
// 64-BIT FAMILY (Epoch 1970)
// ==========================================

class UChrono64 extends ChronoBase {
    static SHIFT = 28n;
    static MASK = 0xFFFFFFFFFn; // 36b
    static RAND_MASK = 0xFFFFFFFn; // 28b
    static PRECISION_US = 1000000n; // Second
}

class Chrono64 extends ChronoBase {
    static SHIFT = 27n;
    static MASK = 0xFFFFFFFFFn; // 36b
    static RAND_MASK = 0x7FFFFFFn; // 27b
    static PRECISION_US = 1000000n; // Second
}

class UChrono64ms extends ChronoBase {
    static SHIFT = 20n;
    static MASK = 0xFFFFFFFFFFFn;
    static RAND_MASK = 0xFFFFFn;
    static PRECISION_US = 1000n; // Millisecond
}

class Chrono64ms extends ChronoBase {
    static SHIFT = 19n;
    static MASK = 0xFFFFFFFFFFFn;
    static RAND_MASK = 0x7FFFFn;
    static PRECISION_US = 1000n; // Millisecond
}

class UChrono64us extends ChronoBase {
    static SHIFT = 10n;
    static MASK = 0x3FFFFFFFFFFFFFn;
    static RAND_MASK = 0x3FFn;
    static PRECISION_US = 1n; // Microsecond
}

class Chrono64us extends ChronoBase {
    static SHIFT = 9n;
    static MASK = 0x3FFFFFFFFFFFFFn;
    static RAND_MASK = 0x1FFn;
    static PRECISION_US = 1n; // Microsecond
}

// Export
const exported = {
    ChronoError,
    UChrono32, Chrono32,
    UChrono32h, Chrono32h,
    UChrono32m, Chrono32m,
    UChrono32w, Chrono32w,
    UChrono64, Chrono64,
    UChrono64ms, Chrono64ms,
    UChrono64us, Chrono64us
};

if (typeof module !== 'undefined' && module.exports) {
    module.exports = exported;
} else if (typeof window !== 'undefined') {
    window.ChronoID = exported;
}