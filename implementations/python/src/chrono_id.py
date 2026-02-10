import time
import secrets
from datetime import datetime

# --- Constants ---
EPOCH_2000 = 946684800  # Jan 1, 2000 (Unix Timestamp)

class ChronoBase(int):
    """
    Base class for Chrono IDs. Inherits from int.
    Provides methods to encode/decode time from the integer value.
    """
    RANDOM_BITS = 0
    TIME_BITS = 0

    def __new__(cls, value=None):
        """If value is None, generate a new ID based on current time."""
        if value is None:
            return cls.from_time(datetime.now())
        return super().__new__(cls, value)

    def get_time(self) -> datetime:
        """Extracts the timestamp embedded in the ID."""
        unix_ts = self._extract_unix_ts()
        return datetime.fromtimestamp(unix_ts)

    @classmethod
    def from_time(cls, dt: datetime, random_val: int = None):
        """Generates an ID for a specific point in time."""
        timestamp = dt.timestamp()
        if random_val is None:
            random_val = secrets.randbits(cls.RANDOM_BITS)

        # Mask random value to ensure it doesn't overflow into time bits
        random_val &= (1 << cls.RANDOM_BITS) - 1

        packed_value = cls._pack_time(timestamp, random_val)
        return super().__new__(cls, packed_value)

# ==========================================
# 32-BIT FAMILY (Epoch: Jan 1, 2000)
# ==========================================

class UChrono32(ChronoBase):
    """Unsigned 32-bit Day ID. [Day: 18b][Rand: 14b]. Expires: 2717."""
    RANDOM_BITS = 14
    @staticmethod
    def _pack_time(ts, rand):
        days = int((ts - EPOCH_2000) // 86400)
        return ((days & 0x3FFFF) << 14) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 14) * 86400)

class Chrono32(ChronoBase):
    """Signed 32-bit Day ID. [0][Day: 18b][Rand: 13b]. Expires: 2717."""
    RANDOM_BITS = 13
    @staticmethod
    def _pack_time(ts, rand):
        days = int((ts - EPOCH_2000) // 86400)
        return ((days & 0x3FFFF) << 13) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 13) * 86400)

class UChrono32h(ChronoBase):
    """Unsigned 32-bit Hour ID. [Hour: 21b][Rand: 11b]. Expires: 2239."""
    RANDOM_BITS = 11
    @staticmethod
    def _pack_time(ts, rand):
        hours = int((ts - EPOCH_2000) // 3600)
        return ((hours & 0x1FFFFF) << 11) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 11) * 3600)

class Chrono32h(ChronoBase):
    """Signed 32-bit Hour ID. [0][Hour: 21b][Rand: 10b]. Expires: 2239."""
    RANDOM_BITS = 10
    @staticmethod
    def _pack_time(ts, rand):
        hours = int((ts - EPOCH_2000) // 3600)
        return ((hours & 0x1FFFFF) << 10) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 10) * 3600)

class UChrono32m(ChronoBase):
    """Unsigned 32-bit Minute ID. [Min: 27b][Rand: 5b]. Expires: 2255."""
    RANDOM_BITS = 5
    @staticmethod
    def _pack_time(ts, rand):
        minutes = int((ts - EPOCH_2000) // 60)
        return ((minutes & 0x7FFFFFF) << 5) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 5) * 60)

class Chrono32m(ChronoBase):
    """Signed 32-bit Minute ID. [0][Min: 27b][Rand: 4b]. Expires: 2255."""
    RANDOM_BITS = 4
    @staticmethod
    def _pack_time(ts, rand):
        minutes = int((ts - EPOCH_2000) // 60)
        return ((minutes & 0x7FFFFFF) << 4) | rand
    def _extract_unix_ts(self):
        return EPOCH_2000 + ((self >> 4) * 60)

# ==========================================
# 64-BIT FAMILY (Epoch: Jan 1, 1970)
# ==========================================

class UChrono64(ChronoBase):
    """Unsigned 64-bit Second ID. [Sec: 36b][Rand: 28b]. Expires: 4147."""
    RANDOM_BITS = 28
    @staticmethod
    def _pack_time(ts, rand):
        secs = int(ts)
        return ((secs & 0xFFFFFFFFF) << 28) | rand
    def _extract_unix_ts(self):
        return float(self >> 28)

class Chrono64(ChronoBase):
    """Signed 64-bit Second ID. [0][Sec: 36b][Rand: 27b]. Expires: 4147."""
    RANDOM_BITS = 27
    @staticmethod
    def _pack_time(ts, rand):
        secs = int(ts)
        return ((secs & 0xFFFFFFFFF) << 27) | rand
    def _extract_unix_ts(self):
        return float(self >> 27)

class UChrono64ms(ChronoBase):
    """Unsigned 64-bit Millisecond ID. [MS: 44b][Rand: 20b]. Expires: 2527."""
    RANDOM_BITS = 20
    @staticmethod
    def _pack_time(ts, rand):
        ms = int(ts * 1000)
        return ((ms & 0xFFFFFFFFFFF) << 20) | rand
    def _extract_unix_ts(self):
        return (self >> 20) / 1000.0

class Chrono64ms(ChronoBase):
    """Signed 64-bit Millisecond ID. [0][MS: 44b][Rand: 19b]. Expires: 2527."""
    RANDOM_BITS = 19
    @staticmethod
    def _pack_time(ts, rand):
        ms = int(ts * 1000)
        return ((ms & 0xFFFFFFFFFFF) << 19) | rand
    def _extract_unix_ts(self):
        return (self >> 19) / 1000.0

# ==========================================
# DEMONSTRATION
# ==========================================
if __name__ == "__main__":
    header = f"{'Function':<12} | {'Value':<20} | {'Decoded Time':<20} | {'Expires'}"
    print(header)
    print("-" * len(header))

    examples = [
        (UChrono32m, "Year 2255"), (Chrono32m, "Year 2255"),
        (UChrono32h, "Year 2239"), (Chrono32h, "Year 2239"),
        (UChrono32,  "Year 2717"), (Chrono32,  "Year 2717"),
        (UChrono64,  "Year 4147"), (Chrono64,  "Year 4147"),
        (UChrono64ms,"Year 2527"), (Chrono64ms,"Year 2527")
    ]

    for cls, expiry in examples:
        obj = cls()
        print(f"{cls.__name__:<12} | {obj:<20} | {obj.get_time().strftime('%Y-%m-%d %H:%M')} | {expiry}")
