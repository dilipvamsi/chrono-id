# Chrono-ID JavaScript Implementation

High-performance, K-Sortable identifiers for Node.js and Browser environments.

## ✨ Features
- **Node.js & Browser Support**: Automatically detects environment and uses relevant Crypto APIs (Node `crypto` or Web Crypto).
- **BigInt Core**: Uses `BigInt` for 64-bit precision safety.
- **Lightweight**: Zero external dependencies.
- **UTC Aware**: All timestamps are handled in UTC.

## 🚀 Usage

### Node.js
```javascript
const ChronoID = require('./src/index');

const id = new ChronoID.Chrono64ms();
console.log(id.toString());
console.log(id.getTime()); // Date object (UTC)
```

### Browser
```html
<script src="path/to/chrono-id/src/index.js"></script>
<script>
    const id = new ChronoID.Chrono32();
    console.log(id.toString());
</script>
```

## 🧪 Testing

Uses Node.js built-in `test` runner and `assert` module.

```bash
# Run tests
node test/test.js

# Run with coverage (Node 20+)
node --test --experimental-test-coverage test/test.js
```

## 🔒 Security
Randomness is provided by:
1. `crypto.getRandomValues()` (Browser/Web Crypto)
2. `require('crypto').randomBytes()` (Node.js)
3. `Math.random()` (Fallback - only if system crypto is unavailable)
