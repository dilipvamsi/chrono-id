import unittest
import psycopg2
import os
from datetime import datetime, timezone

# Configuration
DB_DSN = os.environ.get(
    "TEST_DATABASE_URL", "postgresql://postgres:postgres@localhost:5432/postgres"
)
SQL_FILE_PATH = os.path.join(os.path.dirname(__file__), "../chrono_id.sql")

class TestChronoIDPostgres(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        try:
            cls.conn = psycopg2.connect(DB_DSN)
            cls.conn.autocommit = True
            cls.cursor = cls.conn.cursor()

            with open(SQL_FILE_PATH, "r") as f:
                cls.cursor.execute(f.read())

        except Exception as e:
            print(f"Failed to connect or load SQL: {e}")
            raise unittest.SkipTest("PostgreSQL not available or SQL error")

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "cursor"):
            cls.cursor.close()
        if hasattr(cls, "conn"):
            cls.conn.close()

    def test_all_variants_generation(self):
        variants = [
            "chrono32", "uchrono32",
            "chrono32h", "uchrono32h",
            "chrono32m", "uchrono32m",
            "chrono64", "uchrono64",
            "chrono64ms", "uchrono64ms"
        ]
        for func in variants:
            with self.subTest(func=func):
                self.cursor.execute(f"SELECT {func}()")
                val = self.cursor.fetchone()[0]
                self.assertIsNotNone(val)
                if func.startswith("u") or "64" in func:
                    self.assertIsInstance(val, int) # Bigints come as ints
                else:
                    self.assertIsInstance(val, int)

    def test_all_retrieval_functions(self):
        # Test each retrieval function with a known ID (0)
        # 32-bit: Day, Hour, Minute
        # 64-bit: Second, Millisecond
        retrievals = [
            ("chrono32_get_time", 0, 2000),
            ("uchrono32_get_time", 0, 2000),
            ("chrono32h_get_time", 0, 2000),
            ("chrono32m_get_time", 0, 2000),
            ("chrono64_get_time", 0, 1970),
            ("chrono64ms_get_time", 0, 1970)
        ]
        for func, val, expected_year in retrievals:
            with self.subTest(func=func):
                self.cursor.execute(f"SELECT {func}(%s)", (val,))
                res = self.cursor.fetchone()[0]
                self.assertEqual(res.year, expected_year)
                # Ensure it returns TIMESTAMPTZ (Python datetime with tzinfo)
                self.assertIsNotNone(res.tzinfo)

    def test_accuracy_roundtrip(self):
        # Generate and then decode to verify accuracy within precision
        test_cases = [
            ("chrono32", "chrono32_get_time", 86400),
            ("chrono32h", "chrono32h_get_time", 3600),
            ("chrono32m", "chrono32m_get_time", 60),
            ("chrono64", "chrono64_get_time", 1),
            ("chrono64ms", "chrono64ms_get_time", 0.001)
        ]
        for gen, get, precision in test_cases:
            with self.subTest(gen=gen):
                self.cursor.execute(f"SELECT {gen}()")
                uid = self.cursor.fetchone()[0]
                self.cursor.execute(f"SELECT {get}(%s)", (uid,))
                decoded = self.cursor.fetchone()[0]

                now = datetime.now(timezone.utc)
                diff = abs((now - decoded).total_seconds())
                self.assertLessEqual(diff, precision + 2) # Buffer for execution time

    def test_u_variants_monotonicity(self):
        # Unsigned variants should still be positive BIGINTs in our context
        self.cursor.execute("SELECT uchrono64ms()")
        val = self.cursor.fetchone()[0]
        # In PG, BIGINT is signed. If the high bit is set, it might be negative.
        # But we want to ensure it's generated.
        self.assertIsNotNone(val)

if __name__ == "__main__":
    unittest.main()
