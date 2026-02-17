import json
import os


def generate():
    # Paths are relative to this script
    current_dir = os.path.dirname(os.path.abspath(__file__))
    json_path = os.path.join(current_dir, "../../../tests/cross_platform_tests.json")
    output_cpp = os.path.join(current_dir, "test_json.cpp")

    with open(json_path, "r") as f:
        data = json.load(f)

    cpp_source = """#include "../include/chrono_id.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace chrono_id;

int main() {
    std::cout << "Running Cross-Platform C++ JSON Tests (Diamond Standard)..." << std::endl;
"""

    # Valid Cases
    for case_idx, case in enumerate(data.get("valid_cases", [])):
        iso = case["iso"]
        comment = case.get("comment", f"Case {case_idx}")

        cpp_source += f"\n    // {comment}\n"
        for v in case["variants"]:
            v_name = v["name"]
            # Convert name to C++ casing (e.g., uchrono64s -> UChrono64s)
            cpp_type = v_name[0].upper() + v_name[1:]
            if v_name.startswith("uchrono"):
                cpp_type = "UChrono" + v_name[7:]
            elif v_name.startswith("chrono"):
                cpp_type = "Chrono" + v_name[6:]

            cpp_source += f"    {{\n"
            cpp_source += (
                f'        std::cout << "  Testing {v_name} with {iso}" << std::endl;\n'
            )

            # Persona parameters
            node_id = v.get("node_id", 0)
            seq = v.get("seq", 0)
            n_idx = v.get("node_idx", 0)
            n_salt = v.get("node_salt", 0)
            s_idx = v.get("seq_idx", 0)
            s_salt = v.get("seq_salt", 0)
            s_off = v.get("seq_offset", 0)

            cpp_source += f"        Persona p({n_idx}, {n_salt}U, {s_idx}, {s_salt}U, {s_off}U);\n"
            # Optimization: Use from_persona_units and get_timestamp() to avoid system_clock overflow at 2262
            cpp_source += f'        auto tmp = {cpp_type}::from_iso_string("{iso}");\n'
            cpp_source += f"        auto obj = {cpp_type}::from_persona_units(tmp.get_timestamp(), {node_id}ULL, {seq}ULL, p);\n"

            # Assertions
            if "expected_hex" in v:
                expected_hex = v["expected_hex"]
                cpp_source += f"        assert(obj.value == {expected_hex}ULL);\n"
            if "expected_str" in v:
                expected_str = v["expected_str"]
                cpp_source += f'        assert(obj.formatted() == "{expected_str}");\n'
            if "expected_iso" in v:
                expected_iso = v["expected_iso"]
                cpp_source += (
                    f'        assert(obj.to_iso_string() == "{expected_iso}");\n'
                )

            cpp_source += "    }\n"

    # Error Cases
    cpp_source += '\n    std::cout << "Running Error Cases..." << std::endl;\n'
    for case in data.get("error_cases", []):
        name = case["name"]
        input_val = case["input"]
        expected_err = case["expected_error"]
        v_name = case.get("variant", "chrono64ms")
        cpp_type = v_name[0].upper() + v_name[1:]
        if v_name.startswith("uchrono"):
            cpp_type = "UChrono" + v_name[7:]
        elif v_name.startswith("chrono"):
            cpp_type = "Chrono" + v_name[6:]

        cpp_source += f'\n    {{\n        std::cout << "  Testing error case: {name}" << std::endl;\n'
        cpp_source += "        try {\n"
        if input_val is None:
            cpp_source += f"            {cpp_type}::from_iso_cstring(nullptr);\n"
        else:
            cpp_source += f'            {cpp_type}::from_iso_string("{input_val}");\n'
        cpp_source += '            assert(false && "Should have thrown");\n'
        cpp_source += "        } catch (const ChronoError& e) {\n"
        cpp_source += "            std::string msg = e.what();\n"
        # C++ error messages might differ slightly, but we try to match or adapt
        cpp_source += (
            f'            if (msg.find("{expected_err}") == std::string::npos) {{\n'
        )
        cpp_source += f'                std::cerr << "Expected error containing: {expected_err}, but got: " << msg << std::endl;\n'
        cpp_source += f"                assert(false);\n"
        cpp_source += f"            }}\n"
        cpp_source += "        }\n"
        cpp_source += "    }\n"

    cpp_source += """
    std::cout << "\\nALL CROSS-PLATFORM JSON TESTS PASSED!" << std::endl;
    return 0;
}
"""

    with open(output_cpp, "w") as f:
        f.write(cpp_source)
    print(f"Generated {output_cpp}")


if __name__ == "__main__":
    generate()
