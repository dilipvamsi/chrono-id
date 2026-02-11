from datetime import datetime, timezone
import secrets

# --- Constants ---
EPOCH_2000 = 946684800  # Jan 1, 2000 (Unix Timestamp)

class ChronoError(ValueError):
    """Custom error class for Chrono-ID library."""
    pass

class ChronoBase(int):
    """
    Base class for Chrono IDs. Inherits from int.
    Provides methods to encode/decode time from the integer value.
    """
    EPOCH = 0
    TIME_UNIT_DIVISOR = 1
    TIME_MASK = 0xFFFFFFFF
    SHIFT = 0
    RANDOM_BITS = 0

    def __new__(cls, value=None):
        """If value is None, generate a new ID based on current time."""
        if value is None:
            return cls.from_time(datetime.now(timezone.utc))
        return super().__new__(cls, value)

    def get_time(self) -> datetime:
        """Extracts the timestamp embedded in the ID."""
        unix_ts = self._extract_unix_ts()
        return datetime.fromtimestamp(unix_ts, tz=timezone.utc)

    def get_timestamp(self) -> int:
        """Returns the raw numeric timestamp (units since epoch)."""
        return int(self >> self.SHIFT)

    def to_iso_string(self) -> str:
        """Returns the ID as an ISO 8601 string, naturally matching C++ precision."""
        dt = self.get_time()
        base = dt.strftime("%Y-%m-%dT%H:%M:%S")
        if self.TIME_UNIT_DIVISOR == 0.001:  # Milliseconds
            ms = (dt.microsecond // 1000)
            return f"{base}.{ms:03d}Z"
        elif self.TIME_UNIT_DIVISOR == 0.000001:  # Microseconds
            return f"{base}.{dt.microsecond:06d}Z"
        return f"{base}Z"

    @classmethod
    def from_time(cls, dt: datetime, random_val: int = None):
        """Generates an ID for a specific point in time."""
        if dt is None:
            raise ChronoError("Input date is null")

        timestamp = dt.timestamp()

        # Strict Epoch Check: Reject dates before the configured epoch
        if timestamp < cls.EPOCH:
            if cls.EPOCH > 0:
                raise ChronoError("Timestamp underflow: Date is before Epoch (32-bit types require 2000-01-01 or later)")
            else:
                raise ChronoError("Timestamp underflow: Date is before Unix Epoch (1970-01-01)")

        if random_val is None:
            random_val = secrets.randbits(cls.RANDOM_BITS)

        # Mask random value to ensure it doesn't overflow into time bits
        random_val &= (1 << cls.RANDOM_BITS) - 1

        packed_value = cls._pack_time(dt, random_val)
        return super().__new__(cls, packed_value)

    @classmethod
    def from_iso_string(cls, iso: str, random_val: int = None):
        """Standardized parsing of ISO 8601 strings."""
        if iso is None:
            raise ChronoError("Input string is null")
        try:
            iso_clean = iso.replace("Z", "+00:00")
            dt = datetime.fromisoformat(iso_clean)
        except (ValueError, TypeError):
            raise ChronoError("Invalid ISO 8601 format")
        return cls.from_time(dt, random_val)

    @classmethod
    def _pack_time(cls, dt, rand):
        # Use integer-safe math where possible to avoid floating point precision loss
        if cls.TIME_UNIT_DIVISOR == 0.001:  # Millisecond variants
            units = int(dt.timestamp()) * 1000 + dt.microsecond // 1000
            units -= int(cls.EPOCH * 1000)
        elif cls.TIME_UNIT_DIVISOR == 0.000001:  # Microsecond variants
            units = int(dt.timestamp()) * 1000000 + dt.microsecond
            units -= int(cls.EPOCH * 1000000)
        else:
            # For Day/Hour/Min/Week, floating point floor is safe
            units = int((dt.timestamp() - cls.EPOCH) // cls.TIME_UNIT_DIVISOR)

        return ((units & cls.TIME_MASK) << cls.SHIFT) | rand

    def _extract_unix_ts(self):
        return float(self.EPOCH + ((self >> self.SHIFT) * self.TIME_UNIT_DIVISOR))

# ==========================================
# 32-BIT FAMILY (Epoch: Jan 1, 2000)
# ==========================================

class UChrono32(ChronoBase):
    """Unsigned 32-bit Day ID. [Day: 18b][Rand: 14b]. Expires: 2716."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 86400
    TIME_MASK = 0x3FFFF
    SHIFT = 14
    RANDOM_BITS = 14

class Chrono32(ChronoBase):
    """Signed 32-bit Day ID. [0][Day: 18b][Rand: 13b]. Expires: 2716."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 86400
    TIME_MASK = 0x3FFFF
    SHIFT = 13
    RANDOM_BITS = 13

class UChrono32h(ChronoBase):
    """Unsigned 32-bit Hour ID. [Hour: 21b][Rand: 11b]. Expires: 2239."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 3600
    TIME_MASK = 0x1FFFFF
    SHIFT = 11
    RANDOM_BITS = 11

class Chrono32h(ChronoBase):
    """Signed 32-bit Hour ID. [0][Hour: 21b][Rand: 10b]. Expires: 2239."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 3600
    TIME_MASK = 0x1FFFFF
    SHIFT = 10
    RANDOM_BITS = 10

class UChrono32m(ChronoBase):
    """Unsigned 32-bit Minute ID. [Min: 27b][Rand: 5b]. Expires: 2255."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 60
    TIME_MASK = 0x7FFFFFF
    SHIFT = 5
    RANDOM_BITS = 5

class Chrono32m(ChronoBase):
    """Signed 32-bit Minute ID. [0][Min: 27b][Rand: 4b]. Expires: 2255."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 60
    TIME_MASK = 0x7FFFFFF
    SHIFT = 4
    RANDOM_BITS = 4

class UChrono32w(ChronoBase):
    """Unsigned 32-bit Week ID. [Week: 14b][Rand: 18b]. Longevity: ~314 years."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 604800
    TIME_MASK = 0x3FFF
    SHIFT = 18
    RANDOM_BITS = 18

class Chrono32w(ChronoBase):
    """Signed 32-bit Week ID. [0][Week: 14b][Rand: 17b]. Longevity: ~314 years."""
    EPOCH = EPOCH_2000
    TIME_UNIT_DIVISOR = 604800
    TIME_MASK = 0x3FFF
    SHIFT = 17
    RANDOM_BITS = 17

# ==========================================
# 64-BIT FAMILY (Epoch: Jan 1, 1970)
# ==========================================

class UChrono64(ChronoBase):
    """Unsigned 64-bit Second ID. [Sec: 36b][Rand: 28b]. Expires: 4147."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 1
    TIME_MASK = 0xFFFFFFFFF
    SHIFT = 28
    RANDOM_BITS = 28

class Chrono64(ChronoBase):
    """Signed 64-bit Second ID. [0][Sec: 36b][Rand: 27b]. Expires: 4147."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 1
    TIME_MASK = 0xFFFFFFFFF
    SHIFT = 27
    RANDOM_BITS = 27

class UChrono64ms(ChronoBase):
    """Unsigned 64-bit Millisecond ID. [MS: 44b][Rand: 20b]. Expires: 2527."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.001
    TIME_MASK = 0xFFFFFFFFFFF
    SHIFT = 20
    RANDOM_BITS = 20

class Chrono64ms(ChronoBase):
    """Signed 64-bit Millisecond ID. [0][MS: 44b][Rand: 19b]. Expires: 2527."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.001
    TIME_MASK = 0xFFFFFFFFFFF
    SHIFT = 19
    RANDOM_BITS = 19

class UChrono64us(ChronoBase):
    """Unsigned 64-bit Microsecond ID. [us: 54b][Rand: 10b]. Expires: 2540."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.000001
    TIME_MASK = 0x3FFFFFFFFFFFFF
    SHIFT = 10
    RANDOM_BITS = 10

class Chrono64us(ChronoBase):
    """Signed 64-bit Microsecond ID. [0][us: 54b][Rand: 9b]. Expires: 2540."""
    EPOCH = 0
    TIME_UNIT_DIVISOR = 0.000001
    TIME_MASK = 0x3FFFFFFFFFFFFF
    SHIFT = 9
    RANDOM_BITS = 9
