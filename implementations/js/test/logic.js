/**
 * Shared test logic for Chrono-ID.
 * This file is designed to be used by both Node.js (node:test)
 * and Browser-based test runners.
 */

function defineTests(describe, it, assert, ChronoID) {
  describe("Chrono-ID Implementation", () => {
    const ALL_VARIANTS = Object.keys(ChronoID).filter(
      (k) =>
        (k.startsWith("Chrono") || k.startsWith("UChrono")) &&
        typeof ChronoID[k] === "function" &&
        ![
          "ChronoError",
          "ChronoBase",
          "Chrono32Base",
          "ChronoBasebs",
          "Persona",
          "Generator",
        ].includes(k),
    );

    it("All variants instantiation and tier properties", () => {
      ALL_VARIANTS.forEach((name) => {
        const Cls = ChronoID[name];
        const id = new Cls();
        assert.ok(
          id.toBigInt() >= 0n,
          `${name} should have a non-negative value`,
        );

        const width =
          Cls.T_BITS + Cls.N_BITS + Cls.S_BITS + (Cls.SIGNED ? 1 : 0);
        if (width <= 32) {
          assert.ok(
            typeof id.toNumber === "function",
            `32-bit variant ${name} should have toNumber()`,
          );
          assert.ok(
            id.toNumber() >= 0,
            `32-bit variant ${name} toNumber() should be non-negative`,
          );
        } else {
          assert.strictEqual(
            id.toNumber,
            undefined,
            `64-bit variant ${name} should NOT have toNumber()`,
          );
        }
      });
    });

    it("Instantiation from value", () => {
      const val = 123456789n;
      const id32 = new ChronoID.UChrono32d(val);
      assert.strictEqual(id32.toBigInt(), val);
      assert.strictEqual(id32.toNumber(), Number(val));

      const id64 = new ChronoID.Chrono64s(val);
      assert.strictEqual(id64.toBigInt(), val);
    });

    it("fromTime and getTime accuracy", () => {
      const date = new Date("2023-01-01T12:00:00Z");

      const accuracyMap = {
        Y: 366 * 86400000,
        HY: 366 * 86400000,
        Q: 366 * 86400000,
        MO: 31 * 86400000,
        W: 604800000,
        D: 86400000,
        H: 3600000,
        TM: 600000,
        M: 60000,
        BS: 2000,
        S: 1000,
        DS: 100,
        CS: 10,
        MS: 1,
        US: 1,
      };

      ALL_VARIANTS.forEach((name) => {
        const Cls = ChronoID[name];
        const id = Cls.fromTime(date, 0n);
        const decodedDate = id.getTime();
        const diff = Math.abs(decodedDate.getTime() - date.getTime());

        // Get precision symbol from enum if possible or mapping
        const precKey = Object.keys(ChronoID.Precision || {}).find(
          (k) => (ChronoID.Precision || {})[k] === Cls.PRECISION,
        );
        const precision = accuracyMap[precKey] || 1000;

        assert.ok(
          diff <= precision,
          `${name} failed precision check: ${diff} > ${precision}`,
        );
      });
    });

    it("Time flooring accuracy", () => {
      const date = new Date("2023-01-01T12:30:45.999Z");

      // Minute precision
      assert.strictEqual(
        ChronoID.Chrono32m.fromTime(date, 0n).getTime().toISOString(),
        "2023-01-01T12:30:00.000Z",
      );

      // Hour precision
      assert.strictEqual(
        ChronoID.Chrono32h.fromTime(date, 0n).getTime().toISOString(),
        "2023-01-01T12:00:00.000Z",
      );

      // Day precision
      assert.strictEqual(
        ChronoID.Chrono32d.fromTime(date, 0n).getTime().toISOString(),
        "2023-01-01T00:00:00.000Z",
      );
    });

    it("Epoch boundaries", () => {
      const epoch2020 = new Date("2020-01-01T00:00:00Z");

      ALL_VARIANTS.forEach((name) => {
        const Cls = ChronoID[name];
        const id = Cls.fromTime(epoch2020, 0n);
        assert.strictEqual(
          id.toBigInt() >> Cls.T_SHIFT,
          0n,
          `${name} epoch boundary check failed`,
        );
      });
    });

    it("Expiry boundary for 32-bit", () => {
      const epoch2020 = 1577836800000n;
      // Test a few 32-bit variants for expiry
      const testExpiry = (Cls, unitMs) => {
        const maxUnits = Cls.T_MASK;
        const maxDate = new Date(Number(epoch2020 + maxUnits * BigInt(unitMs)));
        const id = Cls.fromTime(maxDate, 0n);
        assert.strictEqual(
          id.toBigInt() >> Cls.T_SHIFT,
          maxUnits,
          `${Cls.name} expiry check failed`,
        );
      };

      testExpiry(ChronoID.Chrono32d, 86400000);
      testExpiry(ChronoID.Chrono32h, 3600000);
    });

    it("K-Sortability", () => {
      const d1 = new Date("2023-01-01T10:00:00Z");
      const d2 = new Date("2023-01-01T11:00:00Z");

      const id1 = ChronoID.Chrono64s.fromTime(d1);
      const id2 = ChronoID.Chrono64s.fromTime(d2);

      assert.ok(id1.toBigInt() < id2.toBigInt());

      // Same time, different entropy/persona
      const id1Alt = ChronoID.Chrono64s.fromTime(d1);
      assert.notStrictEqual(id1.toBigInt(), id1Alt.toBigInt());
      assert.strictEqual(
        id1.toBigInt() >> ChronoID.Chrono64s.SHIFT,
        id1Alt.toBigInt() >> ChronoID.Chrono64s.SHIFT,
      );
    });

    it("Serialization support", () => {
      ALL_VARIANTS.forEach((name) => {
        const id = new ChronoID[name](12345n);
        assert.strictEqual(JSON.stringify(id), '"12345"');
        assert.strictEqual(id.toJSON(), "12345");
        assert.strictEqual(id.toString(), id.formatted());
      });
    });

    it("Random values are unique", () => {
      const date = new Date();
      ALL_VARIANTS.filter((n) => ChronoID[n].S_BITS > 0).forEach((name) => {
        const id1 = ChronoID[name].fromTime(date);
        const id2 = ChronoID[name].fromTime(date);
        assert.notStrictEqual(
          id1.toBigInt(),
          id2.toBigInt(),
          `${name} failed uniqueness check`,
        );
      });
    });

    it("Invalid date throwing error", () => {
      assert.throws(
        () => ChronoID.Chrono64ms.fromTime("not a date"),
        ChronoID.ChronoError,
        /Invalid date object/,
      );
    });

    it("Constructor with specific value", () => {
      const id = new ChronoID.Chrono32d("100");
      assert.strictEqual(id.toBigInt(), 100n);
    });

    it("fromISOString and standardized errors", () => {
      // Valid ISO
      const iso = "2023-05-20T10:30:00.123Z";
      const id = ChronoID.Chrono64ms.fromISOString(iso);
      assert.strictEqual(id.getTime().toISOString(), iso);

      // Naive ISO string should be treated as UTC
      const naiveIso = "2023-05-20T10:30:00.123";
      const idNaive = ChronoID.Chrono64ms.fromISOString(naiveIso);
      assert.strictEqual(idNaive.getTime().toISOString(), iso);

      // Underflow (Pre-2020)
      assert.throws(
        () => ChronoID.Chrono64s.fromISOString("2019-12-31T23:59:59Z"),
        ChronoID.ChronoError,
        /Date is before Epoch/,
      );
    });
  });
}

// Export for Node.js or Globals for Browser
if (typeof module !== "undefined" && module.exports) {
  module.exports = { defineTests };
} else {
  window.defineTests = defineTests;
}
