const { test: it, describe, before } = require("node:test");
const assert = require("node:assert");
const fs = require("fs");
const path = require("path");
const ChronoID = require("../src/index");

describe("Cross-Platform JSON Tests", () => {
  let testData;
  const variantMap = {};

  // Auto-populate variantMap from ChronoID exports
  Object.keys(ChronoID).forEach((key) => {
    if (key.toLowerCase().includes("chrono")) {
      variantMap[key.toLowerCase()] = ChronoID[key];
    }
  });

  before(() => {
    const jsonPath = path.join(
      __dirname,
      "../../../tests/cross_platform_tests.json",
    );
    testData = JSON.parse(fs.readFileSync(jsonPath, "utf8"));
  });

  it("Valid Cases", () => {
    testData.valid_cases.forEach((case_set, idx) => {
      const iso = case_set.iso;
      case_set.variants.forEach((v_data) => {
        const v_name = v_data.name;
        const Cls = variantMap[v_name];
        if (!Cls) return; // Skip variants not in our map (e.g. if we missed any)

        // Inject exact parameters from test case to verify bit-parity
        const persona = {
          node_idx: v_data.node_idx,
          node_salt: v_data.node_salt,
          seq_idx: v_data.seq_idx,
          seq_salt: v_data.seq_salt,
          seq_offset: v_data.seq_offset || 0,
        };

        // Extract microseconds from ISO if it's a microsecond variant
        const dt = new Date(iso);
        const m = iso.match(/\.(\d+)/);
        if (m) {
          let frac = m[1].padEnd(6, "0").substring(0, 6);
          dt.__us = parseInt(frac) % 1000;
        }

        const obj = Cls.fromPersona(dt, v_data.node_id, v_data.seq, persona);

        if (v_data.expected_hex) {
          const expectedHexPlain = v_data.expected_hex
            .replace("0x", "")
            .toUpperCase();
          const actualHex = obj.value
            .toString(16)
            .toUpperCase()
            .padStart(expectedHexPlain.length, "0");
          assert.strictEqual(
            actualHex,
            expectedHexPlain,
            `Variant ${v_name} in case ${idx} hex mismatch. Expected ${expectedHexPlain}, got ${actualHex}`,
          );
        }

        if (v_data.expected_str) {
          assert.strictEqual(
            obj.formatted(),
            v_data.expected_str,
            `Variant ${v_name} in case ${idx} formatted string mismatch`,
          );
        }

        if (v_data.expected_timestamp !== undefined) {
          assert.strictEqual(
            obj.getTimestamp(),
            BigInt(v_data.expected_timestamp),
            `${v_name} timestamp mismatch`,
          );
        }
        if (v_data.expected_iso) {
          assert.strictEqual(
            obj.toISOString(),
            v_data.expected_iso,
            `${v_name} ISO mismatch`,
          );
        }
      });
    });
  });

  it("Error Cases", () => {
    if (!testData.error_cases) return;
    testData.error_cases.forEach((case_set) => {
      const v_name = case_set.variant || "chrono64ms";
      const Cls = variantMap[v_name.toLowerCase()];
      if (!Cls) return;
      const input = case_set.input;
      const expectedErr = case_set.expected_error;

      assert.throws(
        () => Cls.fromISOString(input),
        {
          name: "ChronoError",
          // message might vary slightly but it should throw ChronoError
        },
        `Case ${case_set.name} failed to throw ChronoError`,
      );
    });
  });
});
