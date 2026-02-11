
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../include/chrono_id.hpp"

using namespace chrono_id;

int main() {
    std::cout << "Running Cross-Platform C++ JSON Tests..." << std::endl;

    {
        std::cout << "Testing chrono64 with 1970-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono64::from_iso_string("1970-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono64 with 1970-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono64::from_iso_string("1970-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono64ms with 1970-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono64ms::from_iso_string("1970-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono64ms with 1970-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono64ms::from_iso_string("1970-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono32 with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono32::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono32 with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono32::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono32h with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono32h::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono32h with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono32h::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono32m with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono32m::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono32m with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono32m::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono32w with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = Chrono32w::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing uchrono32w with 2000-01-01T00:00:00Z" << std::endl;
        auto obj = UChrono32w::from_iso_string("2000-01-01T00:00:00Z", 0ULL);
        assert(obj.get_timestamp() == 0ULL);
    }

    {
        std::cout << "Testing chrono64ms with 2023-05-20T10:30:00.123Z" << std::endl;
        auto obj = Chrono64ms::from_iso_string("2023-05-20T10:30:00.123Z", 0ULL);
        assert(obj.to_iso_string() == "2023-05-20T10:30:00.123Z");
    }

    {
        std::cout << "Testing chrono64us with 2023-05-20T10:30:00.123456Z" << std::endl;
        auto obj = Chrono64us::from_iso_string("2023-05-20T10:30:00.123456Z", 0ULL);
        assert(obj.to_iso_string() == "2023-05-20T10:30:00.123456Z");
    }

    {
        std::cout << "Testing error case: null_input" << std::endl;
        try {
            Chrono64ms::from_iso_cstring(nullptr);
            assert(false && "Should have thrown");
        } catch (const ChronoError& e) {
            std::string msg = e.what();
            assert(msg == "Input string is null");
        }
    }

    {
        std::cout << "Testing error case: invalid_format" << std::endl;
        try {
            Chrono64ms::from_iso_string("not-a-date");
            assert(false && "Should have thrown");
        } catch (const ChronoError& e) {
            std::string msg = e.what();
            assert(msg == "Invalid ISO 8601 format");
        }
    }

    {
        std::cout << "Testing error case: underflow_1970" << std::endl;
        try {
            Chrono64::from_iso_string("1960-01-01T00:00:00Z");
            assert(false && "Should have thrown");
        } catch (const ChronoError& e) {
            std::string msg = e.what();
            assert(msg == "Timestamp underflow: Date is before Unix Epoch (1970-01-01)");
        }
    }

    {
        std::cout << "Testing error case: underflow_2000" << std::endl;
        try {
            Chrono32::from_iso_string("1999-12-31T23:59:59Z");
            assert(false && "Should have thrown");
        } catch (const ChronoError& e) {
            std::string msg = e.what();
            assert(msg == "Timestamp underflow: Date is before Epoch (32-bit types require 2000-01-01 or later)");
        }
    }

    std::cout << "All Cross-Platform JSON Tests Passed!" << std::endl;
    return 0;
}
