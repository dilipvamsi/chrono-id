#include "../include/chrono_id.hpp"
#include <cassert>
#include <iostream>

using namespace chrono_id;

void test_32bit_variants() {
  std::cout << "Testing 32-bit variants..." << std::endl;

  // Day precision
  UChrono32d u32; // Now UChrono32d (Epoch 2020, 15 rand bits)
  assert(u32.value > 0);
  assert(u32.to_iso_string().length() > 0);
  std::cout << "  UChrono32 ISO: " << u32.to_iso_string() << std::endl;

  Chrono32d c32;
  assert(c32.value > 0);

  // Test from_time with default rand
  auto now = std::chrono::system_clock::now();
  auto u32_auto = UChrono32d::from_time(now);
  assert(u32_auto.value > 0);
  // Total rand bits = N_BITS (8) + S_BITS (7) = 15
  assert(u32_auto.get_random() < (1ULL << 15));

  // Conversion operator
  uint32_t raw_u32 = u32_auto;
  assert(raw_u32 == u32_auto.value);

  // Test from_time (Epoch 2020)
  auto epoch_2020 =
      std::chrono::system_clock::from_time_t(detail::EPOCH_2020_SEC);
  auto u32_epoch = UChrono32d::from_time(epoch_2020, 0);
  assert(u32_epoch.get_timestamp() == 0);
  assert(u32_epoch.get_random() == 0);

  auto t_decoded = u32_epoch.get_time();
  auto tt = std::chrono::system_clock::to_time_t(t_decoded);
  assert(tt == detail::EPOCH_2020_SEC);
  assert(u32_epoch.to_iso_string() == "2020-01-01T00:00:00Z");
}

void test_week_variants() {
  std::cout << "Testing Week variants..." << std::endl;

  UChrono32w uw;
  assert(uw.value > 0);
  std::cout << "  UChrono32w ISO: " << uw.to_iso_string() << std::endl;
  assert(uw.get_random() < (1ULL << 18)); // 9 + 9 bits

  Chrono32w cw;
  assert(cw.value > 0);
  assert(cw.get_random() < (1ULL << 17)); // 8 + 9 bits
}

void test_64bit_variants() {
  std::cout << "Testing 64-bit variants..." << std::endl;

  UChrono64s u64; // Now UChrono64s
  assert(u64.value > 0);
  assert(u64.to_iso_string().find("Z") != std::string::npos);
  std::cout << "  UChrono64 ISO: " << u64.to_iso_string() << std::endl;

  Chrono64ms c64ms;
  assert(c64ms.value > 0);
  std::cout << "  Chrono64ms ISO: " << c64ms.to_iso_string() << std::endl;
  assert(c64ms.to_iso_string().find(".") != std::string::npos);

  auto now = std::chrono::system_clock::now();
  auto c64_auto = Chrono64ms::from_time(now);
  auto u64_auto = UChrono64s::from_time(now);
  assert(c64_auto.value > 0);
  assert(u64_auto.value > 0);

  // Test from_time (Epoch 2020)
  auto epoch_2020 =
      std::chrono::system_clock::from_time_t(detail::EPOCH_2020_SEC);
  auto u64_epoch = UChrono64s::from_time(epoch_2020, 0);
  assert(u64_epoch.get_timestamp() == 0);
}

void test_persona_mixing() {
  std::cout << "Testing Persona mixing..." << std::endl;

  Persona p(42, 0xABC, 7, 0xDEF, 100);
  auto now = std::chrono::system_clock::now();

  auto id1 = Chrono64s::from_persona(now, 1, 1, p);
  auto id2 = Chrono64s::from_persona(now, 1, 1, p);

  assert(id1.value == id2.value);
  assert(id1.get_timestamp() == Chrono64s::from_time(now).get_timestamp());

  // Verify that different persona produces different ID
  Persona p2(43, 0xABC, 7, 0xDEF, 100);
  auto id3 = Chrono64s::from_persona(now, 1, 1, p2);
  assert(id1.value != id3.value);

  std::cout << "  Persona mixing passed!" << std::endl;
}

template <typename T> void test_edge_cases() {
  try {
    T::from_iso_cstring(nullptr);
    assert(false && "Should have threw exception for nullptr");
  } catch (const ChronoError &) {
  }

  try {
    T::from_iso_cstring("invalid-date");
    assert(false && "Should have threw exception for invalid format");
  } catch (const ChronoError &) {
  }

  auto id_valid = T::from_iso_cstring("2023-01-01T00:00:00Z");
  assert(id_valid.value != 0);

  // Pre-2020 check
  try {
    T::from_iso_cstring("2019-12-31T23:59:59Z");
    assert(false && "Should have threw exception for pre-2020");
  } catch (const ChronoError &e) {
    assert(std::string(e.what()).find("underflow") != std::string::npos);
  }

  // Naive ISO string check (treated as UTC)
  auto id_naive = T::from_iso_cstring("2023-01-01T00:00:00");
  auto id_utc = T::from_iso_cstring("2023-01-01T00:00:00Z");
  assert(id_naive.get_timestamp() == id_utc.get_timestamp());

  // Fixed rand
  auto id_fixed = T::from_iso_cstring("2023-01-01T00:00:00Z", 12345);
  assert(id_fixed.get_random() == 12345);

  // Direct from_timestamp
  auto id_ts = T::from_timestamp(10, 555);
  assert(id_ts.get_timestamp() == 10);
  assert(id_ts.get_random() == 555);
}

void test_factory_methods() {
  std::cout << "Testing factory methods..." << std::endl;
  test_edge_cases<UChrono64s>();
  test_edge_cases<Chrono64ms>();
  test_edge_cases<UChrono32d>();
  test_edge_cases<Chrono32d>();
  test_edge_cases<UChrono32y>();
  std::cout << "  Factory methods passed!" << std::endl;
}

int main() {
  try {
    test_32bit_variants();
    test_week_variants();
    test_64bit_variants();
    test_persona_mixing();
    test_factory_methods();
    std::cout << "ALL C++ TESTS PASSED!" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Test failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
