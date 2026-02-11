#include "../include/chrono_id.hpp"

#include <cassert>
#include <iostream>

using namespace chrono_id;

void test_32bit_variants() {
  std::cout << "Testing 32-bit variants..." << std::endl;

  // Day precision
  UChrono32 u32;
  assert(u32.value > 0);
  assert(u32.get_timestamp() > 0);
  assert(u32.to_iso_string().length() > 0);
  std::cout << "  UChrono32 ISO: " << u32.to_iso_string() << std::endl;

  Chrono32 c32;
  assert(c32.value > 0);

  // Test from_time with default rand
  auto now = std::chrono::system_clock::now();
  auto u32_auto = UChrono32::from_time(now);
  assert(u32_auto.value > 0);
  assert(u32_auto.get_random() <
         (1ULL << 18)); // Chrono32 Day has 14 rand bits originally, wait...
  // Let me check my own chrono_id.hpp layout for UChrono32.
  // using UChrono32 = ChronoID<uint32_t, detail::EPOCH_2000_SEC,
  // 86400000000ULL, 0x3FFFF, 14, 14>; Ah, UChrono32 STILL HAS 14 rand bits.
  // Only UChrono32w has 18.
  assert(u32_auto.get_random() < (1ULL << 14));

  // Conversion operator
  uint32_t raw_u32 = u32_auto;
  assert(raw_u32 == u32_auto.value);

  // Test from_time (Jan 1, 2000)
  auto epoch_32 = std::chrono::system_clock::from_time_t(946684800);
  auto u32_epoch = UChrono32::from_time(epoch_32, 0);
  assert((static_cast<uint32_t>(u32_epoch.value) >> 14) == 0);
  assert(u32_epoch.get_timestamp() == 0);
  assert(u32_epoch.get_random() == 0);

  auto t_decoded = u32_epoch.get_time();
  auto tt = std::chrono::system_clock::to_time_t(t_decoded);
  assert(tt == 946684800);
  assert(u32_epoch.to_iso_string() == "2000-01-01T00:00:00Z");
}

void test_week_variants() {
  std::cout << "Testing Week variants..." << std::endl;

  UChrono32w uw;
  assert(uw.value > 0);
  std::cout << "  UChrono32w ISO: " << uw.to_iso_string() << std::endl;
  assert(uw.get_random() < (1ULL << 18));

  Chrono32w cw;
  assert(cw.value > 0);
  assert(cw.get_random() < (1ULL << 17));

  // Test Epoch 2000 for week
  auto epoch_32 = std::chrono::system_clock::from_time_t(946684800);
  auto uw_epoch = UChrono32w::from_time(epoch_32, 0);
  assert(uw_epoch.get_timestamp() == 0);
  assert(uw_epoch.to_iso_string() == "2000-01-01T00:00:00Z");
}

void test_64bit_variants() {
  std::cout << "Testing 64-bit variants..." << std::endl;

  UChrono64 u64;
  assert(u64.value > 0);
  assert(u64.to_iso_string().find("Z") != std::string::npos);
  std::cout << "  UChrono64 ISO: " << u64.to_iso_string() << std::endl;

  Chrono64ms c64ms;
  assert(c64ms.value > 0);
  std::cout << "  Chrono64ms ISO: " << c64ms.to_iso_string() << std::endl;
  assert(c64ms.to_iso_string().find(".") != std::string::npos);

  // Test from_time with default rand for various types
  auto now = std::chrono::system_clock::now();
  auto c64_auto = Chrono64ms::from_time(now);
  auto u64_auto = UChrono64::from_time(now);
  assert(c64_auto.value > 0);
  assert(u64_auto.value > 0);

  // Conversion operator
  int64_t raw_c64 = c64_auto;
  assert(raw_c64 == c64_auto.value);

  // Test from_time (Unix Epoch 1970)
  auto epoch_64 = std::chrono::system_clock::from_time_t(0);
  auto u64_epoch = UChrono64::from_time(epoch_64, 0);
  assert(u64_epoch.get_timestamp() == 0);
}

void test_microsecond_variants() {
  std::cout << "Testing microsecond variants..." << std::endl;

  UChrono64us u64us;
  Chrono64us c64us;

  assert(u64us.value > 0);
  assert(c64us.value > 0);

  std::cout << "  UChrono64us ISO: " << u64us.to_iso_string() << std::endl;
  assert(u64us.to_iso_string().length() > 20); // Should have 6 decimal places

  auto t1 = std::chrono::system_clock::now();

  auto id_u = UChrono64us::from_time(t1);
  auto id_c = Chrono64us::from_time(t1);

  auto t_u_decoded = id_u.get_time();
  auto t_c_decoded = id_c.get_time();

  auto diff_u =
      std::chrono::duration_cast<std::chrono::microseconds>(t_u_decoded - t1)
          .count();
  auto diff_c =
      std::chrono::duration_cast<std::chrono::microseconds>(t_c_decoded - t1)
          .count();

  assert(std::abs(diff_u) <= 1000);
  assert(std::abs(diff_c) <= 1000);
}

template <typename T> void test_edge_cases() {
  // 1. from_iso_cstring(nullptr)
  try {
    T::from_iso_cstring(nullptr);
    assert(false && "Should have threw exception for nullptr");
  } catch (const ChronoError &) {
  }

  // 2. Invalid ISO string
  try {
    T::from_iso_cstring("invalid-date");
    assert(false && "Should have threw exception for invalid format");
  } catch (const ChronoError &) {
  }

  // 3. Valid ISO string
  auto id_valid = T::from_iso_cstring("2023-01-01T00:00:00Z");
  assert(id_valid.value != 0);

  // Pre-1970 check
  try {
    T::from_iso_cstring("1969-12-31T23:59:59Z");
    assert(false && "Should have threw exception for pre-1970");
  } catch (const ChronoError &e) {
    assert(std::string(e.what()).find("Date is before Unix Epoch") !=
           std::string::npos);
  }

  // Pre-2000 Epoch boundary check for 32-bit types
  if constexpr (T::Epoch > 0) {
    try {
      T::from_iso_cstring("1999-12-31T23:59:59Z");
      assert(false && "Should have threw underflow exception for pre-2000");
    } catch (const ChronoError &e) {
      std::string msg = e.what();
      assert(msg.find("Date is before Epoch") != std::string::npos);
      assert(msg.find("2000-01-01") != std::string::npos);
    }
    try {
      T::from_iso_cstring("1990-01-01T00:00:00Z");
      assert(false && "Should have threw exception");
    } catch (const ChronoError &e) {
      std::string msg = e.what();
      assert(msg.find("underflow") != std::string::npos);
      assert(msg.find("2000-01-01") != std::string::npos);
    }
  }

  // Fractional seconds logic
  auto id_frac = T::from_iso_cstring("2023-01-01T00:00:00.123456Z");
  assert(id_frac.value != 0);
  auto id_short = T::from_iso_cstring("2023-01-01T00:00:00.1Z");
  assert(id_short.value != 0);
  auto id_long = T::from_iso_cstring("2023-01-01T00:00:00.123456789Z");
  assert(id_long.value != 0);

  // Extremely old date
  try {
    T::from_iso_cstring("1960-01-01T00:00:00Z");
    assert(false && "Should have threw exception for 1960");
  } catch (const ChronoError &) {
  }

  // Direct from_timestamp
  auto id_ts = T::from_timestamp(1000);
  assert(id_ts.get_timestamp() == 1000);
}

void test_factory_methods() {
  std::cout << "Testing factory methods & coverage..." << std::endl;

  test_edge_cases<UChrono32>();
  test_edge_cases<Chrono32>();
  test_edge_cases<UChrono32h>();
  test_edge_cases<Chrono32h>();
  test_edge_cases<UChrono32m>();
  test_edge_cases<Chrono32m>();
  test_edge_cases<UChrono32w>();
  test_edge_cases<Chrono32w>();
  test_edge_cases<UChrono64>();
  test_edge_cases<Chrono64>();
  test_edge_cases<UChrono64ms>();
  test_edge_cases<Chrono64ms>();
  test_edge_cases<UChrono64us>();
  test_edge_cases<Chrono64us>();

  // from_iso_string wrappers
  std::string s_iso = "2023-05-20T10:30:00.123Z";
  auto id1 = UChrono64ms::from_iso_string(s_iso);
  assert(id1.to_iso_string() == s_iso);
  auto id2 = UChrono64ms::from_iso_string(s_iso, 123);
  assert(id2.get_random() == 123);

  auto id3 = UChrono64ms::from_iso_cstring(s_iso.c_str());
  assert(id3.to_iso_string() == s_iso);

  std::cout << "  Factory methods passed!" << std::endl;
}

void test_detail_utils() {
  std::cout << "Testing detail utilities..." << std::endl;
  assert(chrono_id::detail::days_from_civil(2023, 5, 20) > 0);
  assert(chrono_id::detail::days_from_civil(-400, 1, 1) < 0);
  assert(chrono_id::detail::days_from_civil(0, 1, 1) < 0);
  assert(chrono_id::detail::days_from_civil(0, 3, 1) < 0);
  static_cast<void>(chrono_id::detail::get_random_bits(8));
  std::cout << "  Detail utilities passed!" << std::endl;
}

int main() {
  try {
    test_32bit_variants();
    test_week_variants();
    test_64bit_variants();
    test_microsecond_variants();
    test_factory_methods();
    test_detail_utils();
    std::cout << "ALL TESTS PASSED!" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Test failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
