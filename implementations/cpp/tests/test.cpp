#include "../include/chrono_id.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

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
  assert(u32_auto.get_random() < (1ULL << 14));

  // Conversion operator
  uint32_t raw_u32 = u32_auto;
  assert(raw_u32 == u32_auto.value);

  // Test from_time (Jan 1, 2000)
  auto epoch_32 = std::chrono::system_clock::from_time_t(946684800);
  auto u32_epoch = UChrono32::from_time(epoch_32, 0);
  assert((u32_epoch.value >> 14) == 0);
  assert(u32_epoch.get_timestamp() == 0);
  assert(u32_epoch.get_random() == 0);

  auto t_decoded = u32_epoch.get_time();
  auto tt = std::chrono::system_clock::to_time_t(t_decoded);
  assert(tt == 946684800);
  assert(u32_epoch.to_iso_string() == "2000-01-01T00:00:00Z");
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
  auto u64_auto =
      UChrono64::from_time(now); // This was missing default coverage
  assert(c64_auto.value > 0);
  assert(u64_auto.value > 0);

  // Conversion operator
  int64_t raw_c64 = c64_auto;
  assert(raw_c64 == c64_auto.value);

  // Test from_time (Unix Epoch 1970)
  auto epoch_64 = std::chrono::system_clock::from_time_t(0);
  auto u64_epoch = UChrono64::from_time(epoch_64, 0);
  assert((u64_epoch.value >> 28) == 0);
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

  // Test from_time for both
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

  assert(std::abs(diff_u) <=
         1000); // Date precision can vary slightly, but should be close
  assert(std::abs(diff_c) <= 1000);

  // Conversion operators
  uint64_t raw_u = id_u;
  int64_t raw_c = id_c;
  assert(raw_u == id_u.value);
  assert(raw_c == id_c.value);

  // Test specific from_time with rand
  auto id_u_fixed = UChrono64us::from_time(t1, 123);
  assert((id_u_fixed.value & 0x3FF) == 123);
  assert(id_u_fixed.get_random() == 123);
}

template <typename T> void test_edge_cases() {
  // 1. from_iso_cstring(nullptr)
  try {
    T::from_iso_cstring(nullptr);
    assert(false && "Should have threw exception");
  } catch (const std::invalid_argument &) {
    // Expected
  }

  // 2. Invalid ISO string
  try {
    T::from_iso_cstring("invalid-date");
    assert(false && "Should have threw exception");
  } catch (const std::invalid_argument &) {
    // Expected
  }

  // 3. Valid ISO string (to hit from_timestamp via from_iso_cstring)
  // Use a safe date that fits all types (Epoch 2000 or 1970)
  auto id_valid = T::from_iso_cstring("2023-01-01T00:00:00Z");
  assert(id_valid.value > 0);

  // 3b. Valid ISO string with fractional seconds (triggers parsing logic)
  auto id_frac = T::from_iso_cstring("2023-01-01T00:00:00.123456Z");
  assert(id_frac.value > 0);

  // 3d. Short fractional seconds (padding)
  auto id_short = T::from_iso_cstring("2023-01-01T00:00:00.1Z");
  assert(id_short.value > 0);

  // 3e. Long fractional seconds (truncation)
  auto id_long = T::from_iso_cstring("2023-01-01T00:00:00.123456789Z");
  assert(id_long.value > 0);

  // 3c. Pre-ASCII-Epoch date (triggers underflow for 1970 epoch types)
  try {
    T::from_iso_cstring("1960-01-01T00:00:00Z");
    if constexpr (T::Epoch > 0) {
      // Only if epoch > 0 do we expect throw for really old dates if they
      // underflow epoch But wait, 1960 is negative total_sec, so it throws
      // "Invalid date" for ALL types now
    }
    assert(false && "Should have threw exception");
  } catch (const std::invalid_argument &) {
    // Expected
  }

  // 3f. Date between 1970 and Epoch (triggers underflow for custom epoch types)
  if constexpr (T::Epoch > 0) {
    try {
      // 1990 is > 1970 but < 2000 (if Epoch is 2000)
      T::from_iso_cstring("1990-01-01T00:00:00Z");
      assert(false && "Should have threw exception");
    } catch (const std::invalid_argument &e) {
      // Expected: "Timestamp underflow: Date is before Epoch"
      std::string msg = e.what();
      assert(msg.find("underflow") != std::string::npos);
    }
  }

  // 4. Direct from_timestamp call
  auto id_ts = T::from_timestamp(1000); // just some scalar
  assert(id_ts.get_timestamp() == 1000);
}

void test_factory_methods() {
  std::cout << "Testing factory methods..." << std::endl;

  // from_timestamp
  uint64_t ts = 1684578600000ULL; // 2023-05-20T10:30:00.000Z in ms
  auto id1 = UChrono64ms::from_timestamp(ts);
  assert(id1.get_timestamp() == ts);

  auto id2 = UChrono64ms::from_timestamp(ts, 42);
  assert(id2.get_timestamp() == ts);
  assert(id2.get_random() == 42);

  // from_iso_string -> from_iso_cstring
  std::string iso = "2023-05-20T10:30:00.123000Z";
  // For std::string, we can use from_iso_string overload or c_str()
  auto id3 = UChrono64ms::from_iso_cstring("2023-05-20T10:30:00.123Z");
  assert(id3.to_iso_string() == "2023-05-20T10:30:00.123Z");

  auto id4 = UChrono64ms::from_iso_cstring("2023-05-20T10:30:00.123Z", 99);
  assert(id4.to_iso_string() == "2023-05-20T10:30:00.123Z");
  assert(id4.get_random() == 99);

  // from_iso_cstring
  auto id4b = UChrono64ms::from_iso_cstring("2023-05-20T10:30:00.123Z", 99);
  assert(id4b.to_iso_string() == "2023-05-20T10:30:00.123Z");
  assert(id4b.get_random() == 99);

  // microsecond
  std::string iso_us = "2023-05-20T10:30:00.123456Z";
  auto id5 =
      UChrono64us::from_iso_cstring(iso_us.c_str(), 77); // std::string overload
  assert(id5.to_iso_string() == iso_us);
  assert(id5.get_random() == 77);

  // Test padding of fractional seconds (need to cover line 146 in Us variant)
  auto id_padded_us =
      UChrono64us::from_iso_cstring("2023-05-20T10:30:00.1Z", 77);
  assert(id_padded_us.to_iso_string() == "2023-05-20T10:30:00.100000Z");

  // Test truncation of fractional seconds for both variants
  auto id6 =
      UChrono64us::from_iso_cstring("2023-05-20T10:30:00.123456789Z", 77);
  assert(id6.to_iso_string() == iso_us); // Should truncate to 6 digits

  auto id7 =
      UChrono64ms::from_iso_cstring("2023-05-20T10:30:00.123456789Z", 99);
  assert(id7.to_iso_string() == "2023-05-20T10:30:00.123Z");

  // --- Coverage Tests for Overloads ---
  // Run edge cases for ALL used types to ensure template coverage
  test_edge_cases<UChrono32>();
  test_edge_cases<Chrono32>();
  test_edge_cases<UChrono64>();
  test_edge_cases<Chrono64>();
  test_edge_cases<UChrono64ms>();
  test_edge_cases<Chrono64ms>();
  test_edge_cases<UChrono64us>();
  test_edge_cases<Chrono64us>();

  // 5. Underflow check
  // UChrono32 has Epoch 2000. 1999 should underflow.
  try {
    UChrono32::from_iso_cstring("1999-12-31T23:59:59Z");
    assert(false && "Should have threw exception");
  } catch (const std::invalid_argument &) {
    // Expected
  }

  // 2. from_iso_cstring(const char*) - no explicit rand
  auto id_cstr_auto = UChrono64ms::from_iso_cstring("2023-05-20T10:30:00.123Z");
  assert(id_cstr_auto.to_iso_string() == "2023-05-20T10:30:00.123Z");
  // Random part should just be valid (non-zero probability, but we can't assert
  // specific value)

  // 3. from_iso_string(string, rand)
  std::string s_iso = "2023-05-20T10:30:00.123Z";
  auto id_str_rand = UChrono64ms::from_iso_string(s_iso, 55);
  assert(id_str_rand.to_iso_string() == s_iso);
  assert(id_str_rand.get_random() == 55);

  // 4. from_iso_string(string) - no explicit rand
  auto id_str_auto = UChrono64ms::from_iso_string(s_iso);
  assert(id_str_auto.to_iso_string() == s_iso);

  std::cout << "  Factory methods passed!" << std::endl;
}

void test_detail_utils() {
  std::cout << "Testing detail utilities..." << std::endl;

  // Coverage for both y >= 0 and y < 0 paths
  assert(chrono_id::detail::days_from_civil(2023, 5, 20) > 0);
  assert(chrono_id::detail::days_from_civil(-400, 1, 1) < 0);
  assert(chrono_id::detail::days_from_civil(0, 1, 1) < 0);
  assert(chrono_id::detail::days_from_civil(0, 3, 1) < 0); // Just hit the path

  std::cout << "  Detail utilities passed!" << std::endl;
}

void test_ksortability() {
  std::cout << "Testing K-Sortability..." << std::endl;

  std::vector<uint64_t> ids;
  for (int i = 0; i < 100; ++i) {
    ids.push_back(UChrono64ms().value);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  assert(std::is_sorted(ids.begin(), ids.end()));
}

void test_randomness() {
  std::cout << "Testing Randomness..." << std::endl;

  auto now = std::chrono::system_clock::now();
  uint64_t id1 = UChrono64ms::from_time(now).value;
  uint64_t id2 = UChrono64ms::from_time(now).value;

  assert(id1 != id2);
  assert((id1 >> 20) == (id2 >> 20)); // Time bits should be same
}

int main() {
  try {
    test_32bit_variants();
    test_64bit_variants();
    test_microsecond_variants();
    test_factory_methods();
    test_detail_utils();
    test_ksortability();
    test_randomness();
    std::cout << "ALL TESTS PASSED!" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Test failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
