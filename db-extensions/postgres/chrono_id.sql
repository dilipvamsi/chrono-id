-- =========================================================
--  Chrono ID - K-Sortable, Time-Ordered Unique Identifiers (32-bit & 64-bit).
-- =========================================================

-- =========================================================
--  32-BIT FAMILY (EPOCH: JAN 1, 2000)
-- =========================================================

-- 1. Day Precision (18 bits time)
-- 18 bits Day = ~716 years | 13/14 bits Random
CREATE OR REPLACE FUNCTION chrono32() RETURNS INTEGER AS $$
    SELECT (
        ((EXTRACT(DAY FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::INTEGER) & x'3FFFF'::int) << 13 -- 18 bits Day shifted by 13
    ) | (floor(random() * 8192)::int); -- 13 bits Random (2^13 = 8192)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono32() RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(DAY FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::BIGINT) & x'3FFFF'::bigint) << 14 -- 18 bits Day shifted by 14
    ) | (floor(random() * 16384)::bigint); -- 14 bits Random (2^14 = 16384)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- 2. Hour Precision (21 bits time)
-- 21 bits Hour = ~239 years | 10/11 bits Random
CREATE OR REPLACE FUNCTION chrono32h() RETURNS INTEGER AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::INTEGER / 3600) & x'1FFFFF'::int) << 10 -- 21 bits Hour shifted by 10
    ) | (floor(random() * 1024)::int); -- 10 bits Random (2^10 = 1024)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono32h() RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::BIGINT / 3600) & x'1FFFFF'::bigint) << 11 -- 21 bits Hour shifted by 11
    ) | (floor(random() * 2048)::bigint); -- 11 bits Random (2^11 = 2048)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- 3. Minute Precision (27 bits time)
-- 27 bits Minute = ~255 years | 4/5 bits Random
CREATE OR REPLACE FUNCTION chrono32m() RETURNS INTEGER AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::INTEGER / 60) & x'7FFFFFF'::int) << 4 -- 27 bits Minute shifted by 4
    ) | (floor(random() * 16)::int); -- 4 bits Random (2^4 = 16)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono32m() RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp))::BIGINT / 60) & x'7FFFFFF'::bigint) << 5 -- 27 bits Minute shifted by 5
    ) | (floor(random() * 32)::bigint); -- 5 bits Random (2^5 = 32)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- 4. Week Precision (14 bits time)
-- 14 bits Week = ~314 years | 17/18 bits Random
CREATE OR REPLACE FUNCTION chrono32w() RETURNS INTEGER AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp)) / 604800)::INTEGER) & x'3FFF'::int) << 17 -- 14 bits Week shifted by 17
    ) | (floor(random() * 131072)::int); -- 17 bits Random (2^17 = 131072)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono32w() RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc' - '2000-01-01'::timestamp)) / 604800)::BIGINT) & x'3FFF'::bigint) << 18 -- 14 bits Week shifted by 18
    ) | (floor(random() * 262144)::bigint); -- 18 bits Random (2^18 = 262144)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- =========================================================
--  64-BIT FAMILY (EPOCH: JAN 1, 1970)
-- =========================================================

-- 1. Second Precision (36 bits time)
-- 36 bits Second = ~2177 years | 27/28 bits Random
CREATE OR REPLACE FUNCTION chrono64() RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc'))::BIGINT) & x'FFFFFFFFF'::bigint) << 27 -- 36 bits Second shifted by 27
    ) | (floor(random() * 134217728)::bigint); -- 27 bits Random (2^27)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono64() RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc'))::BIGINT) & x'FFFFFFFFF'::bigint) << 28 -- 36 bits Second shifted by 28
    ) | (floor(random() * 268435456)::bigint); -- 28 bits Random (2^28)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- 2. Millisecond Precision (44 bits time)
-- 44 bits Millisecond = ~557 years | 19/20 bits Random
CREATE OR REPLACE FUNCTION chrono64ms() RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc')) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 19 -- 44 bits MS shifted by 19
    ) | (floor(random() * 524288)::bigint); -- 19 bits Random (2^19)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono64ms() RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc')) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 20 -- 44 bits MS shifted by 20
    ) | (floor(random() * 1048576)::bigint); -- 20 bits Random (2^20)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

-- 3. Microsecond Precision (54 bits time)
-- 54 bits Microsecond = ~570 years | 9/10 bits Random
CREATE OR REPLACE FUNCTION chrono64us() RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc')) * 1000000)::BIGINT) & x'3FFFFFFFFFFFFF'::bigint) << 9 -- 54 bits US shifted by 9
    ) | (floor(random() * 512)::bigint); -- 9 bits Random (2^9)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono64us() RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (now() AT TIME ZONE 'utc')) * 1000000)::BIGINT) & x'3FFFFFFFFFFFFF'::bigint) << 10 -- 54 bits US shifted by 10
    ) | (floor(random() * 1024)::bigint); -- 10 bits Random (2^10)
$$ LANGUAGE sql VOLATILE PARALLEL SAFE;


-- =========================================================
--  RETRIEVAL FUNCTIONS (Get Time from ID)
-- =========================================================

-- 32-bit Retrievals
-- Constants: 946684800 is Unix time for 2000-01-01
CREATE OR REPLACE FUNCTION chrono32_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 13) * 86400); -- Unshift 13, convert 86400 sec/day
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono32_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 14) * 86400); -- Unshift 14
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chrono32h_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 10) * 3600); -- Unshift 10, convert 3600 sec/hour
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chrono32m_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 4) * 60); -- Unshift 4, convert 60 sec/min
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

-- 64-bit Retrievals
CREATE OR REPLACE FUNCTION chrono64_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(id >> 27); -- Unshift 27 to get seconds
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chrono64ms_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP((id >> 19) / 1000.0); -- Unshift 19, convert ms to sec
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chrono64us_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP((id >> 9) / 1000000.0); -- Unshift 9, convert us to sec
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION uchrono64us_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP((id >> 10) / 1000000.0); -- Unshift 10, convert us to sec
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

-- Example Usage:
-- SELECT chrono64ms() as id, chrono64ms_get_time(chrono64ms()) as time;
CREATE OR REPLACE FUNCTION uchrono32w_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 18) * 604800); -- Unshift 18, convert 604800 sec/week
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chrono32w_get_time(id BIGINT) RETURNS TIMESTAMPTZ AS $$
    SELECT TO_TIMESTAMP(946684800 + (id >> 17) * 604800); -- Unshift 17
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

-- =========================================================
--  PARSING FUNCTIONS (ISO 8601 to ID)
-- =========================================================

-- Helper for common ISO parsing and epoch checks
CREATE OR REPLACE FUNCTION chrono_id_parse_iso(iso TEXT, epoch_ts TIMESTAMPTZ) RETURNS TIMESTAMPTZ AS $$
DECLARE
    dt TIMESTAMPTZ;
BEGIN
    IF iso IS NULL THEN
        RAISE EXCEPTION 'Input string is null';
    END IF;

    BEGIN
        dt := iso::TIMESTAMPTZ;
    EXCEPTION WHEN OTHERS THEN
        RAISE EXCEPTION 'Invalid ISO 8601 format';
    END;

    IF dt < epoch_ts THEN
        IF epoch_ts = '2000-01-01T00:00:00Z'::TIMESTAMPTZ THEN
            RAISE EXCEPTION 'Timestamp underflow: Date is before Epoch (32-bit types require 2000-01-01 or later)';
        ELSE
            RAISE EXCEPTION 'Timestamp underflow: Date is before Unix Epoch (1970-01-01)';
        END IF;
    END IF;
    RETURN dt;
END;
$$ LANGUAGE plpgsql IMMUTABLE;

-- Specialized parsing functions for each variant

CREATE OR REPLACE FUNCTION chrono32_from_iso(iso TEXT) RETURNS INTEGER AS $$
    SELECT (
        ((EXTRACT(DAY FROM (chrono_id_parse_iso(iso, '2000-01-01Z')::timestamp - '2000-01-01'::timestamp))::INTEGER) & x'3FFFF'::int) << 13
    ) | (floor(random() * 8192)::int);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((EXTRACT(DAY FROM (chrono_id_parse_iso(iso, '2000-01-01Z')::timestamp - '2000-01-01'::timestamp))::BIGINT) & x'3FFFF'::bigint) << 14
    ) | (floor(random() * 16384)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono32h_from_iso(iso TEXT) RETURNS INTEGER AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 3600)::INTEGER) & x'1FFFFF'::int) << 10
    ) | (floor(random() * 1024)::int);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32h_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 3600)::BIGINT) & x'1FFFFF'::bigint) << 11
    ) | (floor(random() * 2048)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono32m_from_iso(iso TEXT) RETURNS INTEGER AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 60)::INTEGER) & x'7FFFFFF'::int) << 4
    ) | (floor(random() * 16)::int);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32m_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 60)::BIGINT) & x'7FFFFFF'::bigint) << 5
    ) | (floor(random() * 32)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono32w_from_iso(iso TEXT) RETURNS INTEGER AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 604800)::INTEGER) & x'3FFF'::int) << 17
    ) | (floor(random() * 131072)::int);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono32w_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM (chrono_id_parse_iso(iso, '2000-01-01Z') - '2000-01-01Z'::timestamptz)) / 604800)::BIGINT) & x'3FFF'::bigint) << 18
    ) | (floor(random() * 262144)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono64_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')))::BIGINT) & x'FFFFFFFFF'::bigint) << 27
    ) | (floor(random() * 134217728)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono64_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')))::BIGINT) & x'FFFFFFFFF'::bigint) << 28
    ) | (floor(random() * 268435456)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono64ms_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 19
    ) | (floor(random() * 524288)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono64ms_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')) * 1000)::BIGINT) & x'FFFFFFFFFFF'::bigint) << 20
    ) | (floor(random() * 1048576)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION chrono64us_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')) * 1000000)::BIGINT) & x'3FFFFFFFFFFFFF'::bigint) << 9
    ) | (floor(random() * 512)::bigint);
$$ LANGUAGE sql VOLATILE;

CREATE OR REPLACE FUNCTION uchrono64us_from_iso(iso TEXT) RETURNS BIGINT AS $$
    SELECT (
        ((floor(EXTRACT(EPOCH FROM chrono_id_parse_iso(iso, '1970-01-01Z')) * 1000000)::BIGINT) & x'3FFFFFFFFFFFFF'::bigint) << 10
    ) | (floor(random() * 1024)::bigint);
$$ LANGUAGE sql VOLATILE;
