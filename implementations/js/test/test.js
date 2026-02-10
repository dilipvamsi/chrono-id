const { test: it, describe, after } = require('node:test');
const assert = require('node:assert');
const ChronoID = require('../src/index');
const { defineTests } = require('./logic');

describe('Chrono-ID JavaScript Implementation', () => {

    // Run shared tests
    defineTests(describe, it, assert, ChronoID);

    // Node-specific tests (e.g. environment fallback)
    describe('Node-specific Fallback Logic', () => {
        const originalCrypto = global.crypto;

        after(() => {
            global.crypto = originalCrypto;
        });

        it('Fallback to Math.random when crypto is unavailable', () => {
            const savedCrypto = global.crypto;
            delete global.crypto;

            const id = new ChronoID.Chrono64ms();
            assert.ok(id.toBigInt() > 0n);

            global.crypto = savedCrypto;
        });
    });
});
