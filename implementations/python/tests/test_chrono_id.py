import unittest
from datetime import datetime, timezone
import time
from chrono_id import (
    UChrono32, Chrono32, UChrono32h, Chrono32h, UChrono32m, Chrono32m,
    UChrono32w, Chrono32w,
    UChrono64, Chrono64, UChrono64ms, Chrono64ms, UChrono64us, Chrono64us,
    ChronoBase, EPOCH_2000, ChronoError
)

# Helper to get UTC datetime
def utc_dt(*args):
    return datetime(*args, tzinfo=timezone.utc)

class TestChronoID(unittest.TestCase):
    def test_chrono_base_instantiation(self):
        # Test default instantiation (current time)
        obj = Chrono32()
        self.assertIsInstance(obj, Chrono32)
        self.assertIsInstance(obj, int)
        
        # Test instantiation from value
        obj2 = Chrono32(12345)
        self.assertEqual(obj2, 12345)
        
        # Test ChronoBase directly
        base = ChronoBase(100)
        self.assertEqual(base, 100)

    def test_from_time_and_get_time(self):
        # Use UTC to avoid local timezone offsets during precision checks
        dt = utc_dt(2023, 1, 1, 12, 30, 45)
        
        variants = [
            (UChrono32, 86400),    # Day precision
            (Chrono32, 86400),
            (UChrono32h, 3600),    # Hour precision
            (Chrono32h, 3600),
            (UChrono32m, 60),      # Minute precision
            (Chrono32m, 60),
            (UChrono64, 1),        # Second precision
            (Chrono64, 1),
            (UChrono64ms, 0.001),
            (Chrono64ms, 0.001),
            (UChrono64us, 0.000001), # Microsecond precision
            (Chrono64us, 0.000001),
            (UChrono32w, 604800),    # Week precision
            (Chrono32w, 604800),
        ]
        
        for cls, precision in variants:
            with self.subTest(cls=cls.__name__):
                obj = cls.from_time(dt, random_val=0)
                decoded_dt = obj.get_time()
                
                # Check if decoded time is within precision limits
                diff = abs((decoded_dt - dt).total_seconds())
                self.assertLessEqual(diff, precision, f"{cls.__name__} failed precision check")

    def test_time_flooring(self):
        # Specific check for flooring in UTC
        dt = utc_dt(2023, 1, 1, 12, 30, 45)
        
        # Minute precision (should be 12:30:00 UTC)
        obj_m = Chrono32m.from_time(dt, random_val=0)
        decoded = obj_m.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 12, 30, 0))
        
        # Hour precision (should be 12:00:00 UTC)
        obj_h = Chrono32h.from_time(dt, random_val=0)
        decoded = obj_h.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 12, 0, 0))
        
        # Day precision (should be 00:00:00 UTC)
        obj_d = Chrono32.from_time(dt, random_val=0)
        decoded = obj_d.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 0, 0, 0))

    def test_epoch_boundaries(self):
        # 32-bit Epoch: Jan 1, 2000 UTC
        epoch_32 = utc_dt(2000, 1, 1, 0, 0, 0)
        obj_32 = Chrono32.from_time(epoch_32, random_val=0)
        self.assertEqual(obj_32 >> 13, 0)

        # 64-bit Epoch: Jan 1, 1970 UTC
        epoch_64 = utc_dt(1970, 1, 1, 0, 0, 0)
        obj_64 = Chrono64.from_time(epoch_64, random_val=0)
        self.assertEqual(obj_64 >> 27, 0)

    def test_expiry_boundary(self):
        # Max days allowed (18 bits)
        max_days = 0x3FFFF
        max_ts = EPOCH_2000 + (max_days * 86400)
        max_dt = datetime.fromtimestamp(max_ts, tz=timezone.utc)
        
        obj = Chrono32.from_time(max_dt, random_val=0)
        self.assertEqual(obj >> 13, max_days)

    def test_sorting(self):
        # IDs generated at different times should be sortable
        t1 = datetime(2023, 1, 1, 10, 0, 0)
        t2 = datetime(2023, 1, 1, 11, 0, 0)

        id1 = Chrono64.from_time(t1)
        id2 = Chrono64.from_time(t2)

        self.assertLess(id1, id2)

        # Different entropy but same time should still be close
        id2_alt = Chrono64.from_time(t1)
        self.assertNotEqual(id1, id2_alt)
        # Time bits should be identical
        self.assertEqual(id1 >> 27, id2_alt >> 27)

    def test_random_masking(self):
        dt = datetime.now()
        # Test with random value larger than allowed bits
        # Chrono32 has 13 random bits (8191 max)
        obj = Chrono32.from_time(dt, random_val=0xFFFFFFFF)
        self.assertEqual(obj & 0x1FFF, 0x1FFF)

        # UChrono32 has 14 random bits (16383 max)
        obj_u = UChrono32.from_time(dt, random_val=0xFFFFFFFF)
        self.assertEqual(obj_u & 0x3FFF, 0x3FFF)

    def test_auto_random(self):
        # Verify that two IDs generated at the same time have different random bits
        dt = datetime.now()
        obj1 = Chrono64ms.from_time(dt)
        obj2 = Chrono64ms.from_time(dt)
        self.assertNotEqual(obj1, obj2)

    def test_inheritance(self):
        obj = Chrono64(1000)
        self.assertEqual(obj + 500, 1500)
        self.assertTrue(obj > 500)
        self.assertEqual(str(obj), "1000")

    def test_from_iso_string(self):
        # Valid ISO strings
        with self.subTest(case="ms-precision"):
            obj1 = Chrono64ms.from_iso_string("2023-05-20T10:30:00.123Z")
            self.assertEqual(obj1.to_iso_string(), "2023-05-20T10:30:00.123Z")
        
        # Test error: Null input
        with self.subTest(case="null-iso"):
            with self.assertRaisesRegex(ChronoError, "Input string is null"):
                Chrono64ms.from_iso_string(None)
            
        # Test error: Invalid format
        with self.subTest(case="invalid-format"):
            with self.assertRaisesRegex(ChronoError, "Invalid ISO 8601 format"):
                Chrono64ms.from_iso_string("not-a-date")
            
        # Test error: Underflow Pre-1970 (64-bit)
        with self.subTest(case="underflow-64"):
            with self.assertRaisesRegex(ChronoError, "Date is before Unix Epoch"):
                Chrono64.from_iso_string("1960-01-01T00:00:00Z")
            
        # Test error: Underflow Pre-2000 (32-bit)
        with self.subTest(case="underflow-32"):
            with self.assertRaisesRegex(ChronoError, "Date is before Epoch"):
                Chrono32.from_iso_string("1999-12-31T23:59:59Z")

    def test_from_time_null(self):
        with self.assertRaisesRegex(ChronoError, "Input date is null"):
            Chrono32.from_time(None)

if __name__ == "__main__":
    unittest.main()
