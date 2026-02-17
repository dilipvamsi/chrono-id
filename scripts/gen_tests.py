import json
from datetime import datetime, timezone
import sys
import os
from typing import List, Dict, Any, Type, Optional, Tuple


def generate_exhaustive() -> None:
    """
    Refreshes the exhaustive multiplier tests in the cross-platform suite
    without overwriting manual baseline or simulation edge cases.
    """
    # Calculate project root relative to this script (scripts/gen_tests.py)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    py_src = os.path.join(project_root, "implementations/python/src")
    test_json_path = os.path.join(project_root, "tests/cross_platform_tests.json")

    sys.path.append(py_src)
    from chrono_id.core import ChronoBase, Persona, TS_COMPUTE

    # Automated Variant Discovery
    variants: List[Type[ChronoBase]] = []

    def find_subclasses(cls: Type[ChronoBase]) -> None:
        for sub in cls.__subclasses__():
            if hasattr(sub, "T_BITS") and sub.__name__ != "ChronoBasebs":
                variants.append(sub)
            find_subclasses(sub)

    find_subclasses(ChronoBase)
    variants.sort(key=lambda x: x.__name__.lower())

    # Load Existing Data to preserve manual scenarios
    existing_data: Dict[str, Any] = {}
    if os.path.exists(test_json_path):
        with open(test_json_path, "r") as f:
            existing_data = json.load(f)

    iso = "2023-05-20T10:30:00.123456Z"
    dt = datetime.fromisoformat(iso.replace("Z", "+00:00"))

    # 1. Preserve Manual Sections
    test_data: Dict[str, Any] = {
        "$schema": existing_data.get("$schema", "./cross_platform_tests.schema.json"),
        "valid_cases": [],
        "precision_checks": existing_data.get("precision_checks", []),
        "error_cases": existing_data.get("error_cases", []),
        "bit_splits": existing_data.get("bit_splits", []),
        "variant_specs": existing_data.get("variant_specs", []),
    }

    # 2. Re-generate Variant Specs (ensure correctness)
    test_data["variant_specs"] = [
        {
            "variant": v.__name__.lower(),
            "t_bits": v.T_BITS,
            "n_bits": v.N_BITS,
            "s_bits": v.S_BITS,
        }
        for v in variants
    ]

    # 3. Generate Exhaustive Multiplier Test
    exhaustive_case: Dict[str, Any] = {
        "comment": "Exhaustive Multiplier Test (All 128 Indices Exercise)",
        "iso": iso,
        "variants": [],
    }

    for i in range(128):
        v = variants[i % len(variants)]
        p = Persona(v.S_BITS)
        p.node_id = 0x123 + i
        p.node_idx = i
        p.node_salt = 0x456 + i
        p.seq_idx = (i + 50) % 128
        p.seq_salt = 0x321 + i
        p.seq_offset = (0x789 + i) & ((1 << v.S_BITS) - 1) if v.S_BITS > 0 else 0

        seq = (456 + i) & ((1 << v.S_BITS) - 1) if v.S_BITS > 0 else 0
        obj = v.from_parts(dt, p.node_id, seq, persona=p)

        exhaustive_case["variants"].append(
            {
                "name": v.__name__.lower(),
                "node_id": p.node_id,
                "seq": seq,
                "node_idx": p.node_idx,
                "node_salt": p.node_salt,
                "seq_idx": p.seq_idx,
                "seq_salt": p.seq_salt,
                "seq_offset": p.seq_offset,
                "expected_hex": hex(int(obj)),
                "expected_str": str(obj),
            }
        )

    # 4. Merge Logic
    # We keep the new exhaustive case first
    test_data["valid_cases"].append(exhaustive_case)

    # Then we append any manual cases from existing data.
    # We identify the old exhaustive test by its comment and skip it.
    if "valid_cases" in existing_data:
        for case in existing_data["valid_cases"]:
            # Skip the automated exhaustive case (identified by its unique comment prefix)
            comment = case.get("comment", "")
            if "Exhaustive Multiplier Test" in comment:
                continue
            # Keep everything else (manual baselines, boundary checks, etc.)
            test_data["valid_cases"].append(case)

    # 5. Final Write
    with open(test_json_path, "w") as f:
        json.dump(test_data, f, indent=4)

    print(f"Successfully updated {test_json_path}")


if __name__ == "__main__":
    generate_exhaustive()
