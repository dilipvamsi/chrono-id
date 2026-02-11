import json
import unittest
import os
import re
from chrono_id import (
    UChrono32, Chrono32, UChrono32h, Chrono32h, UChrono32m, Chrono32m,
    UChrono32w, Chrono32w,
    UChrono64, Chrono64, UChrono64ms, Chrono64ms, UChrono64us, Chrono64us,
    ChronoError
)

class TestCrossPlatformJSON(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        path = os.path.join(os.path.dirname(__file__), "../../../tests/cross_platform_tests.json")
        with open(path, "r") as f:
            cls.data = json.load(f)
        
        cls.variant_map = {
            "chrono32": Chrono32, "uchrono32": UChrono32,
            "chrono32h": Chrono32h, "uchrono32h": UChrono32h,
            "chrono32m": Chrono32m, "uchrono32m": UChrono32m,
            "chrono32w": Chrono32w, "uchrono32w": UChrono32w,
            "chrono64": Chrono64, "uchrono64": UChrono64,
            "chrono64ms": Chrono64ms, "uchrono64ms": UChrono64ms,
            "chrono64us": Chrono64us, "uchrono64us": UChrono64us
        }

    def test_valid_cases(self):
        for case in self.data["valid_cases"]:
            iso = case["iso"]
            for v_data in case["variants"]:
                v_name = v_data["name"]
                cls = self.variant_map[v_name]
                with self.subTest(iso=iso, variant=v_name):
                    obj = cls.from_iso_string(iso, random_val=0)
                    if "expected_timestamp" in v_data:
                        self.assertEqual(obj.get_timestamp(), v_data["expected_timestamp"])
                    if "expected_iso" in v_data:
                        self.assertEqual(obj.to_iso_string(), v_data["expected_iso"])

    def test_error_cases(self):
        for case in self.data["error_cases"]:
            v_name = case.get("variant", "chrono64ms")
            cls = self.variant_map[v_name]
            input_val = case["input"]
            expected_err = case["expected_error"]
            with self.subTest(name=case["name"]):
                # Escape the expected error string to avoid regex meta-character issues
                with self.assertRaisesRegex(ChronoError, re.escape(expected_err)):
                    cls.from_iso_string(input_val)

if __name__ == "__main__":
    unittest.main()
