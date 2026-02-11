#ifndef CHRONO_ID_HPP
#define CHRONO_ID_HPP

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

namespace chrono_id {

/**
 * Custom exception for Chrono-ID library.
 */
class ChronoError : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
};

/**
 * Internal utilities for Chrono-ID generation and parsing.
 * These are not intended for direct use by consumers.
 */
namespace detail {
/**
 * Generates a specified number of cryptographically secure-ish random bits.
 * Uses std::random_device for seeding and std::mt19937_64 for fast generation.
 * @param bits Number of bits of entropy to return.
 * @return A 64-bit unsigned integer containing the random bits.
 */
inline uint64_t get_random_bits(uint8_t bits) {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(0, (1ULL << bits) - 1);
  return dis(gen);
}

/**
 * Converts a civil date (Y, M, D) to days since the Unix Epoch.
 * This is a highly optimized algorithm for leap year handling.
 * @return Total days since 1970-01-01.
 */
inline int64_t days_from_civil(int y, int m, int d) {
  y -= m <= 2;
  const int64_t era = (y >= 0 ? y : y - 399) / 400;
  const uint32_t yoe = static_cast<uint32_t>(y - era * 400);
  const uint32_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const uint32_t ioe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + static_cast<int64_t>(ioe) - 719468;
}

/// Unix timestamp for Jan 1st 2000. Used as default epoch for 32-bit IDs.
constexpr uint64_t EPOCH_2000_SEC = 946684800;
} // namespace detail

/**
 * Unified Chrono-ID Template.
 *
 * A Chrono-ID is a K-sortable identifier that combines a timestamp with random
 * entropy.
 *
 * @tparam T The underlying integer type (uint32_t, int64_t, etc.).
 * @tparam EpochSec The custom epoch in seconds since 1970-01-01.
 * @tparam UnitUs The logical precision unit in microseconds (1=us, 1000=ms,
 * 1000000=sec).
 * @tparam TimeMask Bitmask for the time component.
 * @tparam Shift Number of bits to shift the time component to the left.
 * @tparam RandBits Number of bits allocated for randomness/entropy.
 */
template <typename T, uint64_t EpochSec, uint64_t UnitUs, uint64_t TimeMask,
          uint8_t Shift, uint8_t RandBits>
struct ChronoID {
  static constexpr uint64_t Epoch = EpochSec;
  static constexpr uint64_t Unit = UnitUs;
  /// The packed numeric ID value.
  T value;

  /**
   * Default constructor. Generates a new unique ID for the current time.
   */
  ChronoID() {
    auto now = std::chrono::system_clock::now();
    value = pack(now);
  }

  /**
   * Constructs an ID from an existing raw numeric value.
   * @param val The raw ID value.
   */
  explicit ChronoID(T val) : value(val) {}

  /**
   * Static factory to create an ID from a specific time point.
   * @param tp The time point to encode.
   * @param rand_val Optional fixed random bits; defaults to auto-generation.
   * @return A new ChronoID instance.
   */
  static ChronoID from_time(std::chrono::system_clock::time_point tp,
                            uint64_t rand_val = 0xFFFFFFFFFFFFFFFFULL) {
    return ChronoID(pack(tp, rand_val));
  }

  /**
   * Factory to create an ID from a raw timestamp (units since epoch).
   * Generates new random bits automatically.
   */
  static ChronoID from_timestamp(uint64_t ts) {
    return from_timestamp(ts, detail::get_random_bits(RandBits));
  }

  /**
   * Factory to create an ID from a raw timestamp and fixed random bits.
   */
  static ChronoID from_timestamp(uint64_t ts, uint64_t rand_val) {
    return ChronoID(((ts & TimeMask) << Shift) |
                    (rand_val & ((1ULL << RandBits) - 1)));
  }

  /**
   * Parses an ISO 8601 string from a C-string.
   * Optimized to avoid std::string allocations.
   */
  static ChronoID from_iso_cstring(const char *iso, uint64_t rand_val) {
    if (!iso) {
      throw ChronoError("Input string is null");
    }
    int y, m, d, hh, mm, ss;
    char dash, t, colon;
    std::stringstream s(iso);

    // Parse the fixed-length date and time components
    if (!(s >> y >> dash >> m >> dash >> d >> t >> hh >> colon >> mm >> colon >>
          ss)) {
      throw ChronoError("Invalid ISO 8601 format");
    }

    // Extract sub-second (microseconds) if present
    uint64_t us_part = 0;
    if (s.peek() == '.') {
      char dot;
      std::string frac_str;
      s >> dot;
      while (std::isdigit(s.peek())) {
        frac_str += static_cast<char>(s.get());
      }
      // Normalize to 6 digits (microseconds)
      while (frac_str.length() < 6)
        frac_str += '0';
      if (frac_str.length() > 6)
        frac_str = frac_str.substr(0, 6);
      us_part = std::stoull(frac_str);
    }

    // Convert parsed components to internal microsecond unit
    int64_t total_sec = detail::days_from_civil(y, m, d) * 86400LL +
                        hh * 3600LL + mm * 60LL + ss;

    if (total_sec < 0) {
      throw ChronoError(
          "Timestamp underflow: Date is before Unix Epoch (1970-01-01)");
    }

    uint64_t total_us = static_cast<uint64_t>(total_sec) * 1000000ULL + us_part;
    uint64_t epoch_us = EpochSec * 1000000ULL;

    // Strict Epoch Check: Reject dates before the configured epoch
    if (total_us < epoch_us) {
      if constexpr (EpochSec > 0) {
        throw ChronoError(
            "Timestamp underflow: Date is before Epoch (32-bit types require "
            "2000-01-01 or later)");
      } else {
        throw ChronoError(
            "Timestamp underflow: Date is before Unix Epoch (1970-01-01)");
      }
    }

    uint64_t units = (total_us - epoch_us) / UnitUs;

    return from_timestamp(units, rand_val);
  }

  /**
   * Overload for from_iso_cstring without explicit rand_val.
   * Auto-generates random bits.
   */
  static ChronoID from_iso_cstring(const char *iso) {
    return from_iso_cstring(iso, detail::get_random_bits(RandBits));
  }

  /**
   * Parses an ISO 8601 string (std::string wrapper).
   */
  static ChronoID from_iso_string(const std::string &iso, uint64_t rand_val) {
    return from_iso_cstring(iso.c_str(), rand_val);
  }

  static ChronoID from_iso_string(const std::string &iso) {
    return from_iso_string(iso, detail::get_random_bits(RandBits));
  }

  /**
   * Extracts the encoded time as a chrono time_point.
   */
  std::chrono::system_clock::time_point get_time() const {
    uint64_t units = get_timestamp();
    uint64_t total_us = (units * UnitUs) + (EpochSec * 1000000ULL);
    return std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::microseconds(total_us)));
  }

  /// Implicit conversion to the underlying integer type.
  operator T() const { return value; }

  /**
   * Returns the raw numeric timestamp (number of units since custom epoch).
   */
  uint64_t get_timestamp() const {
    return static_cast<uint64_t>(value >> Shift);
  }

  /**
   * Returns the raw numeric random entropy bits.
   */
  uint64_t get_random() const {
    return static_cast<uint64_t>(value & ((1ULL << RandBits) - 1));
  }

  /**
   * Formats the ID as an ISO 8601 UTC string.
   * Precision is automatically determined based on the ID's internal unit.
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

    // Handling fractional seconds based on UnitUs template parameter
    if (UnitUs == 1) { // Microsecond precision
      ss << "." << std::setfill('0') << std::setw(6)
         << (get_timestamp() % 1000000);
    } else if (UnitUs == 1000) { // Millisecond precision
      ss << "." << std::setfill('0') << std::setw(3)
         << (get_timestamp() % 1000);
    }

    ss << "Z";
    return ss.str();
  }

private:
  /**
   * Internal packer logic. Combines time and random bits into the final layout.
   */
  static T pack(std::chrono::system_clock::time_point tp,
                uint64_t rand = 0xFFFFFFFFFFFFFFFFULL) {
    auto duration = tp.time_since_epoch();
    uint64_t us =
        std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    uint64_t epoch_us = EpochSec * 1000000ULL;

    uint64_t units = (us - epoch_us) / UnitUs;
    if (rand == 0xFFFFFFFFFFFFFFFFULL) {
      rand = detail::get_random_bits(RandBits);
    }

    // Mask both time and random bits to ensure they fit in their allocated
    // slots
    return static_cast<T>(((static_cast<uint64_t>(units) & TimeMask) << Shift) |
                          (rand & ((1ULL << RandBits) - 1)));
  }
};

// --- Convenience Type Aliases ---

// --- 32-bit Family (Epoch 2000) ---

/// 32-bit Day Precision (Longevity ~716 years)
/// Unsigned: 18b Time, 14b Rand, Shift 14
using UChrono32 =
    ChronoID<uint32_t, detail::EPOCH_2000_SEC, 86400000000ULL, 0x3FFFF, 14, 14>;
/// Signed: 18b Time, 13b Rand, Shift 13
using Chrono32 =
    ChronoID<int32_t, detail::EPOCH_2000_SEC, 86400000000ULL, 0x3FFFF, 13, 13>;

/// 32-bit Hour Precision (Longevity ~239 years)
/// Unsigned: 21b Time, 11b Rand, Shift 11
using UChrono32h =
    ChronoID<uint32_t, detail::EPOCH_2000_SEC, 3600000000ULL, 0x1FFFFF, 11, 11>;
/// Signed: 21b Time, 10b Rand, Shift 10
using Chrono32h =
    ChronoID<int32_t, detail::EPOCH_2000_SEC, 3600000000ULL, 0x1FFFFF, 10, 10>;

/// 32-bit Minute Precision (Longevity ~255 years)
/// Unsigned: 27b Time, 5b Rand, Shift 5
using UChrono32m =
    ChronoID<uint32_t, detail::EPOCH_2000_SEC, 60000000ULL, 0x7FFFFFF, 5, 5>;
/// Signed: 27b Time, 4b Rand, Shift 4
using Chrono32m =
    ChronoID<int32_t, detail::EPOCH_2000_SEC, 60000000ULL, 0x7FFFFFF, 4, 4>;

/// 32-bit Week Precision (Longevity ~314 years)
/// Unsigned: 14b Week, 18b Rand, Shift 18
using UChrono32w =
    ChronoID<uint32_t, detail::EPOCH_2000_SEC, 604800000000ULL, 0x3FFF, 18, 18>;
/// Signed: 14b Week, 17b Rand, Shift 17
using Chrono32w =
    ChronoID<int32_t, detail::EPOCH_2000_SEC, 604800000000ULL, 0x3FFF, 17, 17>;

// --- 64-bit Family (Epoch 1970) ---

/// 64-bit Second Precision (Longevity ~2177 years)
/// Unsigned: 36b Time, 28b Rand, Shift 28
using UChrono64 = ChronoID<uint64_t, 0, 1000000ULL, 0xFFFFFFFFFULL, 28, 28>;
/// Signed: 36b Time, 27b Rand, Shift 27
using Chrono64 = ChronoID<int64_t, 0, 1000000ULL, 0xFFFFFFFFFULL, 27, 27>;

/// 64-bit Millisecond Precision (Longevity ~557 years)
/// Unsigned: 44b Time, 20b Rand, Shift 20
using UChrono64ms = ChronoID<uint64_t, 0, 1000ULL, 0xFFFFFFFFFFFULL, 20, 20>;
/// Signed: 44b Time, 19b Rand, Shift 19
using Chrono64ms = ChronoID<int64_t, 0, 1000ULL, 0xFFFFFFFFFFFULL, 19, 19>;

/// 64-bit Microsecond Precision (Longevity ~285 years)
/// Unsigned: 54b Time, 10b Rand, Shift 10
using UChrono64us = ChronoID<uint64_t, 0, 1ULL, 0x3FFFFFFFFFFFFFULL, 10, 10>;
/// Signed: 54b Time, 9b Rand, Shift 9
using Chrono64us = ChronoID<int64_t, 0, 1ULL, 0x3FFFFFFFFFFFFFULL, 9, 9>;

} // namespace chrono_id

#endif // CHRONO_ID_HPP
