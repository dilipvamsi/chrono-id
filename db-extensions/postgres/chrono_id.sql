-- =========================================================
--  32-BIT FAMILY (EPOCH: JAN 1, 2000)
-- =========================================================

-- 1. Day Precision (18 bits time)
CREATE OR REPLACE FUNCTION chrono32() RETURNS INTEGER AS $$
BEGIN
    -- Signed: 13 bits random
    RETURN (((EXTRACT(DAY FROM (NOW() - '2000-01-01'))::INTEGER) & x'3FFFF'::int) << 13) | (floor(random() * 8192)::int);
END; $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32() RETURNS BIGINT AS $$
BEGIN
    -- Unsigned: 14 bits random (Return BIGINT to show positive value)
    RETURN (((EXTRACT(DAY FROM (NOW() - '2000-01-01'))::BIGINT) & x'3FFFF'::bigint) << 14) | (floor(random() * 16384)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;

-- 2. Hour Precision (21 bits time)
CREATE OR REPLACE FUNCTION chrono32h() RETURNS INTEGER AS $$
BEGIN
    -- Signed: 10 bits random
    RETURN (((EXTRACT(EPOCH FROM (NOW() - '2000-01-01'))::INTEGER / 3600) & x'1FFFFF'::int) << 10) | (floor(random() * 1024)::int);
END; $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32h() RETURNS BIGINT AS $$
BEGIN
    -- Unsigned: 11 bits random
    RETURN (((EXTRACT(EPOCH FROM (NOW() - '2000-01-01'))::BIGINT / 3600) & x'1FFFFF'::bigint) << 11) | (floor(random() * 2048)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;

-- 3. Minute Precision (27 bits time)
CREATE OR REPLACE FUNCTION chrono32m() RETURNS INTEGER AS $$
BEGIN
    -- Signed: 4 bits random
    RETURN (((EXTRACT(EPOCH FROM (NOW() - '2000-01-01'))::INTEGER / 60) & x'7FFFFFF'::int) << 4) | (floor(random() * 16)::int);
END; $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32m() RETURNS BIGINT AS $$
BEGIN
    -- Unsigned: 5 bits random
    RETURN (((EXTRACT(EPOCH FROM (NOW() - '2000-01-01'))::BIGINT / 60) & x'7FFFFFF'::bigint) << 5) | (floor(random() * 32)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;


-- =========================================================
--  64-BIT FAMILY (EPOCH: JAN 1, 1970)
-- =========================================================

-- 1. Second Precision (36 bits time)
CREATE OR REPLACE FUNCTION chrono64() RETURNS BIGINT AS $$
BEGIN
    -- Signed: 27 bits random
    RETURN (((EXTRACT(EPOCH FROM NOW())::BIGINT) & x'FFFFFFFFF'::bigint) << 27) | (floor(random() * 134217728)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono64() RETURNS BIGINT AS $$
BEGIN
    -- Unsigned: 28 bits random (Note: Values > 2^63 will appear negative in Postgres)
    RETURN (((EXTRACT(EPOCH FROM NOW())::BIGINT) & x'FFFFFFFFF'::bigint) << 28) | (floor(random() * 268435456)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;

-- 2. Millisecond Precision (44 bits time)
CREATE OR REPLACE FUNCTION chrono64ms() RETURNS BIGINT AS $$
BEGIN
    -- Signed: 19 bits random
    RETURN (((floor(EXTRACT(EPOCH FROM NOW()) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 19) | (floor(random() * 524288)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono64ms() RETURNS BIGINT AS $$
BEGIN
    -- Unsigned: 20 bits random (Note: Values > 2^63 will appear negative in Postgres)
    RETURN (((floor(EXTRACT(EPOCH FROM NOW()) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 20) | (floor(random() * 1048576)::bigint);
END; $$ LANGUAGE plpgsql VOLATILE;


-- =========================================================
--  RETRIEVAL FUNCTIONS (Get Time from ID)
-- =========================================================

-- 32-bit Retrievals
CREATE OR REPLACE FUNCTION chrono32_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP(946684800 + (id >> 13) * 86400);
END; $$ LANGUAGE plpgsql IMMUTABLE;

CREATE OR REPLACE FUNCTION uchrono32_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP(946684800 + (id >> 14) * 86400);
END; $$ LANGUAGE plpgsql IMMUTABLE;

CREATE OR REPLACE FUNCTION chrono32h_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP(946684800 + (id >> 10) * 3600);
END; $$ LANGUAGE plpgsql IMMUTABLE;

CREATE OR REPLACE FUNCTION chrono32m_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP(946684800 + (id >> 4) * 60);
END; $$ LANGUAGE plpgsql IMMUTABLE;

-- 64-bit Retrievals
CREATE OR REPLACE FUNCTION chrono64_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP(id >> 27);
END; $$ LANGUAGE plpgsql IMMUTABLE;

CREATE OR REPLACE FUNCTION chrono64ms_get_time(id BIGINT) RETURNS TIMESTAMP AS $$
BEGIN
    RETURN TO_TIMESTAMP((id >> 19) / 1000.0);
END; $$ LANGUAGE plpgsql IMMUTABLE;

-- Example Usage:
-- SELECT chrono64ms() as id, chrono64ms_get_time(chrono64ms()) as time;
