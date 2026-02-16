"""
ChronoID Weyl Multiplier Generator (Language Portability Tool)

This script generates source code constants for various programming languages
based on the 128 prime Weyl multipliers stored in 'weyl-multipliers.txt'.
It ensures bit-parity and correct syntax for distributed identifier implementations.
"""

import sys
import os
import argparse

def generate_source(mults, lang):
    """
    Dispatches the correct code emitter based on the target language.
    """
    lang = lang.lower()
    count = len(mults)

    def emit_array(header, footer, item_suffix="", items_per_line=4):
        """
        Generic helper to format a 1D array/list into a multi-line source block.
        """
        lines = [header]
        for i in range(0, count, items_per_line):
            batch = [m + item_suffix for m in mults[i:i+items_per_line]]
            line = "    " + ", ".join(batch) + ","
            lines.append(line)
        lines.append(footer)
        return "\n".join(lines)

    # Rust: pub const WEYL_MULTIPLIERS: [u64; 128] = [...]
    if lang == "rust":
        return emit_array(f"pub const WEYL_MULTIPLIERS: [u64; {count}] = [", "];")

    # JavaScript/TypeScript: export const WEYL_MULTIPLIERS = [1n, ...] (BigInt)
    elif lang in ["js", "javascript", "ts", "typescript"]:
        return emit_array("export const WEYL_MULTIPLIERS = [", "];", item_suffix="n")

    # C/C++: const uint64_t WEYL_MULTIPLIERS[128] = {1ULL, ...}
    elif lang in ["cpp", "c++", "c"]:
        return emit_array(f"const uint64_t WEYL_MULTIPLIERS[{count}] = {{", "};", item_suffix="ULL")

    # Python: WEYL_MULTIPLIERS = [...]
    elif lang == "python":
        return emit_array("WEYL_MULTIPLIERS = [", "]")

    # Odin: WEYL_MULTIPLIERS: [128]u64 = {...}
    elif lang == "odin":
        return emit_array(f"WEYL_MULTIPLIERS: [{count}]u64 = {{", "}")

    # Go: var WeylMultipliers = [128]uint64{...}
    elif lang == "go":
        return emit_array(f"var WeylMultipliers = [{count}]uint64{{", "}")

    else:
        return f"// Unsupported language: {lang}\n" + "\n".join(mults)

def main():
    """
    Main CLI entry point with robust argument handling.
    """
    supported_langs = ["rust", "js", "ts", "javascript", "typescript", "cpp", "c++", "c", "python", "odin", "go"]

    parser = argparse.ArgumentParser(
        description="ChronoID Weyl Multiplier Generator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 weyl-lang-gen.py rust implementation/weyl.rs
  python3 weyl-lang-gen.py js multipliers.js --source custom_mults.txt
"""
    )

    parser.add_argument("language", choices=supported_langs, help="Target language")
    parser.add_argument("output", help="Output file path")
    parser.add_argument("--source", help="Path to multipliers text file (default: weyl-multipliers.txt at script root)")

    args = parser.parse_args()

    # Determine source of truth
    if args.source:
        multipliers_path = args.source
    else:
        # Default: Locate 'weyl-multipliers.txt' relative to this script's directory
        script_dir = os.path.dirname(os.path.realpath(__file__))
        multipliers_path = os.path.join(script_dir, "weyl-multipliers.txt")

    # 1. Check if source exists
    if not os.path.exists(multipliers_path):
        print(f"❌ Error: Multipliers source not found at: {multipliers_path}")
        sys.exit(1)

    # 2. Read and validate data
    try:
        with open(multipliers_path, "r") as f:
            mults = [line.strip() for line in f if line.strip()]

        # Enforce Diamond Standard (128 multipliers)
        if len(mults) != 128:
            print(f"⚠️ Warning: Found {len(mults)} multipliers. Diamond Standard requires exactly 128.")
            # We still proceed but warn.
    except Exception as e:
        print(f"❌ Error reading source: {e}")
        sys.exit(1)

    # 3. Ensure output directory exists
    out_dir = os.path.dirname(args.output)
    if out_dir and not os.path.exists(out_dir):
        print(f"📁 Creating output directory: {out_dir}")
        os.makedirs(out_dir, exist_ok=True)

    # 4. Generate and Write
    try:
        output = generate_source(mults, args.language)
        with open(args.output, "w") as f:
            f.write(output + "\n")
        print(f"✅ Success: Generated {len(mults)} multipliers for {args.language} -> {args.output}")
    except Exception as e:
        print(f"❌ Error writing output: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
