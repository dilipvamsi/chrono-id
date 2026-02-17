import json
import unittest
import os
import re
from datetime import datetime
from chrono_id import (
    UChrono32y,
    Chrono32y,
    UChrono32hy,
    Chrono32hy,
    UChrono32q,
    Chrono32q,
    UChrono32mo,
    Chrono32mo,
    UChrono32w,
    Chrono32w,
    UChrono32d,
    Chrono32d,
    UChrono32h,
    Chrono32h,
    UChrono32tm,
    Chrono32tm,
    UChrono32m,
    Chrono32m,
    UChrono64mo,
    Chrono64mo,
    UChrono64w,
    Chrono64w,
    UChrono64d,
    Chrono64d,
    UChrono64h,
    Chrono64h,
    UChrono64m,
    Chrono64m,
    UChrono64s,
    Chrono64s,
    UChrono64ds,
    Chrono64ds,
    UChrono64cs,
    Chrono64cs,
    UChrono64ms,
    Chrono64ms,
    UChrono64us,
    Chrono64us,
    ChronoError,
)


class TestCrossPlatformJSON(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        path = os.path.join(
            os.path.dirname(__file__), "../../../tests/cross_platform_tests.json"
        )
        with open(path, "r") as f:
            cls.data = json.load(f)

        cls.variant_map = {
            "chrono32d": Chrono32d,
            "uchrono32d": UChrono32d,
            "chrono32h": Chrono32h,
            "uchrono32h": UChrono32h,
            "chrono32m": Chrono32m,
            "uchrono32m": UChrono32m,
            "chrono32w": Chrono32w,
            "uchrono32w": UChrono32w,
            "chrono32y": Chrono32y,
            "uchrono32y": UChrono32y,
            "chrono32hy": Chrono32hy,
            "uchrono32hy": UChrono32hy,
            "chrono32q": Chrono32q,
            "uchrono32q": UChrono32q,
            "chrono32mo": Chrono32mo,
            "uchrono32mo": UChrono32mo,
            "chrono32tm": Chrono32tm,
            "uchrono32tm": UChrono32tm,
            "chrono64s": Chrono64s,
            "uchrono64s": UChrono64s,
            "chrono64ms": Chrono64ms,
            "uchrono64ms": UChrono64ms,
            "chrono64us": Chrono64us,
            "uchrono64us": UChrono64us,
            "chrono64mo": Chrono64mo,
            "uchrono64mo": UChrono64mo,
            "chrono64w": Chrono64w,
            "uchrono64w": UChrono64w,
            "chrono64d": Chrono64d,
            "uchrono64d": UChrono64d,
            "chrono64h": Chrono64h,
            "uchrono64h": UChrono64h,
            "chrono64m": Chrono64m,
            "uchrono64m": UChrono64m,
            "chrono64ds": Chrono64ds,
            "uchrono64ds": UChrono64ds,
            "chrono64cs": Chrono64cs,
            "uchrono64cs": UChrono64cs,
        }

    def test_valid_cases(self):
        from chrono_id.core import Persona

        for case in self.data["valid_cases"]:
            iso = case["iso"]
            for v_data in case["variants"]:
                v_name = v_data["name"]
                if v_name not in self.variant_map:
                    continue
                cls = self.variant_map[v_name]
                with self.subTest(iso=iso, variant=v_name):
                    if "node_idx" in v_data:
                        iso_clean = iso.replace("Z", "+00:00")
                        dt = datetime.fromisoformat(iso_clean)
                        p = Persona(s_bits=cls.S_BITS)
                        p.node_id = v_data.get("node_id", 0)
                        p.node_salt = v_data.get("node_salt", 0)
                        p.node_idx = v_data.get("node_idx", 0)
                        p.seq_offset = v_data.get("seq_offset", 0)
                        p.seq_idx = v_data.get("seq_idx", 0)
                        p.seq_salt = v_data.get("seq_salt", 0)

                        obj = cls.from_persona(
                            dt=dt,
                            node_id=p.node_id,
                            seq=v_data.get("seq", 0),
                            persona=p,
                        )
                    else:
                        obj = cls.from_iso_string(iso)

                    if "expected_timestamp" in v_data:
                        ts_val = int(obj) >> (cls.N_BITS + cls.S_BITS)
                        self.assertEqual(ts_val, v_data["expected_timestamp"])
                    if "expected_iso" in v_data:
                        self.assertEqual(obj.to_iso_string(), v_data["expected_iso"])
                    if "expected_hex" in v_data:
                        self.assertEqual(hex(obj), v_data["expected_hex"])
                    if "expected_str" in v_data:
                        self.assertEqual(str(obj), v_data["expected_str"])

    def test_precision_checks(self):
        for check in self.data["precision_checks"]:
            v_name = check["variant"]
            if v_name not in self.variant_map:
                continue
            cls = self.variant_map[v_name]
            with self.subTest(variant=v_name, input=check["input_iso"]):
                obj = cls.from_iso_string(check["input_iso"])
                self.assertEqual(obj.to_iso_string(), check["expected_iso"])

    def test_error_cases(self):
        for case in self.data["error_cases"]:
            v_name = case.get("variant", "chrono64ms")
            if v_name not in self.variant_map:
                continue
            cls = self.variant_map[v_name]
            input_val = case["input"]
            expected_err = case["expected_error"]
            with self.subTest(name=case["name"]):
                with self.assertRaisesRegex(ChronoError, re.escape(expected_err)):
                    cls.from_iso_string(input_val)

    def test_bit_splits(self):
        if "bit_splits" not in self.data:
            return
        for case in self.data["bit_splits"]:
            v_name = case["variant"]
            if v_name not in self.variant_map:
                continue
            cls = self.variant_map[v_name]

            with self.subTest(desc=case.get("description", v_name)):
                iso = case.get("iso", "2023-01-01T00:00:00Z")
                iso_clean = iso.replace("Z", "+00:00")
                dt = datetime.fromisoformat(iso_clean)
                node_id = case.get("node_id", 0)
                seq = case.get("seq", 0)

                # Use from_parts with salt=0 for raw bit check
                obj = cls.from_parts(dt, node_id, seq, p_idx=0, salt=0)
                self.assertEqual(hex(obj), case["expected_hex"])

                val = int(obj)
                if "expected_timestamp_bits" in case:
                    ts_val = val >> (cls.N_BITS + cls.S_BITS)
                    ts_bin = bin(ts_val)[2:].zfill(cls.T_BITS)
                    self.assertEqual(ts_bin, case["expected_timestamp_bits"])

                if "expected_node_bits" in case:
                    # Note: this check only works if we know the multiplier results
                    # The JSON expected_node_bits already accounts for mixing in my generation.
                    n_mix = (val >> cls.S_BITS) & (
                        (1 << cls.N_BITS) - 1 if cls.N_BITS > 0 else 0
                    )
                    n_bin = bin(n_mix)[2:].zfill(cls.N_BITS)
                    self.assertEqual(n_bin, case["expected_node_bits"])

                if "expected_seq_bits" in case:
                    s_mix = val & ((1 << cls.S_BITS) - 1 if cls.S_BITS > 0 else 0)
                    s_bin = bin(s_mix)[2:].zfill(cls.S_BITS)
                    self.assertEqual(s_bin, case["expected_seq_bits"])

    def test_variant_specs(self):
        if "variant_specs" not in self.data:
            return
        for spec in self.data["variant_specs"]:
            v_name = spec["variant"]
            if v_name not in self.variant_map:
                continue
            cls = self.variant_map[v_name]

            with self.subTest(variant=v_name):
                self.assertEqual(
                    cls.T_BITS, spec["t_bits"], f"{v_name} T_BITS mismatch"
                )
                self.assertEqual(
                    cls.N_BITS, spec["n_bits"], f"{v_name} N_BITS mismatch"
                )
                self.assertEqual(
                    cls.S_BITS, spec["s_bits"], f"{v_name} S_BITS mismatch"
                )


if __name__ == "__main__":
    unittest.main()
