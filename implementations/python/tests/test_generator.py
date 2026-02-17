import unittest
from datetime import datetime, timezone
from chrono_id import Chrono64s, Generator, Persona


class TestGenerator(unittest.TestCase):
    def test_basic_generation(self):
        gen = Generator(Chrono64s)
        id1 = gen.generate()
        id2 = gen.generate()
        self.assertIsInstance(id1, Chrono64s)
        self.assertNotEqual(id1, id2)
        self.assertEqual(gen.sequence, 1)

    def test_time_reset(self):
        gen = Generator(Chrono64s)
        t1 = datetime(2023, 1, 1, 12, 0, 0, tzinfo=timezone.utc)
        t2 = datetime(2023, 1, 1, 12, 0, 1, tzinfo=timezone.utc)

        id1 = gen.generate(dt=t1)
        self.assertEqual(gen.sequence, 0)

        id2 = gen.generate(dt=t1)
        self.assertEqual(gen.sequence, 1)

        id3 = gen.generate(dt=t2)
        self.assertEqual(gen.sequence, 0)
        self.assertGreater(id3, id2)

    def test_persona_rotation(self):
        # Force rotation by mocking last_rotate
        gen = Generator(Chrono64s)
        initial_node_salt = gen.persona.node_salt

        # Set last_rotate to 61 seconds ago
        gen.persona.last_rotate = datetime.now(timezone.utc).timestamp() - 61

        gen.generate()
        self.assertNotEqual(gen.persona.node_salt, initial_node_salt)

    def test_rollback_resilience(self):
        # Verify that clock rollback triggers rotation and maintains uniqueness
        gen = Generator(Chrono64s)
        dt1 = datetime(2023, 1, 1, 12, 0, 0, tzinfo=timezone.utc)
        dt2 = datetime(2023, 1, 1, 11, 59, 59, tzinfo=timezone.utc)  # 1s rollback

        id1 = gen.generate(dt=dt1)
        p1_node = gen.persona.node_id

        id2 = gen.generate(dt=dt2)
        self.assertNotEqual(id1, id2)
        self.assertNotEqual(gen.persona.node_id, p1_node)


if __name__ == "__main__":
    unittest.main()
