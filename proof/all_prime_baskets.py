import math
import csv

class PrimeManager:
    def __init__(self):
        self.primes = [2, 3]
        self.sieve_limit = 3

    def _extend_primes(self, needed_limit):
        if needed_limit <= self.sieve_limit:
            return
        candidate = self.sieve_limit + 2
        while candidate <= needed_limit:
            is_prime_cand = True
            limit_sqrt = int(candidate**0.5)
            for p in self.primes:
                if p > limit_sqrt: break
                if candidate % p == 0:
                    is_prime_cand = False
                    break
            if is_prime_cand: self.primes.append(candidate)
            candidate += 2
        self.sieve_limit = self.primes[-1]

    def is_prime(self, n):
        if n <= 1: return False
        if n <= 3: return True
        if n % 2 == 0: return False
        limit = int(n**0.5)
        if limit > self.sieve_limit: self._extend_primes(limit)
        for p in self.primes:
            if p > limit: break
            if n % p == 0: return False
        return True

prime_manager = PrimeManager()

def get_quality_stats(target, prime):
    dist = abs(target - prime)
    if target == 0: return 0.0, "N/A"
    error_pct = (dist / target) * 100.0
    
    if error_pct < 0.0001: label = "Perfect"
    elif error_pct < 0.1:  label = "Excellent"
    elif error_pct < 1.0:  label = "Very Good"
    elif error_pct < 2.5:  label = "Good"
    elif error_pct < 5.0:  label = "Fair"
    else:                  label = "Poor"
    
    return error_pct, label

def find_golden_primes(bits, max_results=50):
    if bits <= 1: return 0, []

    phi = (1 + 5 ** 0.5) / 2
    mod = 1 << bits
    target = int(mod * (phi - 1))
    min_val = 2
    max_val = (1 << bits) - 1

    found_primes = []
    offset = 0

    while len(found_primes) < max_results:
        candidates = []
        if offset == 0: candidates.append(target)
        else:
            candidates.append(target - offset)
            candidates.append(target + offset)

        if (target - offset < min_val) and (target + offset > max_val): break

        for cand in candidates:
            if cand < min_val or cand > max_val: continue
            if cand > 2 and cand % 2 == 0: continue

            if prime_manager.is_prime(cand):
                dist = abs(cand - target)
                err_pct, label = get_quality_stats(target, cand)
                found_primes.append({
                    "prime": cand,
                    "hex": hex(cand),
                    "dist": dist,
                    "error": err_pct,
                    "quality": label
                })
        offset += 1

    found_primes.sort(key=lambda x: x['dist'])
    return target, found_primes

def calculate_safe_basket_size(bits, results):
    """
    Applies the mathematical safety rules to determine usable basket size.
    Rule: Error must be < 2.5%, unless bits are very small (scarcity exception).
    """
    safe_count = 0
    for res in results:
        err = res['error']

        # Safety Logic
        is_safe = False
        if err < 2.5:
            is_safe = True
        elif safe_count < 2:
            # 4-bit Exception: Allow top 3 even if error is high (22%)
            # because 1 prime is too predictable.
            is_safe = True

        if is_safe:
            safe_count += 1
        else:
            # As soon as we hit an unsafe prime, we stop counting for the basket
            # because the list is sorted by quality.
            break

    return safe_count

# --- CONFIGURATION ---
# "mode": "seq" means we calculate based on Sequence Bits
# "mode": "node" means we calculate based on Node Bits

VARIANTS = [
    # -- SEQUENCE PRIMES (The "Moving Target" - Calculated using 's') --
    {"name": "Chrono64 (27b)",   "mode": "seq", "t": 36, "n": 12, "s": 15},
    {"name": "Chrono64ms (19b)", "mode": "seq", "t": 44, "n": 10, "s": 9},
    {"name": "Chrono64us (9b)",  "mode": "seq", "t": 54, "n": 0,  "s": 9},
    {"name": "UChrono64 (28b)",   "mode": "seq", "t": 36, "n": 13, "s": 15},
    {"name": "UChrono64ms (20b)", "mode": "seq", "t": 44, "n": 11, "s": 9},
    {"name": "UChrono64us (10b)", "mode": "seq", "t": 54, "n": 1,  "s": 9},
    {"name": "Chrono32w (17b)",  "mode": "seq", "t": 14, "n": 10, "s": 7},
    {"name": "Chrono32 (13b)",   "mode": "seq", "t": 18, "n": 8,  "s": 5},
    {"name": "Chrono32h (10b)",  "mode": "seq", "t": 21, "n": 6,  "s": 4},
    {"name": "Chrono32m (4b)",   "mode": "seq", "t": 27, "n": 0,  "s": 4},
    {"name": "UChrono32w (18b)",  "mode": "seq", "t": 14, "n": 11, "s": 7},
    {"name": "UChrono32 (14b)",   "mode": "seq", "t": 18, "n": 9,  "s": 5},
    {"name": "UChrono32h (11b)",  "mode": "seq", "t": 21, "n": 7,  "s": 4},
    {"name": "UChrono32m (5b)",   "mode": "seq", "t": 27, "n": 1,  "s": 4},

    # -- NODE PRIMES (Topology Hiding - Calculated using 'n') --
    {"name": "Node_13bit (UChrono64)",   "mode": "node", "t": 36, "n": 13, "s": 15},
    {"name": "Node_12bit (Chrono64)",    "mode": "node", "t": 36, "n": 12, "s": 15},
    {"name": "Node_11bit (UChrono64ms)", "mode": "node", "t": 44, "n": 11, "s": 9},
    {"name": "Node_10bit (Chrono64ms)",  "mode": "node", "t": 44, "n": 10, "s": 9},
    {"name": "Node_9bit (UChrono32)",     "mode": "node", "t": 18, "n": 9,  "s": 5},
    {"name": "Node_8bit (Chrono32)",      "mode": "node", "t": 18, "n": 8,  "s": 5},
    {"name": "Node_7bit (UChrono32h)",    "mode": "node", "t": 21, "n": 7,  "s": 4},
    {"name": "Node_6bit (Chrono32h)",     "mode": "node", "t": 21, "n": 6,  "s": 4}
]

MAX_DISPLAY = 50
OUTPUT_FILENAME = "golden_ratio_primes.csv"

print(f"Searching for primes (Calculating Safe Basket Sizes)...")
print(f"Results will be saved to: {OUTPUT_FILENAME}")
print("-" * 100)

with open(OUTPUT_FILENAME, mode='w', newline='') as file:
    writer = csv.writer(file)

    header = [
        "Variant", "Type", "Calc_Bits", "Time_Bits", "Node_Bits", "Seq_Bits",
        "Target_Phi", "Rank", "Prime_Dec", "Prime_Hex", "Dist", "Error_Pct", "Quality", "Is_Safe"
    ]
    writer.writerow(header)

    for v in VARIANTS:
        name = v["name"]

        # Determine which bits to calculate based on mode
        if v["mode"] == "seq":
            c_bits = v["s"]
            type_label = "Sequence"
        else:
            c_bits = v["n"]
            type_label = "Node"

        target, results = find_golden_primes(c_bits, max_results=MAX_DISPLAY)

        # Calculate the mathematical SAFE LIMIT (<2.5% Error)
        safe_basket_size = calculate_safe_basket_size(c_bits, results)

        print(f"Processed {name:<25} | Mode: {type_label:<8} | Bits: {c_bits:<2} | Computed Safe Basket: {safe_basket_size}")

        for rank, res in enumerate(results):
            r_num = rank + 1

            # Mark if this specific prime is within the safe basket
            is_safe_row = "YES" if r_num <= safe_basket_size else "NO"

            row = [
                name,
                type_label,
                c_bits,
                v["t"], v["n"], v["s"],
                target,
                r_num,
                res['prime'],
                res['hex'],
                res['dist'],
                f"{res['error']:.6f}",
                res['quality'],
                is_safe_row
            ]
            writer.writerow(row)

print("-" * 100)
print(f"Done! Check {OUTPUT_FILENAME}. Use the 'Computed Safe Basket' sizes for your configuration.")