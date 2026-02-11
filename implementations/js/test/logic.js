/**
 * Shared test logic for Chrono-ID.
 * This file is designed to be used by both Node.js (node:test) 
 * and Browser-based test runners.
 */

function defineTests(describe, it, assert, ChronoID) {
    describe('Chrono-ID Implementation', () => {

        it('ChronoBase instantiation', () => {
            const id = new ChronoID.Chrono64ms();
            assert.ok(id.toBigInt() > 0n);
            assert.ok(typeof id.toString() === 'string');
            assert.ok(id instanceof ChronoID.UChrono64ms || id instanceof ChronoID.Chrono64ms);
        });

        it('Instantiation from value', () => {
            const val = 123456789n;
            const id = new ChronoID.UChrono32(val);
            assert.strictEqual(id.toBigInt(), val);
            assert.strictEqual(id.toNumber(), Number(val));
        });

        it('fromTime and getTime accuracy', () => {
            const date = new Date('2023-01-01T12:00:00Z');

            const variants = [
                { cls: ChronoID.UChrono32, precision: 86400000 },
                { cls: ChronoID.Chrono32, precision: 86400000 },
                { cls: ChronoID.UChrono32h, precision: 3600000 },
                { cls: ChronoID.Chrono32h, precision: 3600000 },
                { cls: ChronoID.UChrono32m, precision: 60000 },
                { cls: ChronoID.Chrono32m, precision: 60000 },
                { cls: ChronoID.UChrono64, precision: 1000 },
                { cls: ChronoID.Chrono64, precision: 1000 },
                { cls: ChronoID.UChrono64ms, precision: 1 },
                { cls: ChronoID.Chrono64ms, precision: 1 },
                { cls: ChronoID.UChrono64us, precision: 1 }, // precision in ms for accuracy check
                { cls: ChronoID.Chrono64us, precision: 1 },
                { cls: ChronoID.UChrono32w, precision: 604800000 }, // Week precision in ms
                { cls: ChronoID.Chrono32w, precision: 604800000 }
            ];

            for (const { cls, precision } of variants) {
                const id = cls.fromTime(date, 0n);
                const decodedDate = id.getTime();
                const diff = Math.abs(decodedDate.getTime() - date.getTime());
                assert.ok(diff <= precision, `${cls.name} failed precision check: ${diff} > ${precision}`);
            }
        });

        it('Time flooring accuracy', () => {
            const date = new Date('2023-01-01T12:30:45.999Z');

            // Minute precision
            const id_m = ChronoID.Chrono32m.fromTime(date, 0n);
            assert.strictEqual(id_m.getTime().toISOString(), '2023-01-01T12:30:00.000Z');

            // Hour precision
            const id_h = ChronoID.Chrono32h.fromTime(date, 0n);
            assert.strictEqual(id_h.getTime().toISOString(), '2023-01-01T12:00:00.000Z');

            // Day precision
            const id_d = ChronoID.Chrono32.fromTime(date, 0n);
            assert.strictEqual(id_d.getTime().toISOString(), '2023-01-01T00:00:00.000Z');
        });

        it('Epoch boundaries', () => {
            // 32-bit Epoch: Jan 1, 2000
            const epoch32 = new Date('2000-01-01T00:00:00Z');
            const id32 = ChronoID.Chrono32.fromTime(epoch32, 0n);
            // Should be 0 in time bits (unshift SHIFT)
            assert.strictEqual(id32.toBigInt() >> BigInt(ChronoID.Chrono32.SHIFT), 0n);

            // 64-bit Epoch: Jan 1, 1970
            const epoch64 = new Date('1970-01-01T00:00:00Z');
            const id64 = ChronoID.Chrono64.fromTime(epoch64, 0n);
            // Should be 0 in time bits
            assert.strictEqual(id64.toBigInt() >> BigInt(ChronoID.Chrono64.SHIFT), 0n);
        });

        it('Expiry boundary', () => {
            // Max days allowed (18 bits)
            const maxUnits = BigInt(ChronoID.Chrono32.MASK);
            const epoch2000 = 946684800000n;
            const maxDate = new Date(Number(epoch2000 + maxUnits * 86400000n));

            const id = ChronoID.Chrono32.fromTime(maxDate, 0n);
            assert.strictEqual(id.toBigInt() >> BigInt(ChronoID.Chrono32.SHIFT), maxUnits);
        });

        it('K-Sortability', () => {
            const d1 = new Date('2023-01-01T10:00:00Z');
            const d2 = new Date('2023-01-01T11:00:00Z');

            const id1 = ChronoID.Chrono64.fromTime(d1);
            const id2 = ChronoID.Chrono64.fromTime(d2);

            assert.ok(id1.toBigInt() < id2.toBigInt());

            // Same time, different entropy
            const id1Alt = ChronoID.Chrono64.fromTime(d1);
            assert.notStrictEqual(id1.toBigInt(), id1Alt.toBigInt());
            assert.strictEqual(id1.toBigInt() >> BigInt(ChronoID.Chrono64.SHIFT), id1Alt.toBigInt() >> BigInt(ChronoID.Chrono64.SHIFT));
        });

        it('Serialization support', () => {
            const id = new ChronoID.Chrono64(12345n);
            assert.strictEqual(JSON.stringify(id), '"12345"');
            assert.strictEqual(id.toJSON(), '12345');
            assert.strictEqual(id.toString(), '12345');
        });

        it('Random values are unique', () => {
            const date = new Date();
            const id1 = ChronoID.Chrono64ms.fromTime(date);
            const id2 = ChronoID.Chrono64ms.fromTime(date);
            assert.notStrictEqual(id1.toBigInt(), id2.toBigInt());
        });

        it('Invalid date throwing error', () => {
            assert.throws(() => ChronoID.Chrono64ms.fromTime('not a date'), ChronoID.ChronoError, /Invalid date object/);
        });

        it('Constructor with specific value', () => {
            const id = new ChronoID.Chrono32('100');
            assert.strictEqual(id.toBigInt(), 100n);
        });

        it('fromISOString and standardized errors', () => {
            // Valid ISO
            const id = ChronoID.Chrono64ms.fromISOString('2023-05-20T10:30:00.123Z');
            assert.strictEqual(id.getTime().toISOString(), '2023-05-20T10:30:00.123Z');

            // Null/Undefined inputs
            assert.throws(() => ChronoID.Chrono64ms.fromISOString(null), ChronoID.ChronoError, /Input string is null/);
            assert.throws(() => ChronoID.Chrono64ms.fromISOString(undefined), ChronoID.ChronoError, /Input string is null/);
            assert.throws(() => ChronoID.Chrono32.fromTime(null), ChronoID.ChronoError, /Input date is null/);

            // Invalid format
            assert.throws(() => ChronoID.Chrono64ms.fromISOString('not-a-date'), ChronoID.ChronoError, /Invalid ISO 8601 format/);

            // Underflow 64-bit (Pre-1970)
            assert.throws(() => ChronoID.Chrono64.fromISOString('1960-01-01T00:00:00Z'), ChronoID.ChronoError, /Date is before Unix Epoch/);

            // Underflow 32-bit (Pre-2000)
            assert.throws(() => ChronoID.Chrono32.fromISOString('1999-12-31T23:59:59Z'), ChronoID.ChronoError, /Date is before Epoch/);
        });
    });
}

// Export for Node.js or Globals for Browser
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { defineTests };
} else {
    window.defineTests = defineTests;
}
