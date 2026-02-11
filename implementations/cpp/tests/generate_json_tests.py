import json
import os

def generate():
    json_path = os.path.join(os.path.dirname(__file__), "../../../tests/cross_platform_tests.json")
    with open(json_path, 'r') as f:
        data = json.load(f)

    cpp_source = """
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "chrono_id.hpp"

using namespace chrono_id;

int main() {
    std::cout << "Running Cross-Platform C++ JSON Tests..." << std::endl;
"""

    # Valid Cases
    for case in data["valid_cases"]:
        iso = case["iso"]
        for v in case["variants"]:
            v_name = v["name"]
            # Convert name to C++ casing (e.g., uchrono64 -> UChrono64, chrono64 -> Chrono64)
            cpp_type = v_name
            if v_name.startswith('uchrono'):
                cpp_type = 'UChrono' + v_name[7:]
            elif v_name.startswith('chrono'):
                cpp_type = 'Chrono' + v_name[6:]

            cpp_source += f'\n    {{\n        std::cout << "Testing {v_name} with {iso}" << std::endl;\n'
            cpp_source += f'        auto obj = {cpp_type}::from_iso_string("{iso}", 0ULL);\n'

            if "expected_timestamp" in v:
                cpp_source += f'        assert(obj.get_timestamp() == {v["expected_timestamp"]}ULL);\n'
            if "expected_iso" in v:
                cpp_source += f'        assert(obj.to_iso_string() == "{v["expected_iso"]}");\n'
            cpp_source += "    }\n"

    # Error Cases
    for case in data["error_cases"]:
        name = case["name"]
        input_val = case["input"]
        expected_err = case["expected_error"]
        v_name = case.get("variant", "chrono64ms")
        cpp_type = v_name
        if v_name.startswith('uchrono'):
            cpp_type = 'UChrono' + v_name[7:]
        elif v_name.startswith('chrono'):
            cpp_type = 'Chrono' + v_name[6:]

        cpp_source += f'\n    {{\n        std::cout << "Testing error case: {name}" << std::endl;\n'
        cpp_source += "        try {\n"
        if input_val is None:
             cpp_source += f'            {cpp_type}::from_iso_cstring(nullptr);\n'
        else:
             cpp_source += f'            {cpp_type}::from_iso_string("{input_val}");\n'
        cpp_source += "            assert(false && \"Should have thrown\");\n"
        cpp_source += "        } catch (const ChronoError& e) {\n"
        cpp_source += f'            std::string msg = e.what();\n'
        cpp_source += f'            assert(msg == "{expected_err}");\n'
        cpp_source += "        }\n"
        cpp_source += "    }\n"

    cpp_source += """
    std::cout << "All Cross-Platform JSON Tests Passed!" << std::endl;
    return 0;
}
"""

    with open(os.path.join(os.path.dirname(__file__), "test_json.cpp"), "w") as f:
        f.write(cpp_source)

if __name__ == "__main__":
    generate()
