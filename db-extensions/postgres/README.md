# Chrono-ID PostgreSQL Extension

Native PL/pgSQL functions for generating and decoding Chrono-IDs directly in the database.

## ✨ Features
- **Space Efficient**: Use `INTEGER` (for 32-bit) or `BIGINT` (for 64-bit) primary keys.
- **K-Sortable**: IDs generated in chronological order are sorted by the database engine.
- **Signed Safe**: Variants for signed integers prevent negative overflows.
- **Timezone Correct**: Uses `now() AT TIME ZONE 'utc'` for generation and `TIMESTAMPTZ` for retrieval.

## 🚀 Installation

Simply execute the SQL script in your database:
```bash
psql -d my_db -f chrono_id.sql
```

## 🚀 Usage

```sql
-- Use in table definition
CREATE TABLE events (
    id BIGINT PRIMARY KEY DEFAULT chrono64ms(),
    name TEXT
);

-- Extract time back
SELECT name, chrono64ms_get_time(id) FROM events;
```

## 🧪 Testing

Requires Python and `psycopg2-binary`.

```bash
# Start a local postgres (Optional)
make docker

# Run tests
make test
```

Settings can be overridden via `TEST_DATABASE_URL`:
```bash
TEST_DATABASE_URL=postgresql://user:pass@host:port/db make test
```
