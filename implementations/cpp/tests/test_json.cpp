#include "../include/chrono_id.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace chrono_id;

int main() {
  std::cout << "Running Cross-Platform C++ JSON Tests (Diamond Standard)..."
            << std::endl;

  // Exhaustive Multiplier Test (All 128 Indices Exercise)
  {
    std::cout << "  Testing chrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(1, 1111U, 51, 802U, 10U);
    auto tmp = Chrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 292ULL, 73ULL, p);
    assert(obj.value == 0x134f1efULL);
    assert(obj.formatted() == "0134-F1EF");
  }
  {
    std::cout << "  Testing chrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(2, 1112U, 52, 803U, 11U);
    auto tmp = Chrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 293ULL, 10ULL, p);
    assert(obj.value == 0xe7a56aULL);
    assert(obj.formatted() == "00E7-A56A");
  }
  {
    std::cout << "  Testing chrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(3, 1113U, 53, 804U, 1932U);
    auto tmp = Chrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 294ULL, 459ULL, p);
    assert(obj.value == 0x1a75bd1ULL);
    assert(obj.formatted() == "01A7-5BD1");
  }
  {
    std::cout << "  Testing chrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(4, 1114U, 54, 805U, 1U);
    auto tmp = Chrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 295ULL, 0ULL, p);
    assert(obj.value == 0xd92ab6ULL);
    assert(obj.formatted() == "00D9-2AB6");
  }
  {
    std::cout << "  Testing chrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(5, 1115U, 55, 806U, 910U);
    auto tmp = Chrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 296ULL, 461ULL, p);
    assert(obj.value == 0x141cd45ULL);
    assert(obj.formatted() == "0141-CD45");
  }
  {
    std::cout << "  Testing chrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(6, 1116U, 56, 807U, 1935U);
    auto tmp = Chrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 297ULL, 462ULL, p);
    assert(obj.value == 0x1ac6bb0ULL);
    assert(obj.formatted() == "01AC-6BB0");
  }
  {
    std::cout << "  Testing chrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(7, 1117U, 57, 808U, 0U);
    auto tmp = Chrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 298ULL, 15ULL, p);
    assert(obj.value == 0x15b77f5ULL);
    assert(obj.formatted() == "015B-77F5");
  }
  {
    std::cout << "  Testing chrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(8, 1118U, 58, 809U, 401U);
    auto tmp = Chrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32w::from_persona_units(tmp.get_timestamp(), 299ULL, 464ULL, p);
    assert(obj.value == 0x160f680ULL);
    assert(obj.formatted() == "0160-F680");
  }
  {
    std::cout << "  Testing chrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(9, 1119U, 59, 810U, 1938U);
    auto tmp = Chrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 300ULL, 465ULL, p);
    assert(obj.value == 0x1c69c53ULL);
    assert(obj.formatted() == "01C6-9C53");
  }
  {
    std::cout << "  Testing chrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(10, 1120U, 60, 811U, 1939U);
    auto tmp = Chrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64cs::from_persona_units(tmp.get_timestamp(), 301ULL, 466ULL, p);
    assert(obj.value == 0x13e1d87d62d1926ULL);
    assert(obj.formatted() == "013E-1D87-D62D-1926");
  }
  {
    std::cout << "  Testing chrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(11, 1121U, 61, 812U, 1940U);
    auto tmp = Chrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64d::from_persona_units(tmp.get_timestamp(), 302ULL, 467ULL, p);
    assert(obj.value == 0x134ea78d7abdc1bULL);
    assert(obj.formatted() == "0134-EA78-D7AB-DC1B");
  }
  {
    std::cout << "  Testing chrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(12, 1122U, 62, 813U, 1941U);
    auto tmp = Chrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ds::from_persona_units(tmp.get_timestamp(), 303ULL, 468ULL, p);
    assert(obj.value == 0x1fcfc0c8a20fb66ULL);
    assert(obj.formatted() == "01FC-FC0C-8A20-FB66");
  }
  {
    std::cout << "  Testing chrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(13, 1123U, 63, 814U, 1942U);
    auto tmp = Chrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64h::from_persona_units(tmp.get_timestamp(), 304ULL, 469ULL, p);
    assert(obj.value == 0x1cf4b6a32670b99ULL);
    assert(obj.formatted() == "01CF-4B6A-3267-0B99");
  }
  {
    std::cout << "  Testing chrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(14, 1124U, 64, 815U, 1943U);
    auto tmp = Chrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64m::from_persona_units(tmp.get_timestamp(), 305ULL, 470ULL, p);
    assert(obj.value == 0x1b25563077499ceULL);
    assert(obj.formatted() == "01B2-5563-0774-99CE");
  }
  {
    std::cout << "  Testing chrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(15, 1125U, 65, 816U, 1944U);
    auto tmp = Chrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64mo::from_persona_units(tmp.get_timestamp(), 306ULL, 471ULL, p);
    assert(obj.value == 0x142de904ee77837ULL);
    assert(obj.formatted() == "0142-DE90-4EE7-7837");
  }
  {
    std::cout << "  Testing chrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(16, 1126U, 66, 817U, 921U);
    auto tmp = Chrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ms::from_persona_units(tmp.get_timestamp(), 307ULL, 472ULL, p);
    assert(obj.value == 0x18da4e9cbb59c12ULL);
    assert(obj.formatted() == "018D-A4E9-CBB5-9C12");
  }
  {
    std::cout << "  Testing chrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(17, 1127U, 67, 818U, 1946U);
    auto tmp = Chrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 308ULL, 473ULL, p);
    assert(obj.value == 0x197300a08f5e977ULL);
    assert(obj.formatted() == "0197-300A-08F5-E977");
  }
  {
    std::cout << "  Testing chrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(18, 1128U, 68, 819U, 27U);
    auto tmp = Chrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64us::from_persona_units(tmp.get_timestamp(), 309ULL, 26ULL, p);
    assert(obj.value == 0x184530c50f102feULL);
    assert(obj.formatted() == "0184-530C-50F1-02FE");
  }
  {
    std::cout << "  Testing chrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(19, 1129U, 69, 820U, 1948U);
    auto tmp = Chrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64w::from_persona_units(tmp.get_timestamp(), 310ULL, 475ULL, p);
    assert(obj.value == 0x1609ded539e7e61ULL);
    assert(obj.formatted() == "0160-9DED-539E-7E61");
  }
  {
    std::cout << "  Testing uchrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(21, 1131U, 71, 822U, 30U);
    auto tmp = UChrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32d::from_persona_units(tmp.get_timestamp(), 312ULL, 93ULL, p);
    assert(obj.value == 0x269e9c7ULL);
    assert(obj.formatted() == "0269-E9C7");
  }
  {
    std::cout << "  Testing uchrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(22, 1132U, 72, 823U, 31U);
    auto tmp = UChrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32h::from_persona_units(tmp.get_timestamp(), 313ULL, 30ULL, p);
    assert(obj.value == 0x1cf4a64ULL);
    assert(obj.formatted() == "01CF-4A64");
  }
  {
    std::cout << "  Testing uchrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(23, 1133U, 73, 824U, 1952U);
    auto tmp = UChrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32hy::from_persona_units(tmp.get_timestamp(), 314ULL, 479ULL, p);
    assert(obj.value == 0x319bd9fULL);
    assert(obj.formatted() == "0319-BD9F");
  }
  {
    std::cout << "  Testing uchrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(24, 1134U, 74, 825U, 1U);
    auto tmp = UChrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32m::from_persona_units(tmp.get_timestamp(), 315ULL, 0ULL, p);
    assert(obj.value == 0x1b2556cULL);
    assert(obj.formatted() == "01B2-556C");
  }
  {
    std::cout << "  Testing uchrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(25, 1135U, 75, 826U, 930U);
    auto tmp = UChrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32mo::from_persona_units(tmp.get_timestamp(), 316ULL, 481ULL, p);
    assert(obj.value == 0x28b0c8dULL);
    assert(obj.formatted() == "028B-0C8D");
  }
  {
    std::cout << "  Testing uchrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(26, 1136U, 76, 827U, 1955U);
    auto tmp = UChrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32q::from_persona_units(tmp.get_timestamp(), 317ULL, 482ULL, p);
    assert(obj.value == 0x34c2a56ULL);
    assert(obj.formatted() == "034C-2A56");
  }
  {
    std::cout << "  Testing uchrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(27, 1137U, 77, 828U, 4U);
    auto tmp = UChrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32tm::from_persona_units(tmp.get_timestamp(), 318ULL, 3ULL, p);
    assert(obj.value == 0x2b6ef73ULL);
    assert(obj.formatted() == "02B6-EF73");
  }
  {
    std::cout << "  Testing uchrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(28, 1138U, 78, 829U, 421U);
    auto tmp = UChrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32w::from_persona_units(tmp.get_timestamp(), 319ULL, 484ULL, p);
    assert(obj.value == 0x2c0460eULL);
    assert(obj.formatted() == "02C0-460E");
  }
  {
    std::cout << "  Testing uchrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(29, 1139U, 79, 830U, 1958U);
    auto tmp = UChrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32y::from_persona_units(tmp.get_timestamp(), 320ULL, 485ULL, p);
    assert(obj.value == 0x38d9db7ULL);
    assert(obj.formatted() == "038D-9DB7");
  }
  {
    std::cout << "  Testing uchrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(30, 1140U, 80, 831U, 1959U);
    auto tmp = UChrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64cs::from_persona_units(tmp.get_timestamp(), 321ULL, 486ULL, p);
    assert(obj.value == 0x27c3b0fac1037aeULL);
    assert(obj.formatted() == "027C-3B0F-AC10-37AE");
  }
  {
    std::cout << "  Testing uchrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(31, 1141U, 81, 832U, 1960U);
    auto tmp = UChrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64d::from_persona_units(tmp.get_timestamp(), 322ULL, 487ULL, p);
    assert(obj.value == 0x269d7e53badac9fULL);
    assert(obj.formatted() == "0269-D7E5-3BAD-AC9F");
  }
  {
    std::cout << "  Testing uchrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(32, 1142U, 82, 833U, 1961U);
    auto tmp = UChrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ds::from_persona_units(tmp.get_timestamp(), 323ULL, 488ULL, p);
    assert(obj.value == 0x3f9f8191f90ebbcULL);
    assert(obj.formatted() == "03F9-F819-1F90-EBBC");
  }
  {
    std::cout << "  Testing uchrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(33, 1143U, 83, 834U, 1962U);
    auto tmp = UChrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64h::from_persona_units(tmp.get_timestamp(), 324ULL, 489ULL, p);
    assert(obj.value == 0x39e901df37041fdULL);
    assert(obj.formatted() == "039E-901D-F370-41FD");
  }
  {
    std::cout << "  Testing uchrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(34, 1144U, 84, 835U, 1963U);
    auto tmp = UChrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64m::from_persona_units(tmp.get_timestamp(), 325ULL, 490ULL, p);
    assert(obj.value == 0x364aadd6bf64de2ULL);
    assert(obj.formatted() == "0364-AADD-6BF6-4DE2");
  }
  {
    std::cout << "  Testing uchrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(35, 1145U, 85, 836U, 1964U);
    auto tmp = UChrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64mo::from_persona_units(tmp.get_timestamp(), 326ULL, 491ULL, p);
    assert(obj.value == 0x28a7e514d277c2bULL);
    assert(obj.formatted() == "028A-7E51-4D27-7C2B");
  }
  {
    std::cout << "  Testing uchrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(36, 1146U, 86, 837U, 941U);
    auto tmp = UChrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ms::from_persona_units(tmp.get_timestamp(), 327ULL, 492ULL, p);
    assert(obj.value == 0x31b49d3977645daULL);
    assert(obj.formatted() == "031B-49D3-9776-45DA");
  }
  {
    std::cout << "  Testing uchrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(37, 1147U, 87, 838U, 1966U);
    auto tmp = UChrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 328ULL, 493ULL, p);
    assert(obj.value == 0x32e60144f89ddafULL);
    assert(obj.formatted() == "032E-6014-4F89-DDAF");
  }
  {
    std::cout << "  Testing uchrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(38, 1148U, 88, 839U, 15U);
    auto tmp = UChrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64us::from_persona_units(tmp.get_timestamp(), 329ULL, 14ULL, p);
    assert(obj.value == 0x308a618a1e20670ULL);
    assert(obj.formatted() == "0308-A618-A1E2-0670");
  }
  {
    std::cout << "  Testing uchrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(39, 1149U, 89, 840U, 1968U);
    auto tmp = UChrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64w::from_persona_units(tmp.get_timestamp(), 330ULL, 495ULL, p);
    assert(obj.value == 0x2c384e5abbadd05ULL);
    assert(obj.formatted() == "02C3-84E5-ABBA-DD05");
  }
  {
    std::cout << "  Testing chrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(41, 1151U, 91, 842U, 50U);
    auto tmp = Chrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 332ULL, 113ULL, p);
    assert(obj.value == 0x134d19fULL);
    assert(obj.formatted() == "0134-D19F");
  }
  {
    std::cout << "  Testing chrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(42, 1152U, 92, 843U, 19U);
    auto tmp = Chrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 333ULL, 18ULL, p);
    assert(obj.value == 0xe7a5b8ULL);
    assert(obj.formatted() == "00E7-A5B8");
  }
  {
    std::cout << "  Testing chrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(43, 1153U, 93, 844U, 1972U);
    auto tmp = Chrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 334ULL, 499ULL, p);
    assert(obj.value == 0x1b29975ULL);
    assert(obj.formatted() == "01B2-9975");
  }
  {
    std::cout << "  Testing chrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(44, 1154U, 94, 845U, 1U);
    auto tmp = Chrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 335ULL, 0ULL, p);
    assert(obj.value == 0xd92ab6ULL);
    assert(obj.formatted() == "00D9-2AB6");
  }
  {
    std::cout << "  Testing chrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(45, 1155U, 95, 846U, 950U);
    auto tmp = Chrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 336ULL, 501ULL, p);
    assert(obj.value == 0x145cfbbULL);
    assert(obj.formatted() == "0145-CFBB");
  }
  {
    std::cout << "  Testing chrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(46, 1156U, 96, 847U, 1975U);
    auto tmp = Chrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 337ULL, 502ULL, p);
    assert(obj.value == 0x1a6db6cULL);
    assert(obj.formatted() == "01A6-DB6C");
  }
  {
    std::cout << "  Testing chrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(47, 1157U, 97, 848U, 8U);
    auto tmp = Chrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 338ULL, 7ULL, p);
    assert(obj.value == 0x15b77f3ULL);
    assert(obj.formatted() == "015B-77F3");
  }
  {
    std::cout << "  Testing chrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(48, 1158U, 98, 849U, 441U);
    auto tmp = Chrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32w::from_persona_units(tmp.get_timestamp(), 339ULL, 504ULL, p);
    assert(obj.value == 0x160ab0cULL);
    assert(obj.formatted() == "0160-AB0C");
  }
  {
    std::cout << "  Testing chrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(49, 1159U, 99, 850U, 1978U);
    auto tmp = Chrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 340ULL, 505ULL, p);
    assert(obj.value == 0x1ca9be1ULL);
    assert(obj.formatted() == "01CA-9BE1");
  }
  {
    std::cout << "  Testing chrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(50, 1160U, 100, 851U, 1979U);
    auto tmp = Chrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64cs::from_persona_units(tmp.get_timestamp(), 341ULL, 506ULL, p);
    assert(obj.value == 0x13e1d87d64fd986ULL);
    assert(obj.formatted() == "013E-1D87-D64F-D986");
  }
  {
    std::cout << "  Testing chrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(51, 1161U, 101, 852U, 1980U);
    auto tmp = Chrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64d::from_persona_units(tmp.get_timestamp(), 342ULL, 507ULL, p);
    assert(obj.value == 0x134c52aff8e73d3ULL);
    assert(obj.formatted() == "0134-C52A-FF8E-73D3");
  }
  {
    std::cout << "  Testing chrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(52, 1162U, 102, 853U, 1981U);
    auto tmp = Chrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ds::from_persona_units(tmp.get_timestamp(), 343ULL, 508ULL, p);
    assert(obj.value == 0x1fcfc0c8fd27d08ULL);
    assert(obj.formatted() == "01FC-FC0C-8FD2-7D08");
  }
  {
    std::cout << "  Testing chrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(53, 1163U, 103, 854U, 1982U);
    auto tmp = Chrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64h::from_persona_units(tmp.get_timestamp(), 344ULL, 509ULL, p);
    assert(obj.value == 0x1cf4a2ec6713b91ULL);
    assert(obj.formatted() == "01CF-4A2E-C671-3B91");
  }
  {
    std::cout << "  Testing chrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(54, 1164U, 104, 855U, 1983U);
    auto tmp = Chrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64m::from_persona_units(tmp.get_timestamp(), 345ULL, 510ULL, p);
    assert(obj.value == 0x1b255604576fab6ULL);
    assert(obj.formatted() == "01B2-5560-4576-FAB6");
  }
  {
    std::cout << "  Testing chrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(55, 1165U, 105, 856U, 1984U);
    auto tmp = Chrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64mo::from_persona_units(tmp.get_timestamp(), 346ULL, 511ULL, p);
    assert(obj.value == 0x1429756be5f008fULL);
    assert(obj.formatted() == "0142-9756-BE5F-008F");
  }
  {
    std::cout << "  Testing chrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(56, 1166U, 106, 857U, 961U);
    auto tmp = Chrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ms::from_persona_units(tmp.get_timestamp(), 347ULL, 512ULL, p);
    assert(obj.value == 0x18da4e9cbb65680ULL);
    assert(obj.formatted() == "018D-A4E9-CBB6-5680");
  }
  {
    std::cout << "  Testing chrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(57, 1167U, 107, 858U, 1986U);
    auto tmp = Chrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 348ULL, 513ULL, p);
    assert(obj.value == 0x197300a18419c1dULL);
    assert(obj.formatted() == "0197-300A-1841-9C1D");
  }
  {
    std::cout << "  Testing chrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(58, 1168U, 108, 859U, 3U);
    auto tmp = Chrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64us::from_persona_units(tmp.get_timestamp(), 349ULL, 2ULL, p);
    assert(obj.value == 0x184530c50f1022cULL);
    assert(obj.formatted() == "0184-530C-50F1-022C");
  }
  {
    std::cout << "  Testing chrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(59, 1169U, 109, 860U, 1988U);
    auto tmp = Chrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64w::from_persona_units(tmp.get_timestamp(), 350ULL, 515ULL, p);
    assert(obj.value == 0x161a4d3c3951c19ULL);
    assert(obj.formatted() == "0161-A4D3-C395-1C19");
  }
  {
    std::cout << "  Testing uchrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(61, 1171U, 111, 862U, 70U);
    auto tmp = UChrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32d::from_persona_units(tmp.get_timestamp(), 352ULL, 5ULL, p);
    assert(obj.value == 0x269d9d1ULL);
    assert(obj.formatted() == "0269-D9D1");
  }
  {
    std::cout << "  Testing uchrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(62, 1172U, 112, 863U, 7U);
    auto tmp = UChrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32h::from_persona_units(tmp.get_timestamp(), 353ULL, 6ULL, p);
    assert(obj.value == 0x1cf4bb2ULL);
    assert(obj.formatted() == "01CF-4BB2");
  }
  {
    std::cout << "  Testing uchrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(63, 1173U, 113, 864U, 1992U);
    auto tmp = UChrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32hy::from_persona_units(tmp.get_timestamp(), 354ULL, 519ULL, p);
    assert(obj.value == 0x36b18cbULL);
    assert(obj.formatted() == "036B-18CB");
  }
  {
    std::cout << "  Testing uchrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(64, 1174U, 114, 865U, 1U);
    auto tmp = UChrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32m::from_persona_units(tmp.get_timestamp(), 355ULL, 0ULL, p);
    assert(obj.value == 0x1b25566ULL);
    assert(obj.formatted() == "01B2-5566");
  }
  {
    std::cout << "  Testing uchrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(65, 1175U, 115, 866U, 970U);
    auto tmp = UChrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32mo::from_persona_units(tmp.get_timestamp(), 356ULL, 521ULL, p);
    assert(obj.value == 0x28d6f1bULL);
    assert(obj.formatted() == "028D-6F1B");
  }
  {
    std::cout << "  Testing uchrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(66, 1176U, 116, 867U, 1995U);
    auto tmp = UChrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32q::from_persona_units(tmp.get_timestamp(), 357ULL, 522ULL, p);
    assert(obj.value == 0x37bdfdaULL);
    assert(obj.formatted() == "037B-DFDA");
  }
  {
    std::cout << "  Testing uchrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(67, 1177U, 117, 868U, 12U);
    auto tmp = UChrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32tm::from_persona_units(tmp.get_timestamp(), 358ULL, 11ULL, p);
    assert(obj.value == 0x2b6ef73ULL);
    assert(obj.formatted() == "02B6-EF73");
  }
  {
    std::cout << "  Testing uchrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(68, 1178U, 118, 869U, 461U);
    auto tmp = UChrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32w::from_persona_units(tmp.get_timestamp(), 359ULL, 12ULL, p);
    assert(obj.value == 0x2c19a10ULL);
    assert(obj.formatted() == "02C1-9A10");
  }
  {
    std::cout << "  Testing uchrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(69, 1179U, 119, 870U, 1998U);
    auto tmp = UChrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32y::from_persona_units(tmp.get_timestamp(), 360ULL, 525ULL, p);
    assert(obj.value == 0x3b29bdbULL);
    assert(obj.formatted() == "03B2-9BDB");
  }
  {
    std::cout << "  Testing uchrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(70, 1180U, 120, 871U, 1999U);
    auto tmp = UChrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64cs::from_persona_units(tmp.get_timestamp(), 361ULL, 526ULL, p);
    assert(obj.value == 0x27c3b0fac07bfbeULL);
    assert(obj.formatted() == "027C-3B0F-AC07-BFBE");
  }
  {
    std::cout << "  Testing uchrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(71, 1181U, 121, 872U, 2000U);
    auto tmp = UChrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64d::from_persona_units(tmp.get_timestamp(), 362ULL, 527ULL, p);
    assert(obj.value == 0x269ec30dbce2247ULL);
    assert(obj.formatted() == "0269-EC30-DBCE-2247");
  }
  {
    std::cout << "  Testing uchrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(72, 1182U, 122, 873U, 2001U);
    auto tmp = UChrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ds::from_persona_units(tmp.get_timestamp(), 363ULL, 528ULL, p);
    assert(obj.value == 0x3f9f819134ea854ULL);
    assert(obj.formatted() == "03F9-F819-134E-A854");
  }
  {
    std::cout << "  Testing uchrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(73, 1183U, 123, 874U, 2002U);
    auto tmp = UChrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64h::from_persona_units(tmp.get_timestamp(), 364ULL, 529ULL, p);
    assert(obj.value == 0x39e93932769e9a5ULL);
    assert(obj.formatted() == "039E-9393-2769-E9A5");
  }
  {
    std::cout << "  Testing uchrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(74, 1184U, 124, 875U, 2003U);
    auto tmp = UChrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64m::from_persona_units(tmp.get_timestamp(), 365ULL, 530ULL, p);
    assert(obj.value == 0x364aacfa4b69fcaULL);
    assert(obj.formatted() == "0364-AACF-A4B6-9FCA");
  }
  {
    std::cout << "  Testing uchrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(75, 1185U, 125, 876U, 2004U);
    auto tmp = UChrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64mo::from_persona_units(tmp.get_timestamp(), 366ULL, 531ULL, p);
    assert(obj.value == 0x285cdaefe8e0b53ULL);
    assert(obj.formatted() == "0285-CDAE-FE8E-0B53");
  }
  {
    std::cout << "  Testing uchrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(76, 1186U, 126, 877U, 981U);
    auto tmp = UChrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ms::from_persona_units(tmp.get_timestamp(), 367ULL, 532ULL, p);
    assert(obj.value == 0x31b49d3977214e8ULL);
    assert(obj.formatted() == "031B-49D3-9772-14E8");
  }
  {
    std::cout << "  Testing uchrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(77, 1187U, 127, 878U, 2006U);
    auto tmp = UChrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 368ULL, 533ULL, p);
    assert(obj.value == 0x32e60144969d175ULL);
    assert(obj.formatted() == "032E-6014-4969-D175");
  }
  {
    std::cout << "  Testing uchrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(78, 1188U, 0, 879U, 23U);
    auto tmp = UChrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64us::from_persona_units(tmp.get_timestamp(), 369ULL, 22ULL, p);
    assert(obj.value == 0x308a618a1e205f8ULL);
    assert(obj.formatted() == "0308-A618-A1E2-05F8");
  }
  {
    std::cout << "  Testing uchrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(79, 1189U, 1, 880U, 2008U);
    auto tmp = UChrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64w::from_persona_units(tmp.get_timestamp(), 370ULL, 535ULL, p);
    assert(obj.value == 0x2c08a3a8b0171ffULL);
    assert(obj.formatted() == "02C0-8A3A-8B01-71FF");
  }
  {
    std::cout << "  Testing chrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(81, 1191U, 3, 882U, 90U);
    auto tmp = Chrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 372ULL, 25ULL, p);
    assert(obj.value == 0x134d1afULL);
    assert(obj.formatted() == "0134-D1AF");
  }
  {
    std::cout << "  Testing chrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(82, 1192U, 4, 883U, 27U);
    auto tmp = Chrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 373ULL, 26ULL, p);
    assert(obj.value == 0xe7a4aeULL);
    assert(obj.formatted() == "00E7-A4AE");
  }
  {
    std::cout << "  Testing chrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(83, 1193U, 5, 884U, 2012U);
    auto tmp = Chrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 374ULL, 539ULL, p);
    assert(obj.value == 0x1a0df11ULL);
    assert(obj.formatted() == "01A0-DF11");
  }
  {
    std::cout << "  Testing chrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(84, 1194U, 6, 885U, 1U);
    auto tmp = Chrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 375ULL, 0ULL, p);
    assert(obj.value == 0xd92ab4ULL);
    assert(obj.formatted() == "00D9-2AB4");
  }
  {
    std::cout << "  Testing chrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(85, 1195U, 7, 886U, 990U);
    auto tmp = Chrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 376ULL, 541ULL, p);
    assert(obj.value == 0x1478cddULL);
    assert(obj.formatted() == "0147-8CDD");
  }
  {
    std::cout << "  Testing chrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(86, 1196U, 8, 887U, 2015U);
    auto tmp = Chrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 377ULL, 542ULL, p);
    assert(obj.value == 0x1b898d6ULL);
    assert(obj.formatted() == "01B8-98D6");
  }
  {
    std::cout << "  Testing chrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(87, 1197U, 9, 888U, 0U);
    auto tmp = Chrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 378ULL, 15ULL, p);
    assert(obj.value == 0x15b77f3ULL);
    assert(obj.formatted() == "015B-77F3");
  }
  {
    std::cout << "  Testing chrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(88, 1198U, 10, 889U, 481U);
    auto tmp = Chrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32w::from_persona_units(tmp.get_timestamp(), 379ULL, 32ULL, p);
    assert(obj.value == 0x1610afcULL);
    assert(obj.formatted() == "0161-0AFC");
  }
  {
    std::cout << "  Testing chrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(89, 1199U, 11, 890U, 2018U);
    auto tmp = Chrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 380ULL, 545ULL, p);
    assert(obj.value == 0x1e9dd55ULL);
    assert(obj.formatted() == "01E9-DD55");
  }
  {
    std::cout << "  Testing chrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(90, 1200U, 12, 891U, 2019U);
    auto tmp = Chrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64cs::from_persona_units(tmp.get_timestamp(), 381ULL, 546ULL, p);
    assert(obj.value == 0x13e1d87d6149e6cULL);
    assert(obj.formatted() == "013E-1D87-D614-9E6C");
  }
  {
    std::cout << "  Testing chrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(91, 1201U, 13, 892U, 2020U);
    auto tmp = Chrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64d::from_persona_units(tmp.get_timestamp(), 382ULL, 547ULL, p);
    assert(obj.value == 0x134fec4d79b69ddULL);
    assert(obj.formatted() == "0134-FEC4-D79B-69DD");
  }
  {
    std::cout << "  Testing chrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(92, 1202U, 14, 893U, 2021U);
    auto tmp = Chrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ds::from_persona_units(tmp.get_timestamp(), 383ULL, 548ULL, p);
    assert(obj.value == 0x1fcfc0c8ff7b15cULL);
    assert(obj.formatted() == "01FC-FC0C-8FF7-B15C");
  }
  {
    std::cout << "  Testing chrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(93, 1203U, 15, 894U, 2022U);
    auto tmp = Chrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64h::from_persona_units(tmp.get_timestamp(), 384ULL, 549ULL, p);
    assert(obj.value == 0x1cf4ae066638d0bULL);
    assert(obj.formatted() == "01CF-4AE0-6663-8D0B");
  }
  {
    std::cout << "  Testing chrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(94, 1204U, 16, 895U, 2023U);
    auto tmp = Chrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64m::from_persona_units(tmp.get_timestamp(), 385ULL, 550ULL, p);
    assert(obj.value == 0x1b2556541762a14ULL);
    assert(obj.formatted() == "01B2-5565-4176-2A14");
  }
  {
    std::cout << "  Testing chrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(95, 1205U, 17, 896U, 2024U);
    auto tmp = Chrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64mo::from_persona_units(tmp.get_timestamp(), 386ULL, 551ULL, p);
    assert(obj.value == 0x1462def7e8b8315ULL);
    assert(obj.formatted() == "0146-2DEF-7E8B-8315");
  }
  {
    std::cout << "  Testing chrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(96, 1206U, 18, 897U, 1001U);
    auto tmp = Chrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64ms::from_persona_units(tmp.get_timestamp(), 387ULL, 552ULL, p);
    assert(obj.value == 0x18da4e9cbbe8d7aULL);
    assert(obj.formatted() == "018D-A4E9-CBBE-8D7A");
  }
  {
    std::cout << "  Testing chrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(97, 1207U, 19, 898U, 2026U);
    auto tmp = Chrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 388ULL, 553ULL, p);
    assert(obj.value == 0x197300a0a6ddf79ULL);
    assert(obj.formatted() == "0197-300A-0A6D-DF79");
  }
  {
    std::cout << "  Testing chrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(98, 1208U, 20, 899U, 11U);
    auto tmp = Chrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64us::from_persona_units(tmp.get_timestamp(), 389ULL, 10ULL, p);
    assert(obj.value == 0x184530c50f10362ULL);
    assert(obj.formatted() == "0184-530C-50F1-0362");
  }
  {
    std::cout << "  Testing chrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(99, 1209U, 21, 900U, 2028U);
    auto tmp = Chrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono64w::from_persona_units(tmp.get_timestamp(), 390ULL, 555ULL, p);
    assert(obj.value == 0x161a341d3917033ULL);
    assert(obj.formatted() == "0161-A341-D391-7033");
  }
  {
    std::cout << "  Testing uchrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(101, 1211U, 23, 902U, 110U);
    auto tmp = UChrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32d::from_persona_units(tmp.get_timestamp(), 392ULL, 45ULL, p);
    assert(obj.value == 0x269c987ULL);
    assert(obj.formatted() == "0269-C987");
  }
  {
    std::cout << "  Testing uchrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(102, 1212U, 24, 903U, 15U);
    auto tmp = UChrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32h::from_persona_units(tmp.get_timestamp(), 393ULL, 14ULL, p);
    assert(obj.value == 0x1cf49eaULL);
    assert(obj.formatted() == "01CF-49EA");
  }
  {
    std::cout << "  Testing uchrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(103, 1213U, 25, 904U, 2032U);
    auto tmp = UChrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32hy::from_persona_units(tmp.get_timestamp(), 394ULL, 559ULL, p);
    assert(obj.value == 0x32918dfULL);
    assert(obj.formatted() == "0329-18DF");
  }
  {
    std::cout << "  Testing uchrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(104, 1214U, 26, 905U, 1U);
    auto tmp = UChrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32m::from_persona_units(tmp.get_timestamp(), 395ULL, 0ULL, p);
    assert(obj.value == 0x1b2556cULL);
    assert(obj.formatted() == "01B2-556C");
  }
  {
    std::cout << "  Testing uchrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(105, 1215U, 27, 906U, 1010U);
    auto tmp = UChrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32mo::from_persona_units(tmp.get_timestamp(), 396ULL, 561ULL, p);
    assert(obj.value == 0x289cdb5ULL);
    assert(obj.formatted() == "0289-CDB5");
  }
  {
    std::cout << "  Testing uchrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(106, 1216U, 28, 907U, 2035U);
    auto tmp = UChrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32q::from_persona_units(tmp.get_timestamp(), 397ULL, 562ULL, p);
    assert(obj.value == 0x37ebd8cULL);
    assert(obj.formatted() == "037E-BD8C");
  }
  {
    std::cout << "  Testing uchrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(107, 1217U, 29, 908U, 4U);
    auto tmp = UChrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32tm::from_persona_units(tmp.get_timestamp(), 398ULL, 3ULL, p);
    assert(obj.value == 0x2b6ef31ULL);
    assert(obj.formatted() == "02B6-EF31");
  }
  {
    std::cout << "  Testing uchrono32w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(108, 1218U, 30, 909U, 501U);
    auto tmp = UChrono32w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32w::from_persona_units(tmp.get_timestamp(), 399ULL, 52ULL, p);
    assert(obj.value == 0x2c1b222ULL);
    assert(obj.formatted() == "02C1-B222");
  }
  {
    std::cout << "  Testing uchrono32y with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(109, 1219U, 31, 910U, 2038U);
    auto tmp = UChrono32y::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono32y::from_persona_units(tmp.get_timestamp(), 400ULL, 565ULL, p);
    assert(obj.value == 0x3c49837ULL);
    assert(obj.formatted() == "03C4-9837");
  }
  {
    std::cout << "  Testing uchrono64cs with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(110, 1220U, 32, 911U, 2039U);
    auto tmp = UChrono64cs::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64cs::from_persona_units(tmp.get_timestamp(), 401ULL, 566ULL, p);
    assert(obj.value == 0x27c3b0facf139e4ULL);
    assert(obj.formatted() == "027C-3B0F-ACF1-39E4");
  }
  {
    std::cout << "  Testing uchrono64d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(111, 1221U, 33, 912U, 2040U);
    auto tmp = UChrono64d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64d::from_persona_units(tmp.get_timestamp(), 402ULL, 567ULL, p);
    assert(obj.value == 0x269be5c4bb5f8a9ULL);
    assert(obj.formatted() == "0269-BE5C-4BB5-F8A9");
  }
  {
    std::cout << "  Testing uchrono64ds with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(112, 1222U, 34, 913U, 2041U);
    auto tmp = UChrono64ds::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ds::from_persona_units(tmp.get_timestamp(), 403ULL, 568ULL, p);
    assert(obj.value == 0x3f9f81910c07630ULL);
    assert(obj.formatted() == "03F9-F819-10C0-7630");
  }
  {
    std::cout << "  Testing uchrono64h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(113, 1223U, 35, 914U, 2042U);
    auto tmp = UChrono64h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64h::from_persona_units(tmp.get_timestamp(), 404ULL, 569ULL, p);
    assert(obj.value == 0x39e92e77372fb1fULL);
    assert(obj.formatted() == "039E-92E7-7372-FB1F");
  }
  {
    std::cout << "  Testing uchrono64m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(114, 1224U, 36, 915U, 2043U);
    auto tmp = UChrono64m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64m::from_persona_units(tmp.get_timestamp(), 405ULL, 570ULL, p);
    assert(obj.value == 0x364aad17974a168ULL);
    assert(obj.formatted() == "0364-AAD1-7974-A168");
  }
  {
    std::cout << "  Testing uchrono64mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(115, 1225U, 37, 916U, 2044U);
    auto tmp = UChrono64mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64mo::from_persona_units(tmp.get_timestamp(), 406ULL, 571ULL, p);
    assert(obj.value == 0x288d93d3d918759ULL);
    assert(obj.formatted() == "0288-D93D-3D91-8759");
  }
  {
    std::cout << "  Testing uchrono64ms with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(116, 1226U, 38, 917U, 1021U);
    auto tmp = UChrono64ms::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64ms::from_persona_units(tmp.get_timestamp(), 407ULL, 572ULL, p);
    assert(obj.value == 0x31b49d3977ba422ULL);
    assert(obj.formatted() == "031B-49D3-977B-A422");
  }
  {
    std::cout << "  Testing uchrono64s with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(117, 1227U, 39, 918U, 2046U);
    auto tmp = UChrono64s::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 408ULL, 573ULL, p);
    assert(obj.value == 0x32e601404f9e979ULL);
    assert(obj.formatted() == "032E-6014-04F9-E979");
  }
  {
    std::cout << "  Testing uchrono64us with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(118, 1228U, 40, 919U, 31U);
    auto tmp = UChrono64us::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64us::from_persona_units(tmp.get_timestamp(), 409ULL, 30ULL, p);
    assert(obj.value == 0x308a618a1e20166ULL);
    assert(obj.formatted() == "0308-A618-A1E2-0166");
  }
  {
    std::cout << "  Testing uchrono64w with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(119, 1229U, 41, 920U, 2048U);
    auto tmp = UChrono64w::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        UChrono64w::from_persona_units(tmp.get_timestamp(), 410ULL, 575ULL, p);
    assert(obj.value == 0x2c37e9977df5747ULL);
    assert(obj.formatted() == "02C3-7E99-77DF-5747");
  }
  {
    std::cout << "  Testing chrono32d with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(121, 1231U, 43, 922U, 2U);
    auto tmp = Chrono32d::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 412ULL, 65ULL, p);
    assert(obj.value == 0x134d5ffULL);
    assert(obj.formatted() == "0134-D5FF");
  }
  {
    std::cout << "  Testing chrono32h with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(122, 1232U, 44, 923U, 3U);
    auto tmp = Chrono32h::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 413ULL, 2ULL, p);
    assert(obj.value == 0xe7a4fcULL);
    assert(obj.formatted() == "00E7-A4FC");
  }
  {
    std::cout << "  Testing chrono32hy with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(123, 1233U, 45, 924U, 4U);
    auto tmp = Chrono32hy::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 414ULL, 579ULL, p);
    assert(obj.value == 0x1b41bb5ULL);
    assert(obj.formatted() == "01B4-1BB5");
  }
  {
    std::cout << "  Testing chrono32m with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(124, 1234U, 46, 925U, 1U);
    auto tmp = Chrono32m::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 415ULL, 0ULL, p);
    assert(obj.value == 0xd92ab6ULL);
    assert(obj.formatted() == "00D9-2AB6");
  }
  {
    std::cout << "  Testing chrono32mo with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(125, 1235U, 47, 926U, 6U);
    auto tmp = Chrono32mo::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 416ULL, 581ULL, p);
    assert(obj.value == 0x144cf8dULL);
    assert(obj.formatted() == "0144-CF8D");
  }
  {
    std::cout << "  Testing chrono32q with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(126, 1236U, 48, 927U, 7U);
    auto tmp = Chrono32q::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 417ULL, 582ULL, p);
    assert(obj.value == 0x1b44dd2ULL);
    assert(obj.formatted() == "01B4-4DD2");
  }
  {
    std::cout << "  Testing chrono32tm with 2023-05-20T10:30:00.123456Z"
              << std::endl;
    Persona p(127, 1237U, 49, 928U, 8U);
    auto tmp = Chrono32tm::from_iso_string("2023-05-20T10:30:00.123456Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 418ULL, 7ULL, p);
    assert(obj.value == 0x15b77f1ULL);
    assert(obj.formatted() == "015B-77F1");
  }

  // Case 1
  {
    std::cout << "  Testing chrono32y with 2023-05-20T10:30:00.000000Z"
              << std::endl;
    Persona p(0, 0U, 0, 0U, 0U);
    auto tmp = Chrono32y::from_iso_string("2023-05-20T10:30:00.000000Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 4660ULL, 1383ULL, p);
    assert(obj.value == 0x1c0e1f7ULL);
    assert(obj.formatted() == "01C0-E1F7");
  }
  {
    std::cout << "  Testing chrono64s with 2023-05-20T10:30:00.000000Z"
              << std::endl;
    Persona p(0, 0U, 0, 0U, 0U);
    auto tmp = Chrono64s::from_iso_string("2023-05-20T10:30:00.000000Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 4660ULL, 1383ULL, p);
    assert(obj.value == 0x197300a3bbe5addULL);
    assert(obj.formatted() == "0197-300A-3BBE-5ADD");
  }

  // Epoch baseline
  {
    std::cout << "  Testing uchrono64mo with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64mo::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf7694cf605d0eULL);
    assert(obj.formatted() == "000F-7694-CF60-5D0E");
  }
  {
    std::cout << "  Testing chrono64mo with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64mo::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7bb4dcf605d0eULL);
    assert(obj.formatted() == "0007-BB4D-CF60-5D0E");
  }
  {
    std::cout << "  Testing uchrono64w with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64w::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x3dda533d8190aULL);
    assert(obj.formatted() == "0003-DDA5-33D8-190A");
  }
  {
    std::cout << "  Testing chrono64w with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64w::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1eed373d8190aULL);
    assert(obj.formatted() == "0001-EED3-73D8-190A");
  }
  {
    std::cout << "  Testing uchrono64d with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64d::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64d::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7bb429ec08f6ULL);
    assert(obj.formatted() == "0000-7BB4-29EC-08F6");
  }
  {
    std::cout << "  Testing chrono64d with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64d::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64d::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x3dda61ec08f6ULL);
    assert(obj.formatted() == "0000-3DDA-61EC-08F6");
  }
  {
    std::cout << "  Testing uchrono64h with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64h::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64h::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7bb617b02e4ULL);
    assert(obj.formatted() == "0000-07BB-617B-02E4");
  }
  {
    std::cout << "  Testing chrono64h with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64h::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64h::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x3dd8afb02e4ULL);
    assert(obj.formatted() == "0000-03DD-8AFB-02E4");
  }
  {
    std::cout << "  Testing uchrono64m with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64m::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64m::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1eea075e98ULL);
    assert(obj.formatted() == "0000-001E-EA07-5E98");
  }
  {
    std::cout << "  Testing chrono64m with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64m::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64m::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf72bf5e98ULL);
    assert(obj.formatted() == "0000-000F-72BF-5E98");
  }
  {
    std::cout << "  Testing uchrono64s with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64s::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7b79e9b0ULL);
    assert(obj.formatted() == "0000-0000-7B79-E9B0");
  }
  {
    std::cout << "  Testing chrono64s with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64s::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x3d09e9b0ULL);
    assert(obj.formatted() == "0000-0000-3D09-E9B0");
  }
  {
    std::cout << "  Testing uchrono64ds with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64ds::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64ds::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf427844ULL);
    assert(obj.formatted() == "0000-0000-0F42-7844");
  }
  {
    std::cout << "  Testing chrono64ds with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64ds::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64ds::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7fc7844ULL);
    assert(obj.formatted() == "0000-0000-07FC-7844");
  }
  {
    std::cout << "  Testing uchrono64cs with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64cs::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64cs::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xe2ffb0ULL);
    assert(obj.formatted() == "0000-0000-00E2-FFB0");
  }
  {
    std::cout << "  Testing chrono64cs with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64cs::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64cs::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1f9fb0ULL);
    assert(obj.formatted() == "0000-0000-001F-9FB0");
  }
  {
    std::cout << "  Testing uchrono64ms with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64ms::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64ms::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7e7c4ULL);
    assert(obj.formatted() == "0000-0000-0007-E7C4");
  }
  {
    std::cout << "  Testing chrono64ms with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64ms::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64ms::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x84fc4ULL);
    assert(obj.formatted() == "0000-0000-0008-4FC4");
  }
  {
    std::cout << "  Testing uchrono64us with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono64us::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64us::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x732ULL);
    assert(obj.formatted() == "0000-0000-0000-0732");
  }
  {
    std::cout << "  Testing chrono64us with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono64us::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono64us::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x272ULL);
    assert(obj.formatted() == "0000-0000-0000-0272");
  }
  {
    std::cout << "  Testing uchrono32y with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32y::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32y::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf4d8a6ULL);
    assert(obj.formatted() == "00F4-D8A6");
  }
  {
    std::cout << "  Testing chrono32y with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32y::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7178a6ULL);
    assert(obj.formatted() == "0071-78A6");
  }
  {
    std::cout << "  Testing uchrono32hy with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32hy::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32hy::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x7178a6ULL);
    assert(obj.formatted() == "0071-78A6");
  }
  {
    std::cout << "  Testing chrono32hy with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32hy::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xfc8a6ULL);
    assert(obj.formatted() == "000F-C8A6");
  }
  {
    std::cout << "  Testing uchrono32q with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32q::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32q::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xfc8a6ULL);
    assert(obj.formatted() == "000F-C8A6");
  }
  {
    std::cout << "  Testing chrono32q with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32q::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1098a6ULL);
    assert(obj.formatted() == "0010-98A6");
  }
  {
    std::cout << "  Testing uchrono32mo with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32mo::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x84fc4ULL);
    assert(obj.formatted() == "0008-4FC4");
  }
  {
    std::cout << "  Testing chrono32mo with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32mo::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x78fc4ULL);
    assert(obj.formatted() == "0007-8FC4");
  }
  {
    std::cout << "  Testing uchrono32w with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 277U);
    auto tmp = UChrono32w::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x3c6f0ULL);
    assert(obj.formatted() == "0003-C6F0");
  }
  {
    std::cout << "  Testing chrono32w with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 277U);
    auto tmp = Chrono32w::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x136f0ULL);
    assert(obj.formatted() == "0001-36F0");
  }
  {
    std::cout << "  Testing uchrono32d with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono32d::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32d::from_persona_units(tmp.get_timestamp(), 123ULL, 72ULL, p);
    assert(obj.value == 0x4df4ULL);
    assert(obj.formatted() == "0000-4DF4");
  }
  {
    std::cout << "  Testing chrono32d with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono32d::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 123ULL, 72ULL, p);
    assert(obj.value == 0x17f4ULL);
    assert(obj.formatted() == "0000-17F4");
  }
  {
    std::cout << "  Testing uchrono32h with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono32h::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32h::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x272ULL);
    assert(obj.formatted() == "0000-0272");
  }
  {
    std::cout << "  Testing chrono32h with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono32h::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x72ULL);
    assert(obj.formatted() == "0000-0072");
  }
  {
    std::cout << "  Testing uchrono32tm with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 5U);
    auto tmp = UChrono32tm::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32tm::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x3aULL);
    assert(obj.formatted() == "0000-003A");
  }
  {
    std::cout << "  Testing chrono32tm with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 5U);
    auto tmp = Chrono32tm::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x3aULL);
    assert(obj.formatted() == "0000-003A");
  }
  {
    std::cout << "  Testing uchrono32m with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 1U);
    auto tmp = UChrono32m::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono32m::from_persona_units(tmp.get_timestamp(), 123ULL, 0ULL, p);
    assert(obj.value == 0xcULL);
    assert(obj.formatted() == "0000-000C");
  }
  {
    std::cout << "  Testing chrono32m with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 1U);
    auto tmp = Chrono32m::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 123ULL, 0ULL, p);
    assert(obj.value == 0x4ULL);
    assert(obj.formatted() == "0000-0004");
  }

  // Far future baseline
  {
    std::cout << "  Testing uchrono64mo with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64mo::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x173f7694cf605d0eULL);
    assert(obj.formatted() == "173F-7694-CF60-5D0E");
  }
  {
    std::cout << "  Testing chrono64mo with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64mo::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xb9fbb4dcf605d0eULL);
    assert(obj.formatted() == "0B9F-BB4D-CF60-5D0E");
  }
  {
    std::cout << "  Testing uchrono64w with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64w::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1947dda533d8190aULL);
    assert(obj.formatted() == "1947-DDA5-33D8-190A");
  }
  {
    std::cout << "  Testing chrono64w with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64w::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xca3eed373d8190aULL);
    assert(obj.formatted() == "0CA3-EED3-73D8-190A");
  }
  {
    std::cout << "  Testing uchrono64d with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64d::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64d::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x161d7bb429ec08f6ULL);
    assert(obj.formatted() == "161D-7BB4-29EC-08F6");
  }
  {
    std::cout << "  Testing chrono64d with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64d::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64d::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xb0ebdda61ec08f6ULL);
    assert(obj.formatted() == "0B0E-BDDA-61EC-08F6");
  }
  {
    std::cout << "  Testing uchrono64h with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64h::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64h::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x212c3fbb617b02e4ULL);
    assert(obj.formatted() == "212C-3FBB-617B-02E4");
  }
  {
    std::cout << "  Testing chrono64h with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64h::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64h::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x10961fdd8afb02e4ULL);
    assert(obj.formatted() == "1096-1FDD-8AFB-02E4");
  }
  {
    std::cout << "  Testing uchrono64m with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64m::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64m::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1f197bfeea075e98ULL);
    assert(obj.formatted() == "1F19-7BFE-EA07-5E98");
  }
  {
    std::cout << "  Testing chrono64m with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64m::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64m::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf8cbdff72bf5e98ULL);
    assert(obj.formatted() == "0F8C-BDFF-72BF-5E98");
  }
  {
    std::cout << "  Testing uchrono64s with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64s::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1d27e43ffb79e9b0ULL);
    assert(obj.formatted() == "1D27-E43F-FB79-E9B0");
  }
  {
    std::cout << "  Testing chrono64s with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64s::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64s::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xe93f21ffd09e9b0ULL);
    assert(obj.formatted() == "0E93-F21F-FD09-E9B0");
  }
  {
    std::cout << "  Testing uchrono64ds with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64ds::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64ds::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x2471dd4f6f427844ULL);
    assert(obj.formatted() == "2471-DD4F-6F42-7844");
  }
  {
    std::cout << "  Testing chrono64ds with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64ds::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64ds::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1238eea7b7fc7844ULL);
    assert(obj.formatted() == "1238-EEA7-B7FC-7844");
  }
  {
    std::cout << "  Testing uchrono64cs with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64cs::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64cs::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x16c72a519ce2ffb0ULL);
    assert(obj.formatted() == "16C7-2A51-9CE2-FFB0");
  }
  {
    std::cout << "  Testing chrono64cs with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64cs::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64cs::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xb639528ce1f9fb0ULL);
    assert(obj.formatted() == "0B63-9528-CE1F-9FB0");
  }
  {
    std::cout << "  Testing uchrono64ms with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono64ms::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64ms::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1c78f4e60307e7c4ULL);
    assert(obj.formatted() == "1C78-F4E6-0307-E7C4");
  }
  {
    std::cout << "  Testing chrono64ms with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono64ms::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64ms::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xe3c7a7301884fc4ULL);
    assert(obj.formatted() == "0E3C-7A73-0188-4FC4");
  }
  {
    std::cout << "  Testing uchrono64us with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono64us::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono64us::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x1bce1f289eee0732ULL);
    assert(obj.formatted() == "1BCE-1F28-9EEE-0732");
  }
  {
    std::cout << "  Testing chrono64us with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono64us::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono64us::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0xde70f944f770272ULL);
    assert(obj.formatted() == "0DE7-0F94-4F77-0272");
  }
  {
    std::cout << "  Testing uchrono32y with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32y::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32y::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1ef4d8a6ULL);
    assert(obj.formatted() == "1EF4-D8A6");
  }
  {
    std::cout << "  Testing chrono32y with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32y::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32y::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf7178a6ULL);
    assert(obj.formatted() == "0F71-78A6");
  }
  {
    std::cout << "  Testing uchrono32hy with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32hy::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32hy::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1ef178a6ULL);
    assert(obj.formatted() == "1EF1-78A6");
  }
  {
    std::cout << "  Testing chrono32hy with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32hy::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32hy::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf4fc8a6ULL);
    assert(obj.formatted() == "0F4F-C8A6");
  }
  {
    std::cout << "  Testing uchrono32q with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32q::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32q::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1ecfc8a6ULL);
    assert(obj.formatted() == "1ECF-C8A6");
  }
  {
    std::cout << "  Testing chrono32q with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32q::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32q::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xf7098a6ULL);
    assert(obj.formatted() == "0F70-98A6");
  }
  {
    std::cout << "  Testing uchrono32mo with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = UChrono32mo::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x17384fc4ULL);
    assert(obj.formatted() == "1738-4FC4");
  }
  {
    std::cout << "  Testing chrono32mo with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 789U);
    auto tmp = Chrono32mo::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32mo::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xb9f8fc4ULL);
    assert(obj.formatted() == "0B9F-8FC4");
  }
  {
    std::cout << "  Testing uchrono32w with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 277U);
    auto tmp = UChrono32w::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0x1947c6f0ULL);
    assert(obj.formatted() == "1947-C6F0");
  }
  {
    std::cout << "  Testing chrono32w with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 277U);
    auto tmp = Chrono32w::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32w::from_persona_units(tmp.get_timestamp(), 123ULL, 456ULL, p);
    assert(obj.value == 0xca336f0ULL);
    assert(obj.formatted() == "0CA3-36F0");
  }
  {
    std::cout << "  Testing uchrono32d with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono32d::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32d::from_persona_units(tmp.get_timestamp(), 123ULL, 72ULL, p);
    assert(obj.value == 0x161d4df4ULL);
    assert(obj.formatted() == "161D-4DF4");
  }
  {
    std::cout << "  Testing chrono32d with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono32d::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32d::from_persona_units(tmp.get_timestamp(), 123ULL, 72ULL, p);
    assert(obj.value == 0xb0e97f4ULL);
    assert(obj.formatted() == "0B0E-97F4");
  }
  {
    std::cout << "  Testing uchrono32h with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = UChrono32h::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32h::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x10961e72ULL);
    assert(obj.formatted() == "1096-1E72");
  }
  {
    std::cout << "  Testing chrono32h with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 21U);
    auto tmp = Chrono32h::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32h::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x84b0e72ULL);
    assert(obj.formatted() == "084B-0E72");
  }
  {
    std::cout << "  Testing uchrono32tm with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 5U);
    auto tmp = UChrono32tm::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32tm::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0x18e12f3aULL);
    assert(obj.formatted() == "18E1-2F3A");
  }
  {
    std::cout << "  Testing chrono32tm with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 5U);
    auto tmp = Chrono32tm::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32tm::from_persona_units(tmp.get_timestamp(), 123ULL, 8ULL, p);
    assert(obj.value == 0xc7097baULL);
    assert(obj.formatted() == "0C70-97BA");
  }
  {
    std::cout << "  Testing uchrono32m with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 1U);
    auto tmp = UChrono32m::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        UChrono32m::from_persona_units(tmp.get_timestamp(), 123ULL, 0ULL, p);
    assert(obj.value == 0xf8cbdfcULL);
    assert(obj.formatted() == "0F8C-BDFC");
  }
  {
    std::cout << "  Testing chrono32m with 2050-12-31T23:59:59Z" << std::endl;
    Persona p(1, 456U, 2, 321U, 1U);
    auto tmp = Chrono32m::from_iso_string("2050-12-31T23:59:59Z");
    auto obj =
        Chrono32m::from_persona_units(tmp.get_timestamp(), 123ULL, 0ULL, p);
    assert(obj.value == 0x7c65efcULL);
    assert(obj.formatted() == "07C6-5EFC");
  }

  // Case 4
  {
    std::cout << "  Testing uchrono64s with 2020-01-01T00:00:00Z" << std::endl;
    Persona p(0, 0U, 0, 0U, 0U);
    auto tmp = UChrono64s::from_iso_string("2020-01-01T00:00:00Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 0ULL, 0ULL, p);
    assert(obj.value == 0x0ULL);
    assert(obj.formatted() == "0000-0000-0000-0000");
  }

  // Case 5
  {
    std::cout << "  Testing uchrono64s with 2292-03-15T12:56:31Z" << std::endl;
    Persona p(0, 0U, 0, 0U, 0U);
    auto tmp = UChrono64s::from_iso_string("2292-03-15T12:56:31Z");
    auto obj = UChrono64s::from_persona_units(tmp.get_timestamp(), 34937ULL,
                                              3309ULL, p);
    assert(obj.value == 0xffffffffffffffffULL);
    assert(obj.formatted() == "FFFF-FFFF-FFFF-FFFF");
  }

  // Case 6
  {
    std::cout << "  Testing uchrono64s with 2156-01-01T00:00:00Z" << std::endl;
    Persona p(7, 1234U, 12, 5678U, 100U);
    auto tmp = UChrono64s::from_iso_string("2156-01-01T00:00:00Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 100ULL, 50ULL, p);
    assert(obj.value == 0x7fe76ec0620332d4ULL);
    assert(obj.formatted() == "7FE7-6EC0-6203-32D4");
  }

  // Case 7
  {
    std::cout << "  Testing uchrono64s with 2020-01-01T00:01:40Z" << std::endl;
    Persona p(7, 7495657U, 7, 7495657U, 0U);
    auto tmp = UChrono64s::from_iso_string("2020-01-01T00:01:40Z");
    auto obj =
        UChrono64s::from_persona_units(tmp.get_timestamp(), 221ULL, 983ULL, p);
    assert(obj.value == 0x3200000000ULL);
    assert(obj.formatted() == "0000-0032-0000-0000");
  }

  std::cout << "Running Error Cases..." << std::endl;

  {
    std::cout << "  Testing error case: underflow_2020" << std::endl;
    try {
      Chrono64s::from_iso_string("2019-12-31T23:59:59Z");
      assert(false && "Should have thrown");
    } catch (const ChronoError &e) {
      std::string msg = e.what();
      if (msg.find("Timestamp underflow: Date is before Epoch (2020-01-01)") ==
          std::string::npos) {
        std::cerr << "Expected error containing: Timestamp underflow: Date is "
                     "before Epoch (2020-01-01), but got: "
                  << msg << std::endl;
        assert(false);
      }
    }
  }

  std::cout << "\nALL CROSS-PLATFORM JSON TESTS PASSED!" << std::endl;
  return 0;
}
