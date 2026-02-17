import unittest
from datetime import datetime, timezone
from chrono_id import (
    UChrono32d,
    Chrono32d,
    UChrono32h,
    Chrono32h,
    UChrono32m,
    Chrono32m,
    UChrono32w,
    Chrono32w,
    Chrono32y,
    UChrono32y,
    UChrono64s,
    Chrono64s,
    UChrono64ms,
    Chrono64ms,
    UChrono64us,
    Chrono64us,
    ChronoBase,
    EPOCH_2020,
    ChronoError,
)


# Helper to get UTC datetime (Post-2020)
def utc_dt(*args):
    return datetime(*args, tzinfo=timezone.utc)


class TestChronoID(unittest.TestCase):
    def test_chrono_base_instantiation(self):
        # Test default instantiation (current time)
        obj = Chrono32d()
        self.assertIsInstance(obj, Chrono32d)
        self.assertIsInstance(obj, int)

        # Test instantiation from value
        obj2 = Chrono32d(12345)
        self.assertEqual(obj2, 12345)

    def test_from_time_and_get_time(self):
        # Use UTC post-2020
        dt = utc_dt(2023, 1, 1, 12, 30, 45)

        variants = [
            (UChrono32d, 86400),  # Day precision
            (Chrono32d, 86400),
            (UChrono32h, 3600),  # Hour precision
            (Chrono32h, 3600),
            (UChrono32m, 60),  # Minute precision
            (Chrono32m, 60),
            (UChrono64s, 1),  # Second precision
            (Chrono64s, 1),
            (UChrono64ms, 0.001),
            (Chrono64ms, 0.001),
            (UChrono64us, 0.000001),  # Microsecond precision
            (Chrono64us, 0.000001),
            (UChrono32w, 604800),  # Week precision
            (Chrono32w, 604800),
            (Chrono32y, 31536000),  # Year precision
        ]

        for cls, precision in variants:
            with self.subTest(cls=cls.__name__):
                obj = cls.from_parts(dt, node_id=0, seq=0, p_idx=0, salt=0)
                decoded_dt = obj.get_time()

                # Check if decoded time is within precision limits
                diff = abs((decoded_dt - dt).total_seconds())
                self.assertLessEqual(
                    diff, precision, f"{cls.__name__} failed precision check"
                )

    def test_time_flooring(self):
        dt = utc_dt(2023, 1, 1, 12, 30, 45)

        # Minute precision (should be 12:30:00 UTC)
        obj_m = Chrono32m.from_parts(dt, node_id=0, seq=0, p_idx=0, salt=0)
        decoded = obj_m.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 12, 30, 0))

        # Hour precision (should be 12:00:00 UTC)
        obj_h = Chrono32h.from_parts(dt, node_id=0, seq=0, p_idx=0, salt=0)
        decoded = obj_h.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 12, 0, 0))

        # Day precision (should be 00:00:00 UTC)
        obj_d = Chrono32d.from_parts(
            dt, node_id=0, seq=0, persona=None, p_idx=0, salt=0
        )
        decoded = obj_d.get_time()
        self.assertEqual(decoded, utc_dt(2023, 1, 1, 0, 0, 0))

    def test_epoch_boundaries(self):
        # Epoch: Jan 1, 2020 UTC
        epoch = utc_dt(2020, 1, 1, 0, 0, 0)
        obj = Chrono32d.from_parts(
            epoch, node_id=0, seq=0, persona=None, p_idx=0, salt=0
        )
        # T should be 0, so shifted value depends on N and S bits
        self.assertEqual(obj >> (Chrono32d.N_BITS + Chrono32d.S_BITS), 0)

    def test_sorting(self):
        t1 = utc_dt(2023, 1, 1, 10, 0, 0)
        t2 = utc_dt(2023, 1, 1, 11, 0, 0)

        id1 = Chrono64s.generate(dt=t1)
        id2 = Chrono64s.generate(dt=t2)

        self.assertLess(id1, id2)

    def test_auto_random(self):
        dt = datetime.now(timezone.utc)
        obj1 = Chrono64ms.generate(dt=dt)
        obj2 = Chrono64ms.generate(dt=dt)
        # With high probability they are different
        self.assertNotEqual(obj1, obj2)

    def test_precision_mismatch(self):
        with self.assertRaises(ChronoError):
            Chrono64s.from_iso_string("2019-12-31T23:59:59Z")

    def test_string_representation(self):
        # 64-bit variant
        id64 = Chrono64s(0x1234567890ABCDEF)
        fmt64 = str(id64)
        self.assertEqual(fmt64, "1234-5678-90AB-CDEF")
        self.assertEqual(repr(id64), "Chrono64s(1234-5678-90AB-CDEF)")

        # Round-trip parsing
        id64_parsed = Chrono64s("1234-5678-90AB-CDEF")
        self.assertEqual(id64, id64_parsed)

        # 32-bit variant
        id32 = Chrono32y(0x12345678)
        fmt32 = str(id32)
        self.assertEqual(fmt32, "1234-5678")

        # Round-trip 32-bit
        id32_parsed = Chrono32y("1234-5678")
        self.assertEqual(id32, id32_parsed)

        # Handling lowercase and mixed spaces
        id64_mixed = Chrono64s("1234 5678 90ab cdef")
        self.assertEqual(id64, id64_mixed)

    def test_from_formatted(self):
        # 64-bit explicit
        id64 = Chrono64s.from_formatted("1234-5678-90AB-CDEF")
        self.assertEqual(int(id64), 0x1234567890ABCDEF)

        # 32-bit explicit
        id32 = Chrono32y.from_formatted("1234-5678")
        self.assertEqual(int(id32), 0x12345678)

        # Error case
        with self.assertRaises(ChronoError):
            Chrono32y.from_formatted("Invalid-Hex")

    def test_from_iso_string(self):
        with self.subTest(case="ms-precision"):
            obj1 = Chrono64ms.from_iso_string("2023-05-20T10:30:00.123Z")
            self.assertEqual(obj1.to_iso_string(), "2023-05-20T10:30:00.123Z")

        with self.subTest(case="null-iso"):
            with self.assertRaisesRegex(ChronoError, "Input string is null"):
                Chrono64ms.from_iso_string(None)

        with self.subTest(case="invalid-format"):
            with self.assertRaisesRegex(ChronoError, "Invalid ISO 8601 format"):
                Chrono64ms.from_iso_string("not-a-date")

        with self.subTest(case="underflow"):
            with self.assertRaisesRegex(ChronoError, "Date is before Epoch"):
                Chrono64s.from_iso_string("2010-01-01T00:00:00Z")

    def test_from_time_null(self):
        with self.assertRaisesRegex(ChronoError, "Input date is null"):
            Chrono32d.from_parts(None, 0, 0, persona=None, p_idx=0, salt=0)


if __name__ == "__main__":
    unittest.main()
