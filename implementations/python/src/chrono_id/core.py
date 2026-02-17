"""
Core implementation of Chrono-ID (Diamond Standard).

This module contains the central logic for generating and parsing Chrono-IDs,
following the formal mathematical proof and specifications.

Key Features:
- 2020-01-01 Epoch.
- Birthday Shield Weyl-Golden Self-Healing Mixer.
- Precision-specific bit splits for 64-bit and 32-bit families.
- Deterministic extraction of timestamp, node_id, and sequence.
"""

from datetime import datetime, timezone
import secrets
import threading
from enum import IntEnum
from typing import Optional, Dict, List, Type, TypeVar, Union, Any, Callable
from .weyl import WEYL_MULTIPLIERS

# --- Precomputed Weyl Cache ---
_WEYL_CACHE: Dict[int, List[int]] = {}


def _get_weyl_mults(bits: int) -> List[int]:
    """
    Returns a list of 128 pre-calculated multipliers for the given bit-width.

    Multipliers are made ODD to ensure full period coverage (Diamond Standard requirement).
    Calculated once per bit-width and cached globally for O(1) retrieval during class definition.
    """
    if bits not in _WEYL_CACHE:
        if bits <= 0:
            # For variants with 0 entropy bits (e.g. 32bs), we provide a dummy multiplier list.
            # While specialized from_parts methods bypass this, it's kept for generic safety.
            _WEYL_CACHE[bits] = [1] * 128
        else:
            # The Golden Ratio multipliers are derived from 64-bit CS-Primes.
            # We shift them to fit the target bit-width while preserving the leading bit.
            shift = 64 - bits
            _WEYL_CACHE[bits] = [((m >> shift) | 1) for m in WEYL_MULTIPLIERS]
    return _WEYL_CACHE[bits]


# --- Constants ---
EPOCH_2020 = 1577836800  # Unix timestamp for 2020-01-01T00:00:00Z
EPOCH_YEAR = 2020


class ChronoError(ValueError):
    """Base exception for all Chrono-ID related errors."""

    pass


class Precision(IntEnum):
    """
    Enum defining all supported time precisions for Chrono-ID.
    Ordered from lowest resolution (Year) to highest resolution (Microsecond).
    """

    Y = 0  # Year
    HY = 1  # Half-Year
    Q = 2  # Quarter
    MO = 3  # Month
    W = 4  # Week
    D = 5  # Day
    H = 6  # Hour
    TM = 7  # Ten-Minute
    M = 8  # Minute
    BS = 9  # Binary Second (2 seconds)
    S = 10  # Second
    DS = 11  # Decisecond (100ms)
    CS = 12  # Centisecond (10ms)
    MS = 13  # Millisecond
    US = 14  # Microsecond


# --- Dispatch Tables for High-Performance Timestamping ---
def _ts_y(dt: datetime) -> int:
    return dt.year - EPOCH_YEAR


def _ts_hy(dt: datetime) -> int:
    return (dt.year - EPOCH_YEAR) * 2 + (0 if dt.month <= 6 else 1)


def _ts_q(dt: datetime) -> int:
    return (dt.year - EPOCH_YEAR) * 4 + (dt.month - 1) // 3


def _ts_mo(dt: datetime) -> int:
    return (dt.year - EPOCH_YEAR) * 12 + (dt.month - 1)


def _ts_w(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 604800


def _ts_d(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 86400


def _ts_h(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 3600


def _ts_tm(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 600


def _ts_m(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 60


def _ts_bs(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) // 2


def _ts_s(dt: datetime) -> int:
    return int(dt.timestamp()) - EPOCH_2020


def _ts_ds(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) * 10 + (dt.microsecond // 100000)


def _ts_cs(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) * 100 + (dt.microsecond // 10000)


def _ts_ms(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) * 1000 + (dt.microsecond // 1000)


def _ts_us(dt: datetime) -> int:
    return (int(dt.timestamp()) - EPOCH_2020) * 1000000 + dt.microsecond


def _ensure_utc(dt: datetime) -> datetime:
    """
    Ensures that the input datetime is UTC-normalized.
    If naive, it is assumed to be UTC (to maintain cross-platform parity).
    If aware, it is converted to UTC.
    """
    if dt.tzinfo is None:
        return dt.replace(tzinfo=timezone.utc)
    return dt.astimezone(timezone.utc)


# Array-based dispatch for O(1) timestamp computation.
TS_COMPUTE: List[Callable[[datetime], int]] = [
    _ts_y,
    _ts_hy,
    _ts_q,
    _ts_mo,
    _ts_w,
    _ts_d,
    _ts_h,
    _ts_tm,
    _ts_m,
    _ts_bs,
    _ts_s,
    _ts_ds,
    _ts_cs,
    _ts_ms,
    _ts_us,
]


def _rev_y(ts: int) -> float:
    return datetime(EPOCH_YEAR + ts, 1, 1, tzinfo=timezone.utc).timestamp()


def _rev_hy(ts: int) -> float:
    return datetime(
        EPOCH_YEAR + (ts // 2), 1 if (ts % 2 == 0) else 7, 1, tzinfo=timezone.utc
    ).timestamp()


def _rev_q(ts: int) -> float:
    return datetime(
        EPOCH_YEAR + (ts // 4), ((ts % 4) * 3) + 1, 1, tzinfo=timezone.utc
    ).timestamp()


def _rev_mo(ts: int) -> float:
    return datetime(
        EPOCH_YEAR + (ts // 12), (ts % 12) + 1, 1, tzinfo=timezone.utc
    ).timestamp()


def _rev_w(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 604800))


def _rev_d(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 86400))


def _rev_h(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 3600))


def _rev_tm(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 600))


def _rev_m(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 60))


def _rev_bs(ts: int) -> float:
    return float(EPOCH_2020 + (ts * 2))


def _rev_s(ts: int) -> float:
    return float(EPOCH_2020 + ts)


def _rev_ds(ts: int) -> float:
    return EPOCH_2020 + (ts / 10)


def _rev_cs(ts: int) -> float:
    return EPOCH_2020 + (ts / 100)


def _rev_ms(ts: int) -> float:
    return EPOCH_2020 + (ts / 1000)


def _rev_us(ts: int) -> float:
    return EPOCH_2020 + (ts / 1000000)


# Array-based dispatch for O(1) timestamp reversal (decoding).
TS_REVERSE: List[Callable[[int], float]] = [
    _rev_y,
    _rev_hy,
    _rev_q,
    _rev_mo,
    _rev_w,
    _rev_d,
    _rev_h,
    _rev_tm,
    _rev_m,
    _rev_bs,
    _rev_s,
    _rev_ds,
    _rev_cs,
    _rev_ms,
    _rev_us,
]


class WeylMixer:
    """
    Implements the Weyl self-healing mixing logic.
    Uses large primes (multipliers) to distribute bits across the suffix space.
    """

    @staticmethod
    def mix(v: int, mask: int, mult: int, salt: int) -> int:
        """
        High-performance mix using precomputed values.
        """
        return ((v * mult) ^ salt) & mask


class Persona:
    """
    Represents an autonomous generation state (Mode A: Stateless).

    The 'Persona' acts as the RNG-gatekeeper for Chrono-ID. It fetches a large block
    of entropy once and distributes it across all parameters (salts, indices, offsets).
    This enables high-performance rotations and per-persona uniqueness.
    """

    __slots__ = (
        "node_id",
        "node_salt",
        "node_idx",
        "seq_salt",
        "seq_idx",
        "seq_offset",
        "last_rotate",
    )

    node_id: int
    node_salt: int
    node_idx: int
    seq_salt: int
    seq_idx: int
    seq_offset: int
    last_rotate: float

    def __init__(self, s_bits: int = 15) -> None:
        """Initializes a new persona with a random 128-bit entropy seed."""
        self._reseed(s_bits)

    def rotate(self, s_bits: int = 15) -> None:
        """Standard rotation method used to refresh persona parameters."""
        self._reseed(s_bits)

    def _reseed(self, s_bits: int, ts: Optional[float] = None) -> None:
        """
        Internal worker that fetches 128-bits of entropy and splits it.

        This method minimizes calls to 'secrets.randbits' by pulling all required
        entropy for node ID, multipliers, and sequence offsets in a single pass.
        """
        pool = secrets.randbits(128)
        self.node_id = pool & 0xFFFF  # 16-bit Node ID
        self.node_salt = (pool >> 16) & 0xFFFF  # 16-bit Node Salt
        self.node_idx = (pool >> 32) & 0x7F  # 7-bit Multiplier Index
        self.seq_salt = (pool >> 39) & 0xFFFF  # 16-bit Sequence Salt
        self.seq_idx = (pool >> 55) & 0x7F  # 7-bit Multiplier Index

        # Sequence offset matches the bit-width of the variant to ensure
        # that even when sequence is reset to 0, personas start from different points.
        offset_mask = (1 << s_bits) - 1 if s_bits > 0 else 0
        self.seq_offset = (pool >> 62) & offset_mask
        self.last_rotate = (
            ts if ts is not None else datetime.now(timezone.utc).timestamp()
        )


# --- Thread-Local Storage ---
# Used to hold per-thread personas for stateless 'generate()' calls.
# This ensures thread-safety without requiring global locks.
_TLS = threading.local()


class GeneratorState:
    """Specialized container for per-thread generator state."""

    __slots__ = ("persona", "last_ts", "sequence")

    persona: Persona
    last_ts: int
    sequence: int

    def __init__(self, cls: Type["ChronoBase"]) -> None:
        self.persona = Persona(cls.S_BITS)
        self.last_ts = 0
        self.sequence = 0


def _get_thread_state(cls: Type["ChronoBase"]) -> GeneratorState:
    """Retrieves or initializes generator state for the current thread."""
    if not hasattr(_TLS, "states"):
        _TLS.states = {}
    if cls not in _TLS.states:
        _TLS.states[cls] = GeneratorState(cls)
    return _TLS.states[cls]


class Generator:
    """
    Stateful Chrono-ID generator for high-frequency uniqueness.
    """

    __slots__ = ("cls", "persona", "last_ts", "sequence", "seq_mask")

    cls: Type["ChronoBase"]
    persona: Persona
    last_ts: int
    sequence: int
    seq_mask: int

    def __init__(
        self, cls: Type["ChronoBase"], persona: Optional[Persona] = None
    ) -> None:
        """
        Initializes the generator.

        Args:
            cls: A specific Chrono-ID variant class (e.g., Chrono64s).
            persona: Optional Persona object.
        """
        self.cls = cls
        self.persona = persona or Persona(s_bits=self.cls.S_BITS)
        self.last_ts = 0
        self.sequence = 0
        self.seq_mask = (1 << self.cls.S_BITS) - 1 if self.cls.S_BITS > 0 else 0

    def generate(self, dt: Optional[datetime] = None) -> "ChronoBase":
        """Generates a unique ID, incrementing sequence if in the same time window."""
        if dt is None:
            dt = datetime.now(timezone.utc)
        else:
            dt = _ensure_utc(dt)
        ts_unix = dt.timestamp()
        ts = TS_COMPUTE[self.cls.PRECISION](dt)

        if ts > self.last_ts:
            self.last_ts = ts
            self.sequence = 0
        elif ts == self.last_ts:
            self.sequence = (self.sequence + 1) & self.seq_mask
            if self.sequence == 0:
                self.persona.rotate(self.cls.S_BITS)  # Overflow rotation
        else:
            # ROLLBACK DETECTED: ts < last_ts
            # Force rotation to ensure uniqueness even if coordinates are reused.
            self.persona.rotate(self.cls.S_BITS)
            self.sequence = (self.sequence + 1) & self.seq_mask

        # Automatic rotation for Mode A every 60 seconds
        if ts_unix - self.persona.last_rotate > 60:
            self.persona._reseed(self.cls.S_BITS, ts=ts_unix)

        return self.cls.from_persona(
            dt, self.persona.node_id, self.sequence, self.persona, ts=ts
        )


T = TypeVar("T", bound="ChronoBase")


class ChronoBase(int):
    """
    Base class for all Chrono-ID variants. Inherits from 'int' for transparent usage.
    """

    PRECISION: Precision = Precision.S
    T_BITS: int = 0
    N_BITS: int = 0
    S_BITS: int = 0
    SIGNED: bool = True

    # Precomputed values (populated at class definition)
    T_MASK: int = 0  # Precomputed mask for timestamp
    N_MASK: int = 0  # Precomputed mask for Node segment
    S_MASK: int = 0  # Precomputed mask for Sequence segment
    T_SHIFT: int = 0  # Precomputed shift (N_BITS + S_BITS) to align timestamp
    N_MULT: List[int] = []  # Cache pointer to odd-prime multipliers for Node segment
    S_MULT: List[int] = (
        []
    )  # Cache pointer to odd-prime multipliers for Sequence segment

    def __init_subclass__(cls, **kwargs: Any) -> None:
        """
        Definition-time optimization logic.

        When a variant class is defined (e.g. class Chrono64s), we calculate all
        logical constants and multipliers once. This ensures that the 'from_parts'
        hot path contains zero addition, subtraction, or shift-calculation logic.
        """
        super().__init_subclass__(**kwargs)
        cls.T_MASK = (1 << cls.T_BITS) - 1 if cls.T_BITS > 0 else 0
        cls.N_MASK = (1 << cls.N_BITS) - 1 if cls.N_BITS > 0 else 0
        cls.S_MASK = (1 << cls.S_BITS) - 1 if cls.S_BITS > 0 else 0
        cls.T_SHIFT = cls.N_BITS + cls.S_BITS

        # Multipliers are pulled from a shared global pool to optimize memory.
        cls.N_MULT = _get_weyl_mults(cls.N_BITS)
        cls.S_MULT = _get_weyl_mults(cls.S_BITS)

    def __new__(cls: Type[T], value: Optional[Union[int, str]] = None) -> T:
        """Constructs a Chrono-ID from an integer value or hex/hyphenated string."""
        if value is None:
            return cls.generate()
        if isinstance(value, str) and (
            "-" in value or " " in value or any(c in "abcdefABCDEF" for c in value)
        ):
            # If it looks like a formatted string (hyphens, spaces, or hex chars),
            # use the formatted parser.
            try:
                return cls.from_formatted(value)
            except (ValueError, ChronoError):
                # Fallback to decimal parsing if it's just a numeric string
                pass
        return super().__new__(cls, int(value))

    @classmethod
    def from_formatted(cls: Type[T], formatted: str) -> T:
        """
        Parses a hyphenated or plain hex string into a Chrono-ID.
        Supports 'XXXX-XXXX', 'XXXX XXXX', and plain 'XXXXXXXX'.
        """
        if formatted is None:
            raise ChronoError("Input string is null")
        clean = formatted.replace("-", "").replace(" ", "")
        try:
            return cls(int(clean, 16))
        except ValueError:
            raise ChronoError(f"Invalid hex format for {cls.__name__}: {formatted}")

    @classmethod
    def generate(
        cls,
        dt: Optional[datetime] = None,
        node_id: Optional[int] = None,
        seq: Optional[int] = None,
        persona: Optional[Persona] = None,
    ) -> T:
        """
        Generates a new Chrono-ID.

        Thread-safe: Uses thread-local storage if no persona is provided.
        Note: For high-frequency use, use the 'Generator' class for stateful uniqueness.
        """
        if dt is None:
            dt = datetime.now(timezone.utc)
        else:
            dt = _ensure_utc(dt)

        if persona is None:
            state = _get_thread_state(cls)
            persona = state.persona
            ts = TS_COMPUTE[cls.PRECISION](dt)

            if ts > state.last_ts:
                state.last_ts = ts
                state.sequence = 0
            elif ts == state.last_ts:
                state.sequence = (state.sequence + 1) & (
                    (1 << cls.S_BITS) - 1 if cls.S_BITS > 0 else 0
                )
                if state.sequence == 0:
                    persona.rotate(cls.S_BITS)
            else:
                # Rollback
                persona.rotate(cls.S_BITS)
                state.sequence = (state.sequence + 1) & (
                    (1 << cls.S_BITS) - 1 if cls.S_BITS > 0 else 0
                )

            if dt.timestamp() - persona.last_rotate > 60:
                persona.rotate(cls.S_BITS)

            if node_id is None:
                node_id = persona.node_id
            if seq is None:
                seq = state.sequence
        else:
            if node_id is None:
                node_id = persona.node_id
            if seq is None:
                seq = secrets.randbits(cls.S_BITS) if cls.S_BITS > 0 else 0

        return cls.from_persona(dt, node_id, seq, persona)

    @classmethod
    def from_persona(
        cls: Type[T],
        dt: datetime,
        node_id: int,
        seq: int,
        persona: Persona,
        ts: Optional[int] = None,
    ) -> T:
        """
        High-performance assembly using a Persona object.

        This is the PRIMARY hot path for ID generation. It uses the precomputed
        constants and inlined mixing logic to minimize CPU cycles.

        Args:
            dt: Target datetime.
            node_id: Explicit node ID (usually from persona).
            seq: The incrementing sequence value.
            persona: The persona providing salts and multiplier indices.
            ts: Optional pre-calculated timestamp value to avoid re-computation.
        """
        if dt is None:
            raise ChronoError("Input date is null")
        dt = _ensure_utc(dt)
        if dt.timestamp() < EPOCH_2020:
            raise ChronoError("Timestamp underflow: Date is before Epoch (2020-01-01)")
        ts_val = (ts if ts is not None else TS_COMPUTE[cls.PRECISION](dt)) & cls.T_MASK

        # Hot Path: Inlined Weyl-Golden Mixing
        # Multipliers ensure that the entropy space is covered uniformly (self-healing).
        mix_n = (
            (node_id * cls.N_MULT[persona.node_idx]) ^ persona.node_salt
        ) & cls.N_MASK
        s_val = (seq + persona.seq_offset) & cls.S_MASK
        mix_s = ((s_val * cls.S_MULT[persona.seq_idx]) ^ persona.seq_salt) & cls.S_MASK

        val = (ts_val << cls.T_SHIFT) | (mix_n << cls.S_BITS) | mix_s
        return super().__new__(cls, val)

    @classmethod
    def from_parts(
        cls: Type[T],
        dt: datetime,
        node_id: int,
        seq: int,
        p_idx: int = 0,
        salt: int = 0,
        ts: Optional[int] = None,
        persona: Optional[Persona] = None,
    ) -> T:
        """
        Assembles a Chrono-ID from individual parts.

        Useful for manual ID construction when a 'Persona' object is not available.
        Uses a shared 'p_idx' and 'salt' for both node and sequence segments.
        If 'persona' is provided, it overrides p_idx and salt.
        """
        if dt is None:
            raise ChronoError("Input date is null")
        if dt.timestamp() < EPOCH_2020:
            raise ChronoError("Timestamp underflow: Date is before Epoch (2020-01-01)")

        if persona:
            return cls.from_persona(dt, node_id, seq, persona, ts=ts)

        ts_val = (ts if ts is not None else TS_COMPUTE[cls.PRECISION](dt)) & cls.T_MASK

        mix_n = ((node_id * cls.N_MULT[p_idx % 128]) ^ salt) & cls.N_MASK
        mix_s = ((seq * cls.S_MULT[p_idx % 128]) ^ salt) & cls.S_MASK

        val = (ts_val << cls.T_SHIFT) | (mix_n << cls.S_BITS) | mix_s
        return super().__new__(cls, val)

    @classmethod
    def from_time(cls: Type[T], dt: datetime, random_val: Optional[int] = None) -> T:
        """
        Compatibility method for constructing IDs with explicit random entropy.

        Args:
            dt: Target datetime.
            random_val: Combined node_id and sequence entropy.
        """
        if random_val is not None:
            mask_s = (1 << cls.S_BITS) - 1
            node_id = (
                (random_val >> cls.S_BITS) & ((1 << cls.N_BITS) - 1)
                if cls.N_BITS > 0
                else 0
            )
            seq = random_val & mask_s
            return cls.generate(dt=dt, node_id=node_id, seq=seq)
        return cls.generate(dt=dt)

    @classmethod
    def from_iso_string(cls: Type[T], iso: str) -> T:
        """Parses an ISO 8601 string and generates an ID."""
        if iso is None:
            raise ChronoError("Input string is null")
        try:
            iso_clean = iso.replace("Z", "+00:00")
            dt = datetime.fromisoformat(iso_clean)
        except (ValueError, TypeError):
            raise ChronoError("Invalid ISO 8601 format")
        return cls.generate(dt=dt)

    def get_time(self) -> datetime:
        """Decodes the embedded timestamp from the ID."""
        ts_val = int(self) >> self.T_SHIFT
        unix_ts = TS_REVERSE[self.PRECISION](ts_val)
        return datetime.fromtimestamp(unix_ts, tz=timezone.utc)

    def to_iso_string(self) -> str:
        """Converts the ID's embedded time to an ISO 8601 string."""
        dt = self.get_time()
        base = dt.strftime("%Y-%m-%dT%H:%M:%S")
        if self.PRECISION == Precision.MS:
            return f"{base}.{dt.microsecond // 1000:03d}Z"
        if self.PRECISION == Precision.US:
            return f"{base}.{dt.microsecond:06d}Z"
        return f"{base}Z"

    def formatted(self) -> str:
        """
        Returns the standard hyphenated hex representation.
        32-bit tier: XXXX-XXXX (9 chars)
        64-bit tier: XXXX-XXXX-XXXX-XXXX (19 chars)
        """
        # Tiered padding strategy: 32-bit family vs 64-bit family.
        width = self.T_BITS + self.N_BITS + self.S_BITS
        if self.SIGNED:
            width += 1

        # Use 16 hex digits (4 chunks) for 64-bit tier, 8 digits (2 chunks) for 32-bit.
        hex_len = 16 if width > 32 else 8
        h = f"{int(self):0{hex_len}X}"

        # Split into 4-character chunks separated by hyphens
        parts = [h[i : i + 4] for i in range(0, len(h), 4)]
        return "-".join(parts)

    def __str__(self) -> str:
        return self.formatted()

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.formatted()})"


class ChronoBasebs(ChronoBase):
    """
    Specialized base for zero-entropy variants (e.g., Binary Second).
    Removes the overhead of mixing logic entirely.
    """

    @classmethod
    def from_persona(
        cls: Type[T],
        dt: datetime,
        node_id: int = 0,
        seq: int = 0,
        persona: Optional[Persona] = None,
        ts: Optional[int] = None,
    ) -> T:
        """
        Zero-overhead assembly for timestamp-only variants (e.g. BS).

        Bypasses all mixing and shift logic because entropy bits (N/S) are zero.
        Used by 'uchrono32bs' and 'chrono32bs'.
        """
        if dt is None:
            raise ChronoError("Input date is null")
        if dt.timestamp() < EPOCH_2020:
            raise ChronoError("Timestamp underflow: Date is before Epoch (2020-01-01)")
        ts_val = (ts if ts is not None else TS_COMPUTE[cls.PRECISION](dt)) & cls.T_MASK
        return super().__new__(cls, ts_val)

    @classmethod
    def from_parts(
        cls: Type[T],
        dt: datetime,
        node_id: int = 0,
        seq: int = 0,
        p_idx: int = 0,
        salt: int = 0,
        ts: Optional[int] = None,
        persona: Optional[Persona] = None,
    ) -> T:
        """Zero-overhead assembly for timestamp-only variants (e.g. BS)."""
        if dt is None:
            raise ChronoError("Input date is null")
        if dt.timestamp() < EPOCH_2020:
            raise ChronoError("Timestamp underflow: Date is before Epoch (2020-01-01)")
        ts_val = (ts if ts is not None else TS_COMPUTE[cls.PRECISION](dt)) & cls.T_MASK
        return super().__new__(cls, ts_val)


# --- 64-bit High Entropy Family ---


class UChrono64mo(ChronoBase):
    """Unsigned 64-bit Month ID. [T:12][N:26][S:26]. 341 years range."""

    PRECISION = Precision.MO
    T_BITS, N_BITS, S_BITS = 12, 26, 26
    SIGNED = False


class Chrono64mo(ChronoBase):
    """Signed 64-bit Month ID. [0][T:12][N:25][S:26]."""

    PRECISION = Precision.MO
    T_BITS, N_BITS, S_BITS = 12, 25, 26


class UChrono64w(ChronoBase):
    """Unsigned 64-bit Week ID. [T:14][N:26][S:24]. 315 years range."""

    PRECISION = Precision.W
    T_BITS, N_BITS, S_BITS = 14, 26, 24
    SIGNED = False


class Chrono64w(ChronoBase):
    """Signed 64-bit Week ID. [0][T:14][N:25][S:24]."""

    PRECISION = Precision.W
    T_BITS, N_BITS, S_BITS = 14, 25, 24


class UChrono64d(ChronoBase):
    """Unsigned 64-bit Day ID. [T:17][N:24][S:23]. 358 years range."""

    PRECISION = Precision.D
    T_BITS, N_BITS, S_BITS = 17, 24, 23
    SIGNED = False


class Chrono64d(ChronoBase):
    """Signed 64-bit Day ID. [0][T:17][N:23][S:23]."""

    PRECISION = Precision.D
    T_BITS, N_BITS, S_BITS = 17, 23, 23


class UChrono64h(ChronoBase):
    """Unsigned 64-bit Hour ID. [T:21][N:22][S:21]. 239 years range."""

    PRECISION = Precision.H
    T_BITS, N_BITS, S_BITS = 21, 22, 21
    SIGNED = False


class Chrono64h(ChronoBase):
    """Signed 64-bit Hour ID. [0][T:21][N:21][S:21]."""

    PRECISION = Precision.H
    T_BITS, N_BITS, S_BITS = 21, 21, 21


class UChrono64m(ChronoBase):
    """Unsigned 64-bit Minute ID. [T:27][N:19][S:18]. 255 years range."""

    PRECISION = Precision.M
    T_BITS, N_BITS, S_BITS = 27, 19, 18
    SIGNED = False


class Chrono64m(ChronoBase):
    """Signed 64-bit Minute ID. [0][T:27][N:18][S:18]."""

    PRECISION = Precision.M
    T_BITS, N_BITS, S_BITS = 27, 18, 18


class UChrono64s(ChronoBase):
    """Unsigned 64-bit Second ID. [T:33][N:16][S:15]. 272 years range."""

    PRECISION = Precision.S
    T_BITS, N_BITS, S_BITS = 33, 16, 15
    SIGNED = False


class Chrono64s(ChronoBase):
    """Signed 64-bit Second ID. [0][T:33][N:15][S:15]."""

    PRECISION = Precision.S
    T_BITS, N_BITS, S_BITS = 33, 15, 15


# --- 64-bit High Frequency Family ---


class UChrono64ds(ChronoBase):
    """Unsigned 64-bit Decisecond (100ms) ID. [T:36][N:15][S:13]. 217 years range."""

    PRECISION = Precision.DS
    T_BITS, N_BITS, S_BITS = 36, 15, 13
    SIGNED = False


class Chrono64ds(ChronoBase):
    """Signed 64-bit Decisecond ID. [0][T:36][N:14][S:13]."""

    PRECISION = Precision.DS
    T_BITS, N_BITS, S_BITS = 36, 14, 13


class UChrono64cs(ChronoBase):
    """Unsigned 64-bit Centisecond (10ms) ID. [T:40][N:12][S:12]. 348 years range."""

    PRECISION = Precision.CS
    T_BITS, N_BITS, S_BITS = 40, 12, 12
    SIGNED = False


class Chrono64cs(ChronoBase):
    """Signed 64-bit Centisecond ID. [0][T:40][N:11][S:12]."""

    PRECISION = Precision.CS
    T_BITS, N_BITS, S_BITS = 40, 11, 12


class UChrono64ms(ChronoBase):
    """Unsigned 64-bit Millisecond ID. [T:43][N:11][S:10]. 279 years range."""

    PRECISION = Precision.MS
    T_BITS, N_BITS, S_BITS = 43, 11, 10
    SIGNED = False


class Chrono64ms(ChronoBase):
    """Signed 64-bit Millisecond ID. [0][T:43][N:10][S:10]."""

    PRECISION = Precision.MS
    T_BITS, N_BITS, S_BITS = 43, 10, 10


class UChrono64us(ChronoBase):
    """Unsigned 64-bit Microsecond ID. [T:53][N:6][S:5]. 285 years range."""

    PRECISION = Precision.US
    T_BITS, N_BITS, S_BITS = 53, 6, 5
    SIGNED = False


class Chrono64us(ChronoBase):
    """Signed 64-bit Microsecond ID. [0][T:53][N:5][S:5]."""

    PRECISION = Precision.US
    T_BITS, N_BITS, S_BITS = 53, 5, 5


# --- 32-bit Identity Family ---


class UChrono32y(ChronoBase):
    """Unsigned 32-bit Year ID. [T:8][N:13][S:11]. 256 years range."""

    PRECISION = Precision.Y
    T_BITS, N_BITS, S_BITS = 8, 13, 11
    SIGNED = False


class Chrono32y(ChronoBase):
    """Signed 32-bit Year ID. [0][T:8][N:12][S:11]."""

    PRECISION = Precision.Y
    T_BITS, N_BITS, S_BITS = 8, 12, 11


class UChrono32hy(ChronoBase):
    """Unsigned 32-bit Half-Year ID. [T:9][N:12][S:11]. 256 years range."""

    PRECISION = Precision.HY
    T_BITS, N_BITS, S_BITS = 9, 12, 11
    SIGNED = False


class Chrono32hy(ChronoBase):
    """Signed 32-bit Half-Year ID. [0][T:9][N:11][S:11]."""

    PRECISION = Precision.HY
    T_BITS, N_BITS, S_BITS = 9, 11, 11


class UChrono32q(ChronoBase):
    """Unsigned 32-bit Quarter ID. [T:10][N:11][S:11]. 256 years range."""

    PRECISION = Precision.Q
    T_BITS, N_BITS, S_BITS = 10, 11, 11
    SIGNED = False


class Chrono32q(ChronoBase):
    """Signed 32-bit Quarter ID. [0][T:10][N:10][S:11]."""

    PRECISION = Precision.Q
    T_BITS, N_BITS, S_BITS = 10, 10, 11


class UChrono32mo(ChronoBase):
    """Unsigned 32-bit Month ID. [T:12][N:10][S:10]. 341 years range."""

    PRECISION = Precision.MO
    T_BITS, N_BITS, S_BITS = 12, 10, 10
    SIGNED = False


class Chrono32mo(ChronoBase):
    """Signed 32-bit Month ID. [0][T:12][N:9][S:10]."""

    PRECISION = Precision.MO
    T_BITS, N_BITS, S_BITS = 12, 9, 10


class UChrono32w(ChronoBase):
    """Unsigned 32-bit Week ID. [T:14][N:9][S:9]. 315 years range."""

    PRECISION = Precision.W
    T_BITS, N_BITS, S_BITS = 14, 9, 9
    SIGNED = False


class Chrono32w(ChronoBase):
    """Signed 32-bit Week ID. [0][T:14][N:8][S:9]."""

    PRECISION = Precision.W
    T_BITS, N_BITS, S_BITS = 14, 8, 9


class UChrono32d(ChronoBase):
    """Unsigned 32-bit Day ID. [T:17][N:8][S:7]. 358 years range."""

    PRECISION = Precision.D
    T_BITS, N_BITS, S_BITS = 17, 8, 7
    SIGNED = False


class Chrono32d(ChronoBase):
    """Signed 32-bit Day ID. [0][T:17][N:7][S:7]."""

    PRECISION = Precision.D
    T_BITS, N_BITS, S_BITS = 17, 7, 7


# --- 32-bit Sort-Key Family ---


class UChrono32h(ChronoBase):
    """Unsigned 32-bit Hour ID. [T:22][N:5][S:5]. 478 years range."""

    PRECISION = Precision.H
    T_BITS, N_BITS, S_BITS = 22, 5, 5
    SIGNED = False


class Chrono32h(ChronoBase):
    """Signed 32-bit Hour ID. [0][T:22][N:4][S:5]."""

    PRECISION = Precision.H
    T_BITS, N_BITS, S_BITS = 22, 4, 5


class UChrono32tm(ChronoBase):
    """Unsigned 32-bit Ten-Minute ID. [T:24][N:4][S:4]. 319 years range."""

    PRECISION = Precision.TM
    T_BITS, N_BITS, S_BITS = 24, 4, 4
    SIGNED = False


class Chrono32tm(ChronoBase):
    """Signed 32-bit Ten-Minute ID. [0][T:24][N:3][S:4]."""

    PRECISION = Precision.TM
    T_BITS, N_BITS, S_BITS = 24, 3, 4


class UChrono32m(ChronoBase):
    """Unsigned 32-bit Minute ID. [T:28][N:2][S:2]. 510 years range."""

    PRECISION = Precision.M
    T_BITS, N_BITS, S_BITS = 28, 2, 2
    SIGNED = False


class Chrono32m(ChronoBase):
    """Signed 32-bit Minute ID. [0][T:28][N:1][S:2]."""

    PRECISION = Precision.M
    T_BITS, N_BITS, S_BITS = 28, 1, 2


class UChrono32bs(ChronoBasebs):
    """Unsigned 32-bit Binary Second (2s) ID. [T:32][N:0][S:0]. 272 years range."""

    PRECISION = Precision.BS
    T_BITS, N_BITS, S_BITS = 32, 0, 0
    SIGNED = False


class Chrono32bs(ChronoBasebs):
    """Signed 32-bit Binary Second (2s) ID. [0][T:31][N:0][S:0]."""

    PRECISION = Precision.BS
    T_BITS, N_BITS, S_BITS = 31, 0, 0


# --- End of Core Implementation ---
