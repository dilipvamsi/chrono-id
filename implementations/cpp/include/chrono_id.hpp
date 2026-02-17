/**
 * @file chrono_id.hpp
 * @brief Header-only C++ implementation of the Chrono-ID Diamond Standard.
 *
 * Chrono-ID is a cross-platform, bit-packed identifier format that combines
 * time-based precision, Weyl-Golden mixing entropy.
 */

#ifndef CHRONO_ID_HPP
#define CHRONO_ID_HPP

#include "./multipliers.hpp"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * @namespace chrono_id
 * @brief Standard namespace for all Chrono-ID components.
 */
namespace chrono_id {

/**
 * @class ChronoError
 * @brief Custom exception class for all Chrono-ID library errors.
 *
 * Used for reporting parsing errors, timestamp underflows, or invalid Persona
 * configurations.
 */
class ChronoError : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
};

/**
 * @struct Persona
 * @brief State container for Weyl-Golden mixing entropy.
 *
 * A Persona allows uncoordinated distributed nodes to generate unique IDs
 * by rotating through deterministic sequences and applying salt. This
 * effectively "heals" collisions without cross-node synchronization.
 */
struct Persona {
  uint8_t node_idx;    ///< Index into the Weyl multiplier table for Node ID.
  uint32_t node_salt;  ///< XOR salt for Node ID mixing.
  uint8_t seq_idx;     ///< Index into the Weyl multiplier table for Sequence.
  uint32_t seq_salt;   ///< XOR salt for Sequence mixing.
  uint32_t seq_offset; ///< Offset applied to the base sequence number.

  /**
   * @brief Initializes a Persona with specific indices and salts.
   * @param n_idx Node multiplier index (wrapped % 128).
   * @param n_salt Node salt value.
   * @param s_idx Sequence multiplier index (wrapped % 128).
   * @param s_salt Sequence salt value.
   * @param s_offset Sequence offset value.
   */
  Persona(uint8_t n_idx = 0, uint32_t n_salt = 0, uint8_t s_idx = 0,
          uint32_t s_salt = 0, uint32_t s_offset = 0)
      : node_idx(n_idx % 128), node_salt(n_salt), seq_idx(s_idx % 128),
        seq_salt(s_salt), seq_offset(s_offset) {}

  /**
   * @brief Generates a completely random Persona.
   *
   * Use this at application startup to ensure your node occupies a unique
   * entropy lane without needing a central coordinator.
   */
  static Persona random() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist;
    std::uniform_int_distribution<uint8_t> idx_dist(0, 127);
    return Persona(idx_dist(gen), dist(gen), idx_dist(gen), dist(gen),
                   dist(gen));
  }
};

/**
 * @namespace chrono_id::detail
 * @brief Internal implementation details (not part of the public API).
 */
namespace detail {
/**
 * @brief Generates cryptographically secure random bits using
 * std::random_device.
 * @param bits Number of bits to generate.
 * @return A 64-bit integer containing the requested random bits.
 */
inline uint64_t get_random_bits(uint8_t bits) {
  if (bits == 0)
    return 0;
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(0, (1ULL << bits) - 1);
  return dis(gen);
}

/**
 * @brief Converts civil date to days since Unix Epoch.
 * @param y Year.
 * @param m Month [1-12].
 * @param d Day [1-31].
 * @return Days elapsed since 1970-01-01.
 */
inline int64_t days_from_civil(int y, int m, int d) {
  y -= m <= 2;
  const int64_t era = (y >= 0 ? y : y - 399) / 400;
  const uint32_t yoe = static_cast<uint32_t>(y - era * 400);
  const uint32_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const uint32_t ioe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + static_cast<int64_t>(ioe) - 719468;
}

// Fixed time unit constants (in microseconds)
#define CHRONO_US_W 604800000000ULL ///< One week.
#define CHRONO_US_D 86400000000ULL  ///< One day.
#define CHRONO_US_H 3600000000ULL   ///< One hour.
#define CHRONO_US_TM 600000000ULL   ///< Ten minutes.
#define CHRONO_US_M 60000000ULL     ///< One minute.
#define CHRONO_US_BS 2000000ULL     ///< Two seconds (Beacon period).
#define CHRONO_US_S 1000000ULL      ///< One second.
#define CHRONO_US_DS 100000ULL      ///< One decisecond.
#define CHRONO_US_CS 10000ULL       ///< One centisecond.
#define CHRONO_US_MS 1000ULL        ///< One millisecond.
#define CHRONO_US_US 1ULL           ///< One microsecond.

/// Standard Epoch for Chrono-ID: 2020-01-01T00:00:00Z.
constexpr uint64_t EPOCH_2020_SEC = 1577836800ULL;

/// Year offset for C time compatibility.
constexpr int TM_YEAR_EPOCH_OFFSET = 1900 - 2020;

/**
 * @brief Weyl-Golden mixing function.
 * @param val Base value to mix (Node or Seq).
 * @param bits Width of the target field.
 * @param p_idx Persona multiplier index.
 * @param salt XOR salt.
 * @return Mixed 64-bit value masked to bit width.
 */
inline uint64_t mix(uint64_t val, uint8_t bits, uint8_t p_idx, uint64_t salt) {
  if (bits == 0)
    return 0;
  uint64_t mask = (1ULL << bits) - 1;
  uint64_t mult = (WEYL_MULTIPLIERS[p_idx % 128] >> (64 - bits)) | 1ULL;
  return ((val * mult) ^ salt) & mask;
}

/// Static map for microsecond units used in Precision lookups.
constexpr uint64_t US_MAP[] = {
    0ULL,         // Y
    0ULL,         // HY
    0ULL,         // Q
    0ULL,         // MO
    CHRONO_US_W,  // W
    CHRONO_US_D,  // D
    CHRONO_US_H,  // H
    CHRONO_US_TM, // TM
    CHRONO_US_M,  // M
    CHRONO_US_BS, // BS
    CHRONO_US_S,  // S
    CHRONO_US_DS, // DS
    CHRONO_US_CS, // CS
    CHRONO_US_MS, // MS
    CHRONO_US_US  // US
};

/**
 * @brief Function pointer type for JS-style unit dispatch.
 */
typedef uint64_t (*TSFunc)(std::chrono::system_clock::time_point, uint64_t);

/**
 * @brief Cross-platform gmtime wrapper.
 */
inline std::tm get_gmtime(std::chrono::system_clock::time_point tp) {
  auto tt = std::chrono::system_clock::to_time_t(tp);
  std::tm tm_buf;
#ifdef _WIN32
  gmtime_s(&tm_buf, &tt);
#else
  gmtime_r(&tt, &tm_buf);
#endif
  return tm_buf;
}

/**
 * @brief Calculator for Year precision.
 */
inline uint64_t calc_y(std::chrono::system_clock::time_point tp, uint64_t) {
  auto tm = get_gmtime(tp);
  return static_cast<uint64_t>(tm.tm_year + TM_YEAR_EPOCH_OFFSET);
}

/**
 * @brief Calculator for Half-Year (Semester) precision.
 */
inline uint64_t calc_hy(std::chrono::system_clock::time_point tp, uint64_t) {
  auto tm = get_gmtime(tp);
  return static_cast<uint64_t>((tm.tm_year + TM_YEAR_EPOCH_OFFSET) * 2 +
                               (tm.tm_mon < 6 ? 0 : 1));
}

/**
 * @brief Calculator for Quarter precision.
 */
inline uint64_t calc_q(std::chrono::system_clock::time_point tp, uint64_t) {
  auto tm = get_gmtime(tp);
  return static_cast<uint64_t>((tm.tm_year + TM_YEAR_EPOCH_OFFSET) * 4 +
                               (tm.tm_mon / 3));
}

/**
 * @brief Calculator for Month precision.
 */
inline uint64_t calc_mo(std::chrono::system_clock::time_point tp, uint64_t) {
  auto tm = get_gmtime(tp);
  return static_cast<uint64_t>((tm.tm_year + TM_YEAR_EPOCH_OFFSET) * 12 +
                               tm.tm_mon);
}

/**
 * @brief Generic division-based calculator for high-precision units.
 * @tparam Div The microsecond divisor for the target precision.
 */
template <uint64_t Div>
inline uint64_t calc_div(std::chrono::system_clock::time_point tp,
                         uint64_t epoch_sec) {
  auto duration = tp.time_since_epoch();
  uint64_t us =
      std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  return (us - epoch_sec * CHRONO_US_S) / Div;
}

/**
 * @brief Internal dispatch table for timestamp unit calculations.
 *
 * Provides O(1) jump to the correct calendar or division logic for any
 * precision.
 */
static constexpr TSFunc TS_COMPUTE[] = {
    calc_y,
    calc_hy,
    calc_q,
    calc_mo,
    calc_div<CHRONO_US_W>,  // W
    calc_div<CHRONO_US_D>,  // D
    calc_div<CHRONO_US_H>,  // H
    calc_div<CHRONO_US_TM>, // TM
    calc_div<CHRONO_US_M>,  // M
    calc_div<CHRONO_US_BS>, // BS
    calc_div<CHRONO_US_S>,  // S
    calc_div<CHRONO_US_DS>, // DS
    calc_div<CHRONO_US_CS>, // CS
    calc_div<CHRONO_US_MS>, // MS
    calc_div<CHRONO_US_US>  // US
};
} // namespace detail

/**
 * @enum Precision
 * @brief Identifies the time-precision level of a Chrono-ID.
 *
 * Each level corresponds to a specific bit-packing strategy and calendar unit.
 */
enum class Precision {
  Y = 0,   ///< Year.
  HY = 1,  ///< Half-Year (Semester).
  Q = 2,   ///< Quarter.
  MO = 3,  ///< Month.
  W = 4,   ///< Week.
  D = 5,   ///< Day.
  H = 6,   ///< Hour.
  TM = 7,  ///< Ten Minutes.
  M = 8,   ///< Minute.
  BS = 9,  ///< Beacon Second (2 seconds).
  S = 10,  ///< Second.
  DS = 11, ///< Decisecond (100ms).
  CS = 12, ///< Centisecond (10ms).
  MS = 13, ///< Millisecond (1ms).
  US = 14  ///< Microsecond (1us).
};

/**
 * @class ChronoID
 * @brief Unified template for all Chrono-ID variants.
 *
 * This class handles the bit-packing, generation, and parsing for a specific
 * configuration.
 *
 * @tparam T The underlying integer type (typically uint64_t or uint32_t).
 * @tparam EpochSec The custom epoch in seconds (default is 2020-01-01).
 * @tparam P The precision level (e.g., Precision::MS).
 * @tparam NBits Number of bits allocated for Node ID entropy.
 * @tparam SBits Number of bits allocated for Sequence entropy.
 */
template <typename T, uint64_t EpochSec, Precision P, uint8_t NBits,
          uint8_t SBits>
struct ChronoID {
  static constexpr uint64_t Epoch = EpochSec; ///< Base epoch seconds.
  static constexpr Precision PRECISION = P;   ///< Variant precision.
  static constexpr uint8_t N_BITS = NBits;    ///< Node bits width.
  static constexpr uint8_t S_BITS = SBits;    ///< Sequence bits width.
  static constexpr uint8_t T_SHIFT =
      NBits + SBits; ///< Bits to shift for timestamp.
  static constexpr uint64_t T_MASK =
      (1ULL << (sizeof(T) * 8 - T_SHIFT - (std::is_signed_v<T> ? 1 : 0))) -
      1ULL; ///< Mask for timestamp units.

  /// The microsecond value of one unit of this precision.
  static constexpr uint64_t UNIT_US = detail::US_MAP[static_cast<int>(P)];

  /**
   * @brief Pre-calculated Weyl Multipliers for Node ID.
   *
   * Optimized at compile-time to match the NBits width of this specific
   * variant.
   */
  static constexpr std::array<uint64_t, 128> N_MULTS = []() {
    std::array<uint64_t, 128> arr{};
    if constexpr (N_BITS > 0) {
      for (int i = 0; i < 128; ++i) {
        arr[i] = (WEYL_MULTIPLIERS[i] >> (64 - N_BITS)) | 1ULL;
      }
    }
    return arr;
  }();

  /**
   * @brief Pre-calculated Weyl Multipliers for Sequence.
   *
   * Optimized at compile-time to match the SBits width of this specific
   * variant.
   */
  static constexpr std::array<uint64_t, 128> S_MULTS = []() {
    std::array<uint64_t, 128> arr{};
    if constexpr (S_BITS > 0) {
      for (int i = 0; i < 128; ++i) {
        arr[i] = (WEYL_MULTIPLIERS[i] >> (64 - S_BITS)) | 1ULL;
      }
    }
    return arr;
  }();

  T value; ///< The raw bit-packed ID value.

  /**
   * @brief Constructs an ID from a raw value.
   */
  explicit ChronoID(T val) : value(val) {}

  /**
   * @brief Default constructor (Current Time + Random Entropy).
   */
  ChronoID() {
    auto now = std::chrono::system_clock::now();
    value = pack(now);
  }

  /**
   * @brief Generates an ID from a specific time point.
   * @param tp The time point.
   * @param rand_val Optional specific entropy bits.
   */
  static ChronoID from_time(std::chrono::system_clock::time_point tp,
                            uint64_t rand_val = 0xFFFFFFFFFFFFFFFFULL) {
    return ChronoID(pack(tp, rand_val));
  }

  /**
   * @brief Generates an ID using specific Persona entropy.
   * @param tp The time point.
   * @param node_id Logical Node ID (e.g., Worker ID).
   * @param seq Sequential incrementor.
   * @param persona Entropy configuration.
   */
  static ChronoID from_persona(std::chrono::system_clock::time_point tp,
                               uint64_t node_id, uint64_t seq,
                               const Persona &persona) {
    return from_persona_units(calculate_units(tp), node_id, seq, persona);
  }

  /**
   * @brief Generates an ID using pre-calculated units (Bypass system_clock
   * limits).
   * @param ts_units Calculated time units (e.g., years since 2020).
   * @param node_id Logical Node ID.
   * @param seq Sequential incrementor.
   * @param persona Entropy configuration.
   */
  static ChronoID from_persona_units(uint64_t ts_units, uint64_t node_id,
                                     uint64_t seq, const Persona &persona) {
    uint64_t n_mask = (1ULL << N_BITS) - 1;
    uint64_t s_mask = (1ULL << S_BITS) - 1;

    uint64_t mix_n = 0;
    if constexpr (N_BITS > 0) {
      mix_n =
          ((node_id * N_MULTS[persona.node_idx % 128]) ^ persona.node_salt) &
          n_mask;
    }
    uint64_t mix_s = 0;
    if constexpr (S_BITS > 0) {
      mix_s = (((seq + persona.seq_offset) * S_MULTS[persona.seq_idx % 128]) ^
               persona.seq_salt) &
              s_mask;
    }
    return ChronoID(((ts_units & T_MASK) << T_SHIFT) | (mix_n << S_BITS) |
                    mix_s);
  }

  /**
   * @brief Constructs an ID from raw timestamp and entropy components.
   */
  static ChronoID from_timestamp(uint64_t ts, uint64_t rand_val) {
    return ChronoID(((ts & T_MASK) << T_SHIFT) |
                    (rand_val & ((1ULL << T_SHIFT) - 1)));
  }

  /**
   * @brief Parses an ISO 8601 string into a ChronoID.
   * @param iso Date string (e.g., "2023-05-20T10:30:00Z").
   * @param rand_val Optional specific entropy bits.
   * @return A ChronoID instance.
   */
  static ChronoID from_iso_cstring(const char *iso,
                                   uint64_t rand_val = 0xFFFFFFFFFFFFFFFFULL) {
    if (!iso)
      throw ChronoError("Input string is null");

    std::string iso_str(iso);
    // Treat naive strings as UTC
    if (iso_str.find('Z') == std::string::npos &&
        iso_str.find('+') == std::string::npos &&
        iso_str.find_last_of('-') < 10) { // last '-' should be in YYYY-MM-DD
      iso_str += 'Z';
    }

    int y, m, d, hh, mm, ss;
    char dash, t, colon;
    std::stringstream s(iso_str);

    if (!(s >> y >> dash >> m >> dash >> d >> t >> hh >> colon >> mm >> colon >>
          ss)) {
      throw ChronoError("Invalid ISO 8601 format");
    }

    uint64_t us_part = 0;
    if (s.peek() == '.') {
      char dot;
      std::string frac_str;
      s >> dot;
      while (std::isdigit(s.peek()))
        frac_str += static_cast<char>(s.get());
      while (frac_str.length() < 6)
        frac_str += '0';
      if (frac_str.length() > 6)
        frac_str = frac_str.substr(0, 6);
      us_part = std::stoull(frac_str);
    }

    int64_t total_sec = detail::days_from_civil(y, m, d) * 86400LL +
                        hh * 3600LL + mm * 60LL + ss;
    uint64_t total_us =
        static_cast<uint64_t>(total_sec) * CHRONO_US_S + us_part;
    uint64_t epoch_us = EpochSec * CHRONO_US_S;

    if (total_us < epoch_us)
      throw ChronoError(
          "Timestamp underflow: Date is before Epoch (2020-01-01)");

    uint64_t units = 0;
    if constexpr (P == Precision::Y)
      units = y - 2020;
    else if constexpr (P == Precision::HY)
      units = (y - 2020) * 2 + (m < 7 ? 0 : 1);
    else if constexpr (P == Precision::Q)
      units = (y - 2020) * 4 + (m - 1) / 3;
    else if constexpr (P == Precision::MO)
      units = (y - 2020) * 12 + (m - 1);
    else {
      units = (total_us - epoch_us) / UNIT_US;
    }

    if (rand_val == 0xFFFFFFFFFFFFFFFFULL)
      rand_val = detail::get_random_bits(T_SHIFT);

    return from_timestamp(units, rand_val);
  }
  /**
   * @brief parses an ISO 8601 string into a ChronoID.
   * @param iso Date string.
   * @param rand_val Optional specific entropy bits.
   * @return A ChronoID instance.
   */
  static ChronoID from_iso_string(const std::string &iso,
                                  uint64_t rand_val = 0xFFFFFFFFFFFFFFFFULL) {
    return from_iso_cstring(iso.c_str(), rand_val);
  }

  /**
   * @brief Returns a system_clock time_point decoded from the ID.
   */
  std::chrono::system_clock::time_point get_time() const {
    uint64_t units = get_timestamp();
    if constexpr (P == Precision::Y || P == Precision::HY ||
                  P == Precision::Q || P == Precision::MO) {
      int y = 2020, m = 1;
      if constexpr (P == Precision::Y)
        y += units;
      else if constexpr (P == Precision::HY) {
        y += units / 2;
        m = (units % 2 == 0 ? 1 : 7);
      } else if constexpr (P == Precision::Q) {
        y += units / 4;
        m = (units % 4) * 3 + 1;
      } else if constexpr (P == Precision::MO) {
        y += units / 12;
        m = (units % 12) + 1;
      }
      int64_t sec = detail::days_from_civil(y, m, 1) * 86400LL;
      return std::chrono::system_clock::time_point(
          std::chrono::duration_cast<std::chrono::system_clock::duration>(
              std::chrono::seconds(sec)));
    }
    uint64_t total_us = (units * UNIT_US) + (EpochSec * CHRONO_US_S);
    return std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::microseconds(total_us)));
  }

  /**
   * @brief Conversion operator to underlying type.
   */
  operator T() const { return value; }

  /**
   * @brief Extracts the raw timestamp units from the ID.
   */
  uint64_t get_timestamp() const {
    return static_cast<uint64_t>(value >> T_SHIFT) & T_MASK;
  }

  /**
   * @brief Extracts the raw random entropy bits from the ID.
   */
  uint64_t get_random() const {
    return static_cast<uint64_t>(value & ((1ULL << T_SHIFT) - 1));
  }

  /**
   * @brief Formats the ID as an ISO 8601 string.
   */
  std::string to_iso_string() const {
    auto tp = get_time();
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
#ifdef _WIN32
    gmtime_s(&tm_buf, &tt);
#else
    gmtime_r(&tt, &tm_buf);
#endif
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%S");
    uint64_t u_us = unit_us();
    if (u_us == 1) {
      ss << "." << std::setfill('0') << std::setw(6)
         << (std::chrono::duration_cast<std::chrono::microseconds>(
                 tp.time_since_epoch())
                 .count() %
             1000000);
    } else if (u_us == 1000) {
      ss << "." << std::setfill('0') << std::setw(3)
         << (std::chrono::duration_cast<std::chrono::milliseconds>(
                 tp.time_since_epoch())
                 .count() %
             1000);
    }
    ss << "Z";
    return ss.str();
  }

  /**
   * @brief Returns the formatted human-readable representation (e.g.,
   * 032E-6014).
   * @return A hyphenated hex string in uppercase.
   */
  std::string formatted() const {
    std::stringstream ss;
    int hexLen = sizeof(T) * 2;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(hexLen)
       << static_cast<uint64_t>(value);
    std::string hex_str = ss.str();
    std::string result;
    for (size_t i = 0; i < hex_str.length(); i += 4) {
      if (i > 0)
        result += "-";
      result += hex_str.substr(i, 4);
    }
    return result;
  }

  /**
   * @brief Reconstructs an ID from its formatted hyphenated hex string.
   * @param fmt The formatted string (e.g., "032E-6014").
   * @return A ChronoID instance.
   * @throws ChronoError if the format is invalid or length is incorrect.
   */
  static ChronoID from_format(const char *fmt) {
    if (!fmt)
      throw ChronoError("Formatted input is null");
    std::string clean;
    for (const char *p = fmt; *p; ++p) {
      if (*p != '-')
        clean += *p;
    }
    if (clean.length() != sizeof(T) * 2) {
      throw ChronoError("Invalid formatted string length");
    }
    try {
      T val;
      if constexpr (sizeof(T) == 8) {
        val = static_cast<T>(std::stoull(clean, nullptr, 16));
      } else {
        val = static_cast<T>(std::stoul(clean, nullptr, 16));
      }
      return ChronoID(val);
    } catch (...) {
      throw ChronoError("Failed to parse hex value from formatted string");
    }
  }

  /**
   * @brief Reconstructs an ID from its formatted hyphenated hex string.
   * @param fmt The formatted string.
   * @return A ChronoID instance.
   */
  static ChronoID from_format(const std::string &fmt) {
    return from_format(fmt.c_str());
  }

private:
  /**
   * @brief Internal helper to get the microsecond multiplier for the current
   * precision.
   */
  static constexpr uint64_t unit_us() {
    return detail::US_MAP[static_cast<int>(P)];
  }

  /**
   * @brief Performs raw time-to-units calculation via the dispatch table.
   * @param tp The time point to calculate for.
   * @return Elapsed units since EpochSec.
   */
  static uint64_t calculate_units(std::chrono::system_clock::time_point tp) {
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
                      tp.time_since_epoch())
                      .count();
    uint64_t epoch_us = EpochSec * CHRONO_US_S;
    if (us < epoch_us)
      throw ChronoError(
          "Timestamp underflow: Date is before Epoch (2020-01-01)");

    return detail::TS_COMPUTE[static_cast<int>(P)](tp, EpochSec);
  }

  /**
   * @brief Internal packing logic for creating new IDs.
   * @param tp Target time point.
   * @param rand Optional random bits (if 0xFFFFFFFFFFFFFFFF, generates new
   * randomness).
   * @return The bit-packed ID value.
   */
  static T pack(std::chrono::system_clock::time_point tp,
                uint64_t rand = 0xFFFFFFFFFFFFFFFFULL) {
    uint64_t units = calculate_units(tp);
    if (rand == 0xFFFFFFFFFFFFFFFFULL)
      rand = detail::get_random_bits(T_SHIFT);
    return static_cast<T>(((units & T_MASK) << T_SHIFT) |
                          (rand & ((1ULL << T_SHIFT) - 1)));
  }
};

// --- Convenience Type Aliases (Aligned with Diamond Standard) ---

/** @defgroup family64 64-bit ID Family (Epoch 2020)
 *  @{
 */
using UChrono64mo = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::MO,
                             26, 26>; ///< Unsigned 64-bit, Month precision.
using Chrono64mo = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::MO, 25,
                            26>; ///< Signed 64-bit, Month precision.
using UChrono64w = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::W, 26,
                            24>; ///< Unsigned 64-bit, Week precision.
using Chrono64w = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::W, 25,
                           24>; ///< Signed 64-bit, Week precision.
using UChrono64d = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::D, 24,
                            23>; ///< Unsigned 64-bit, Day precision.
using Chrono64d = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::D, 23,
                           23>; ///< Signed 64-bit, Day precision.
using UChrono64h = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::H, 22,
                            21>; ///< Unsigned 64-bit, Hour precision.
using Chrono64h = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::H, 21,
                           21>; ///< Signed 64-bit, Hour precision.
using UChrono64m = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::M, 19,
                            18>; ///< Unsigned 64-bit, Minute precision.
using Chrono64m = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::M, 18,
                           18>; ///< Signed 64-bit, Minute precision.
using UChrono64s = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::S, 16,
                            15>; ///< Unsigned 64-bit, Second precision.
using Chrono64s = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::S, 15,
                           15>; ///< Signed 64-bit, Second precision.
using UChrono64ds =
    ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::DS, 15,
             13>; ///< Unsigned 64-bit, Decisecond precision.
using Chrono64ds = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::DS, 14,
                            13>; ///< Signed 64-bit, Decisecond precision.
using UChrono64cs =
    ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::CS, 12,
             12>; ///< Unsigned 64-bit, Centisecond precision.
using Chrono64cs = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::CS, 11,
                            12>; ///< Signed 64-bit, Centisecond precision.
using UChrono64ms =
    ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::MS, 11,
             10>; ///< Unsigned 64-bit, Millisecond precision.
using Chrono64ms = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::MS, 10,
                            10>; ///< Signed 64-bit, Millisecond precision.
using UChrono64us = ChronoID<uint64_t, detail::EPOCH_2020_SEC, Precision::US, 6,
                             5>; ///< Unsigned 64-bit, Microsecond precision.
using Chrono64us = ChronoID<int64_t, detail::EPOCH_2020_SEC, Precision::US, 5,
                            5>; ///< Signed 64-bit, Microsecond precision.
/** @} */

/** @defgroup family32 32-bit ID Family (Epoch 2020)
 *  @{
 */
using UChrono32y = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::Y, 13,
                            11>; ///< Unsigned 32-bit, Year precision.
using Chrono32y = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::Y, 12,
                           11>; ///< Signed 32-bit, Year precision.
using UChrono32hy = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::HY,
                             12, 11>; ///< Unsigned 32-bit, Half-Year precision.
using Chrono32hy = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::HY, 11,
                            11>; ///< Signed 32-bit, Half-Year precision.
using UChrono32q = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::Q, 11,
                            11>; ///< Unsigned 32-bit, Quarter precision.
using Chrono32q = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::Q, 10,
                           11>; ///< Signed 32-bit, Quarter precision.
using UChrono32mo = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::MO,
                             10, 10>; ///< Unsigned 32-bit, Month precision.
using Chrono32mo = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::MO, 9,
                            10>; ///< Signed 32-bit, Month precision.
using UChrono32w = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::W, 9,
                            9>; ///< Unsigned 32-bit, Week precision.
using Chrono32w = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::W, 8,
                           9>; ///< Signed 32-bit, Week precision.
using UChrono32d = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::D, 8,
                            7>; ///< Unsigned 32-bit, Day precision.
using Chrono32d = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::D, 7,
                           7>; ///< Signed 32-bit, Day precision.
using UChrono32h = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::H, 5,
                            5>; ///< Unsigned 32-bit, Hour precision.
using Chrono32h = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::H, 4,
                           5>; ///< Signed 32-bit, Hour precision.
using UChrono32tm = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::TM, 4,
                             4>; ///< Unsigned 32-bit, 10-Min precision.
using Chrono32tm = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::TM, 3,
                            4>; ///< Signed 32-bit, 10-Min precision.
using UChrono32m = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::M, 2,
                            2>; ///< Unsigned 32-bit, Minute precision.
using Chrono32m = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::M, 1,
                           2>; ///< Signed 32-bit, Minute precision.
using UChrono32bs = ChronoID<uint32_t, detail::EPOCH_2020_SEC, Precision::BS, 1,
                             0>; ///< Unsigned 32-bit, Beacon precision.
using Chrono32bs = ChronoID<int32_t, detail::EPOCH_2020_SEC, Precision::BS, 0,
                            0>; ///< Signed 32-bit, Beacon precision.
/** @} */

} // namespace chrono_id

#endif // CHRONO_ID_HPP
