const { test: it, describe, before } = require('node:test');
const assert = require('node:assert');
const fs = require('fs');
const path = require('path');
const ChronoID = require('../src/index');

describe('Cross-Platform JSON Tests', () => {
    let testData;
    const variantMap = {
        "chrono32": ChronoID.Chrono32, "uchrono32": ChronoID.UChrono32,
        "chrono32h": ChronoID.Chrono32h, "uchrono32h": ChronoID.UChrono32h,
        "chrono32m": ChronoID.Chrono32m, "uchrono32m": ChronoID.UChrono32m,
        "chrono32w": ChronoID.Chrono32w, "uchrono32w": ChronoID.UChrono32w,
        "chrono64": ChronoID.Chrono64, "uchrono64": ChronoID.UChrono64,
        "chrono64ms": ChronoID.Chrono64ms, "uchrono64ms": ChronoID.UChrono64ms,
        "chrono64us": ChronoID.Chrono64us, "uchrono64us": ChronoID.UChrono64us
    };

    before(() => {
        const jsonPath = path.join(__dirname, '../../../tests/cross_platform_tests.json');
        testData = JSON.parse(fs.readFileSync(jsonPath, 'utf8'));
    });

    it('Valid Cases', () => {
        testData.valid_cases.forEach(case_set => {
            const iso = case_set.iso;
            case_set.variants.forEach(v_data => {
                const v_name = v_data.name;
                const Cls = variantMap[v_name];
                const obj = Cls.fromISOString(iso, 0n);

                if (v_data.expected_timestamp !== undefined) {
                    assert.strictEqual(obj.get_timestamp(), BigInt(v_data.expected_timestamp), `${v_name} timestamp mismatch`);
                }
                if (v_data.expected_iso) {
                    assert.strictEqual(obj.to_iso_string(), v_data.expected_iso, `${v_name} ISO mismatch`);
                }
            });
        });
    });

    it('Error Cases', () => {
        testData.error_cases.forEach(case_set => {
            const v_name = case_set.variant || "chrono64ms";
            const Cls = variantMap[v_name];
            const input = case_set.input;
            const expectedErr = case_set.expected_error;

            assert.throws(() => Cls.fromISOString(input), {
                name: 'ChronoError',
                message: expectedErr
            }, `Case ${case_set.name} failed`);
        });
    });
});
