import unittest
import sqlite3
import os
from datetime import datetime, timezone

# Configuration
EXT_PATH = os.path.join(os.path.dirname(__file__), "../build/chrono_id")
# Note: For loading extensions in Python, the path often needs to be absolute
ABS_EXT_PATH = os.path.abspath(EXT_PATH)

class TestChronoIDSQLite(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        try:
            cls.conn = sqlite3.connect(":memory:")
            cls.conn.enable_load_extension(True)
            cls.conn.load_extension(ABS_EXT_PATH)
            cls.cursor = cls.conn.cursor()
        except Exception as e:
            print(f"Failed to load SQLite extension: {e}")
            raise unittest.SkipTest(f"SQLite extension not loadable: {e}")

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "conn"):
            cls.conn.close()

    def test_generic_functions(self):
        # chrono_new(type)
        variants = ["32", "u32", "32h", "32m", "32w", "64", "64ms", "64us"]
        for v in variants:
            with self.subTest(variant=v):
                self.cursor.execute("SELECT chrono_new(?)", (v,))
                val = self.cursor.fetchone()[0]
                self.assertIsNotNone(val)
                self.assertIsInstance(val, int)

    def test_individual_wrappers(self):
        # Individual functions like chrono32() (Postgres naming)
        wrappers = ["chrono32", "uchrono32", "chrono32h", "chrono32m", "chrono32w", "uchrono32w", "chrono64", "chrono64ms", "chrono64us"]
        for f in wrappers:
            with self.subTest(func=f):
                self.cursor.execute(f"SELECT {f}()")
                val = self.cursor.fetchone()[0]
                self.assertIsNotNone(val)
                self.assertIsInstance(val, int)

    def test_iso_roundtrip(self):
        # chrono_to_iso and chrono_from_iso (Generic)
        iso_in = "2023-05-20T10:30:00.123456Z"
        variant = "64us"

        self.cursor.execute("SELECT chrono_from_iso(?, ?)", (iso_in, variant))
        val = self.cursor.fetchone()[0]
        self.assertIsNotNone(val)

        self.cursor.execute("SELECT chrono_to_iso(?, ?)", (val, variant))
        iso_out = self.cursor.fetchone()[0]
        self.assertEqual(iso_in, iso_out)

    def test_32bit_limits(self):
        # chrono32() should return a signed 32-bit integer
        self.cursor.execute("SELECT chrono32()")
        val = self.cursor.fetchone()[0]
        self.assertLess(val, 2147483647)
        self.assertGreater(val, -2147483648)

    def test_individual_getters(self):
        # chrono64ms_get_time(id)
        self.cursor.execute("SELECT chrono64ms()")
        val = self.cursor.fetchone()[0]

        self.cursor.execute("SELECT chrono64ms_get_time(?)", (val,))
        iso = self.cursor.fetchone()[0]
        self.assertIsInstance(iso, str)
        self.assertTrue(iso.endswith("Z"))

    def test_error_handling(self):
        # Invalid ISO string should raise an OperationalError
        with self.assertRaises(sqlite3.OperationalError) as cm:
            self.cursor.execute("SELECT chrono_from_iso('invalid-date', '64ms')")
        self.assertIn("Invalid ISO 8601 format", str(cm.exception))

        with self.assertRaises(sqlite3.OperationalError) as cm:
             self.cursor.execute("SELECT chrono64ms_from_iso('invalid-date')")
        self.assertIn("Invalid ISO 8601 format", str(cm.exception))

        # Test NULL handling
        with self.assertRaises(sqlite3.OperationalError) as cm:
             self.cursor.execute("SELECT chrono_from_iso(NULL, '64ms')")
        self.assertIn("Input string is null", str(cm.exception))

        # Test Underflow (Pre-2000 for 32-bit)
        with self.assertRaises(sqlite3.OperationalError) as cm:
             self.cursor.execute("SELECT chrono_from_iso('1999-12-31T23:59:59Z', '32')")
        self.assertIn("Timestamp underflow", str(cm.exception))
        self.assertIn("2000-01-01", str(cm.exception))

        with self.assertRaises(sqlite3.OperationalError) as cm:
             self.cursor.execute("SELECT chrono_from_iso('1960-01-01T00:00:00Z', '64')")
        self.assertIn("Pre-1970 not supported", str(cm.exception))

if __name__ == "__main__":
    unittest.main()
