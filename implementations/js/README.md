# Chrono-ID JavaScript Implementation

High-performance, K-Sortable identifiers for Node.js and Browser environments.

## ✨ Features

- **Node.js & Browser Support**: Automatically detects environment and uses relevant Crypto APIs (Node `crypto` or Web Crypto).
- **BigInt Core**: Uses `BigInt` for 64-bit precision safety.
- **Bit-Parity**: 100% parity with Python and C++ implementations.
- **Hyphenated Hex**: IDs format as `XXXX-XXXX` (32-bit) or `XXXX-XXXX-XXXX-XXXX` (64-bit).
- **Lightweight**: Zero external dependencies.
- **UTC Aware**: All timestamps are handled in UTC.

## 🚀 Usage

### Installation

```bash
# Clone or copy src/index.js into your project
```

### Basic Usage (Node.js)

```javascript
const { Chrono64ms, Chrono32y } = require("./src/index");

// 1. Generate a new ID (64-bit millisecond precision)
const id = new Chrono64ms();
console.log(id.toString()); // "C67F-32A1-001F-4BCC" (Formatted Hex)
console.log(id.getTime()); // Date object (UTC)
console.log(id.toBigInt()); // 14303492038421033n

// 2. Generate for a specific date (32-bit Year precision)
const oldId = Chrono32y.fromTime(new Date("2022-01-01T00:00:00Z"));
console.log(oldId.toString()); // "0202-0000"

// 3. Parse from formatted string (Bidirectional)
const parsed = Chrono64ms.fromFormat("C67F-32A1-001F-4BCC");
assert(id.toBigInt() === parsed.toBigInt());
```

### Browser Usage

```html
<script src="path/to/chrono-id/src/index.js"></script>
<script>
  // Classes are available on the global ChronoID object
  const id = new ChronoID.Chrono64s();
  console.log(id.formatted()); // e.g. "C67F-32A1-001F-4BCC"
</script>
```

## 🧪 Testing

Uses Node.js built-in `test` runner and `assert` module.

```bash
# Run logic tests
node test/test.js

# Run cross-platform parity tests
node test/test-json.js
```

## 🔒 Security

Randomness is provided by:

1. `crypto.getRandomValues()` (Browser/Web Crypto)
2. `require('crypto').randomBytes()` (Node.js)
