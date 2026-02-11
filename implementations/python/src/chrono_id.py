from datetime import datetime, timezone
import secrets

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
            return cls.from_time(datetime.now(timezone.utc))
        return super().__new__(cls, value)

    def get_time(self) -> datetime:
        """Extracts the timestamp embedded in the ID."""
        unix_ts = self._extract_unix_ts()
        return datetime.fromtimestamp(unix_ts, tz=timezone.utc)

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

# ==========================================
# 32-BIT FAMILY (Epoch: Jan 1, 2000)
# ==========================================

class UChrono32(ChronoBase):
    """Unsigned 32-bit Day ID. [Day: 18b][Rand: 14b]. Expires: 2717."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 86400  # Day
    TIME_MASK = 0x3FFFF
    SHIFT = 14
    RANDOM_BITS = 14

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class Chrono32(ChronoBase):
    """Signed 32-bit Day ID. [0][Day: 18b][Rand: 13b]. Expires: 2717."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 86400  # Day
    TIME_MASK = 0x3FFFF
    SHIFT = 13
    RANDOM_BITS = 13

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class UChrono32h(ChronoBase):
    """Unsigned 32-bit Hour ID. [Hour: 21b][Rand: 11b]. Expires: 2239."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 3600  # Hour
    TIME_MASK = 0x1FFFFF
    SHIFT = 11
    RANDOM_BITS = 11

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class Chrono32h(ChronoBase):
    """Signed 32-bit Hour ID. [0][Hour: 21b][Rand: 10b]. Expires: 2239."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 3600  # Hour
    TIME_MASK = 0x1FFFFF
    SHIFT = 10
    RANDOM_BITS = 10

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class UChrono32m(ChronoBase):
    """Unsigned 32-bit Minute ID. [Min: 27b][Rand: 5b]. Expires: 2255."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 60  # Minute
    TIME_MASK = 0x7FFFFFF
    SHIFT = 5
    RANDOM_BITS = 5

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class Chrono32m(ChronoBase):
    """Signed 32-bit Minute ID. [0][Min: 27b][Rand: 4b]. Expires: 2255."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 60  # Minute
    TIME_MASK = 0x7FFFFFF
    SHIFT = 4
    RANDOM_BITS = 4

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

# ==========================================
# 64-BIT FAMILY (Epoch: Jan 1, 1970)
# ==========================================

class UChrono64(ChronoBase):
    """Unsigned 64-bit Second ID. [Sec: 36b][Rand: 28b]. Expires: 4147."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 1  # Second
    TIME_MASK = 0xFFFFFFFFF
    SHIFT = 28
    RANDOM_BITS = 28

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return float(self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR))

class Chrono64(ChronoBase):
    """Signed 64-bit Second ID. [0][Sec: 36b][Rand: 27b]. Expires: 4147."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 1  # Second
    TIME_MASK = 0xFFFFFFFFF
    SHIFT = 27
    RANDOM_BITS = 27

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return float(self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR))

class UChrono64ms(ChronoBase):
    """Unsigned 64-bit Millisecond ID. [MS: 44b][Rand: 20b]. Expires: 2527."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.001  # Millisecond
    TIME_MASK = 0xFFFFFFFFFFF
    SHIFT = 20
    RANDOM_BITS = 20

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class Chrono64ms(ChronoBase):
    """Signed 64-bit Millisecond ID. [0][MS: 44b][Rand: 19b]. Expires: 2527."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.001  # Millisecond
    TIME_MASK = 0xFFFFFFFFFFF
    SHIFT = 19
    RANDOM_BITS = 19

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class UChrono64us(ChronoBase):
    """Unsigned 64-bit Microsecond ID. [us: 54b][Rand: 10b]. Expires: 2540."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.000001  # Microsecond
    TIME_MASK = 0x3FFFFFFFFFFFFF
    SHIFT = 10
    RANDOM_BITS = 10

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

class Chrono64us(ChronoBase):
    """Signed 64-bit Microsecond ID. [0][us: 54b][Rand: 9b]. Expires: 2540."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.000001  # Microsecond
    TIME_MASK = 0x3FFFFFFFFFFFFF
    SHIFT = 9
    RANDOM_BITS = 9

    @classmethod
    def _pack_time(cls, ts, rand):
        units = int((ts - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)
        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR)

# ==========================================
# DEMONSTRATION
# ==========================================
if __name__ == "__main__":  # pragma: no cover
    header = f"{'Function':<12} | {'Value':<20} | {'Decoded Time':<20} | {'Expires'}"
    print(header)
    print("-" * len(header))

    examples = [
        (UChrono32m, "Year 2255"), (Chrono32m, "Year 2255"),
        (UChrono32h, "Year 2239"), (Chrono32h, "Year 2239"),
        (UChrono32,  "Year 2717"), (Chrono32,  "Year 2717"),
        (UChrono64,  "Year 4147"), (Chrono64,  "Year 4147"),
        (UChrono64ms,"Year 2527"), (Chrono64ms,"Year 2527"),
        (UChrono64us,"Year 2540"), (Chrono64us,"Year 2540")
    ]

    for cls, expiry in examples:
        obj = cls()
        print(f"{cls.__name__:<12} | {obj:<20} | {obj.get_time().strftime('%Y-%m-%d %H:%M')} | {expiry}")
