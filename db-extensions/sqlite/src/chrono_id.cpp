#include <sqlite3.h>
#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1
#include "../../../implementations/cpp/include/chrono_id.hpp"
#include <cstring>
#include <stdexcept>

using namespace chrono_id;

// --- Type Aliases for Macro Concatenation (Matching Postgres Names) ---
using chrono32 = chrono_id::Chrono32;
using uchrono32 = chrono_id::UChrono32;
using chrono32h = chrono_id::Chrono32h;
using uchrono32h = chrono_id::UChrono32h;
using chrono32m = chrono_id::Chrono32m;
using uchrono32m = chrono_id::UChrono32m;
using chrono64 = chrono_id::Chrono64;
using uchrono64 = chrono_id::UChrono64;
using chrono64ms = chrono_id::Chrono64ms;
using uchrono64ms = chrono_id::UChrono64ms;
using chrono64us = chrono_id::Chrono64us;
using uchrono64us = chrono_id::UChrono64us;
using chrono32w = chrono_id::Chrono32w;
using uchrono32w = chrono_id::UChrono32w;

// --- Result Helpers ---

static void set_result(sqlite3_context *context, int32_t val) {
  sqlite3_result_int(context, val);
}
static void set_result(sqlite3_context *context, uint32_t val) {
  sqlite3_result_int64(context, static_cast<int64_t>(val));
}
static void set_result(sqlite3_context *context, int64_t val) {
  sqlite3_result_int64(context, val);
}
static void set_result(sqlite3_context *context, uint64_t val) {
  sqlite3_result_int64(context, static_cast<int64_t>(val));
}

// --- Helper to dispatch based on type string for generic functions ---

enum class Variant {
  C32,
  UC32,
  C32H,
  UC32H,
  C32M,
  UC32M,
  C64,
  UC64,
  C64MS,
  UC64MS,
  C64US,
  UC64US,
  C32W,
  UC32W
};

Variant parse_variant(const char *type) {
  if (strcmp(type, "32") == 0 || strcmp(type, "chrono32") == 0)
    return Variant::C32;
  if (strcmp(type, "u32") == 0 || strcmp(type, "uchrono32") == 0)
    return Variant::UC32;
  if (strcmp(type, "32h") == 0 || strcmp(type, "chrono32h") == 0)
    return Variant::C32H;
  if (strcmp(type, "u32h") == 0 || strcmp(type, "uchrono32h") == 0)
    return Variant::UC32H;
  if (strcmp(type, "32m") == 0 || strcmp(type, "chrono32m") == 0)
    return Variant::C32M;
  if (strcmp(type, "u32m") == 0 || strcmp(type, "uchrono32m") == 0)
    return Variant::UC32M;
  if (strcmp(type, "64") == 0 || strcmp(type, "chrono64") == 0)
    return Variant::C64;
  if (strcmp(type, "u64") == 0 || strcmp(type, "uchrono64") == 0)
    return Variant::UC64;
  if (strcmp(type, "64ms") == 0 || strcmp(type, "chrono64ms") == 0)
    return Variant::C64MS;
  if (strcmp(type, "u64ms") == 0 || strcmp(type, "uchrono64ms") == 0)
    return Variant::UC64MS;
  if (strcmp(type, "64us") == 0 || strcmp(type, "chrono64us") == 0)
    return Variant::C64US;
  if (strcmp(type, "u64us") == 0 || strcmp(type, "uchrono64us") == 0)
    return Variant::UC64US;
  if (strcmp(type, "32w") == 0 || strcmp(type, "chrono32w") == 0)
    return Variant::C32W;
  if (strcmp(type, "u32w") == 0 || strcmp(type, "uchrono32w") == 0)
    return Variant::UC32W;
  return Variant::C64MS; // Default
}

// --- Generic SQL Functions Implementation ---

static void chrono_new_func(sqlite3_context *context, int argc,
                            sqlite3_value **argv) {
  const char *type =
      (argc > 0) ? (const char *)sqlite3_value_text(argv[0]) : "64ms";
  Variant v = parse_variant(type);

  switch (v) {
  case Variant::C32:
    set_result(context, (int32_t)chrono32());
    break;
  case Variant::UC32:
    set_result(context, (uint32_t)uchrono32());
    break;
  case Variant::C32H:
    set_result(context, (int32_t)chrono32h());
    break;
  case Variant::UC32H:
    set_result(context, (uint32_t)uchrono32h());
    break;
  case Variant::C32M:
    set_result(context, (int32_t)chrono32m());
    break;
  case Variant::UC32M:
    set_result(context, (uint32_t)uchrono32m());
    break;
  case Variant::C64:
    set_result(context, (int64_t)chrono64());
    break;
  case Variant::UC64:
    set_result(context, (uint64_t)uchrono64());
    break;
  case Variant::C64MS:
    set_result(context, (int64_t)chrono64ms());
    break;
  case Variant::UC64MS:
    set_result(context, (uint64_t)uchrono64ms());
    break;
  case Variant::C64US:
    set_result(context, (int64_t)chrono64us());
    break;
  case Variant::UC64US:
    set_result(context, (uint64_t)uchrono64us());
    break;
  case Variant::C32W:
    set_result(context, (int32_t)chrono32w());
    break;
  case Variant::UC32W:
    set_result(context, (uint32_t)uchrono32w());
    break;
  }
}

static void chrono_to_iso_func(sqlite3_context *context, int argc,
                               sqlite3_value **argv) {
  if (argc < 2) {
    sqlite3_result_error(context,
                         "chrono_to_iso(id, type) requires 2 arguments", -1);
    return;
  }
  int64_t id = sqlite3_value_int64(argv[0]);
  const char *type = (const char *)sqlite3_value_text(argv[1]);
  Variant v = parse_variant(type);
  std::string res;

  switch (v) {
  case Variant::C32:
    res = chrono32((int32_t)id).to_iso_string();
    break;
  case Variant::UC32:
    res = uchrono32((uint32_t)id).to_iso_string();
    break;
  case Variant::C32H:
    res = chrono32h((int32_t)id).to_iso_string();
    break;
  case Variant::UC32H:
    res = uchrono32h((uint32_t)id).to_iso_string();
    break;
  case Variant::C32M:
    res = chrono32m((int32_t)id).to_iso_string();
    break;
  case Variant::UC32M:
    res = uchrono32m((uint32_t)id).to_iso_string();
    break;
  case Variant::C64:
    res = chrono64((int64_t)id).to_iso_string();
    break;
  case Variant::UC64:
    res = uchrono64((uint64_t)id).to_iso_string();
    break;
  case Variant::C64MS:
    res = chrono64ms((int64_t)id).to_iso_string();
    break;
  case Variant::UC64MS:
    res = uchrono64ms((uint64_t)id).to_iso_string();
    break;
  case Variant::C64US:
    res = chrono64us((int64_t)id).to_iso_string();
    break;
  case Variant::UC64US:
    res = uchrono64us((uint64_t)id).to_iso_string();
    break;
  case Variant::C32W:
    res = chrono32w((int32_t)id).to_iso_string();
    break;
  case Variant::UC32W:
    res = uchrono32w((uint32_t)id).to_iso_string();
    break;
  }
  sqlite3_result_text(context, res.c_str(), -1, SQLITE_TRANSIENT);
}

static void chrono_from_iso_func(sqlite3_context *context, int argc,
                                 sqlite3_value **argv) {
  if (argc < 2) {
    sqlite3_result_error(context,
                         "chrono_from_iso(iso, type) requires 2 arguments", -1);
    return;
  }
  const char *iso = (const char *)sqlite3_value_text(argv[0]);
  const char *type = (const char *)sqlite3_value_text(argv[1]);
  Variant v = parse_variant(type);

  try {
    switch (v) {
    case Variant::C32:
      set_result(context, (int32_t)chrono32::from_iso_cstring(iso));
      break;
    case Variant::UC32:
      set_result(context, (uint32_t)uchrono32::from_iso_cstring(iso));
      break;
    case Variant::C32H:
      set_result(context, (int32_t)chrono32h::from_iso_cstring(iso));
      break;
    case Variant::UC32H:
      set_result(context, (uint32_t)uchrono32h::from_iso_cstring(iso));
      break;
    case Variant::C32M:
      set_result(context, (int32_t)chrono32m::from_iso_cstring(iso));
      break;
    case Variant::UC32M:
      set_result(context, (uint32_t)uchrono32m::from_iso_cstring(iso));
      break;
    case Variant::C64:
      set_result(context, (int64_t)chrono64::from_iso_cstring(iso));
      break;
    case Variant::UC64:
      set_result(context, (uint64_t)uchrono64::from_iso_cstring(iso));
      break;
    case Variant::C64MS:
      set_result(context, (int64_t)chrono64ms::from_iso_cstring(iso));
      break;
    case Variant::UC64MS:
      set_result(context, (uint64_t)uchrono64ms::from_iso_cstring(iso));
      break;
    case Variant::C64US:
      set_result(context, (int64_t)chrono64us::from_iso_cstring(iso));
      break;
    case Variant::UC64US:
      set_result(context, (uint64_t)uchrono64us::from_iso_cstring(iso));
      break;
    case Variant::C32W:
      set_result(context, (int32_t)chrono32w::from_iso_cstring(iso));
      break;
    case Variant::UC32W:
      set_result(context, (uint32_t)uchrono32w::from_iso_cstring(iso));
      break;
    }
  } catch (const std::exception &e) {
    sqlite3_result_error(context, e.what(), -1);
  }
}

static void chrono_get_time_func(sqlite3_context *context, int argc,
                                 sqlite3_value **argv) {
  if (argc < 2) {
    sqlite3_result_error(context,
                         "chrono_get_time(id, type) requires 2 arguments", -1);
    return;
  }
  int64_t id = sqlite3_value_int64(argv[0]);
  const char *type = (const char *)sqlite3_value_text(argv[1]);
  Variant v = parse_variant(type);

  switch (v) {
  case Variant::C32:
    set_result(context, chrono32((int32_t)id).get_timestamp());
    break;
  case Variant::UC32:
    set_result(context, uchrono32((uint32_t)id).get_timestamp());
    break;
  case Variant::C32H:
    set_result(context, chrono32h((int32_t)id).get_timestamp());
    break;
  case Variant::UC32H:
    set_result(context, uchrono32h((uint32_t)id).get_timestamp());
    break;
  case Variant::C32M:
    set_result(context, chrono32m((int32_t)id).get_timestamp());
    break;
  case Variant::UC32M:
    set_result(context, uchrono32m((uint32_t)id).get_timestamp());
    break;
  case Variant::C64:
    set_result(context, chrono64((int64_t)id).get_timestamp());
    break;
  case Variant::UC64:
    set_result(context, uchrono64((uint64_t)id).get_timestamp());
    break;
  case Variant::C64MS:
    set_result(context, chrono64ms((int64_t)id).get_timestamp());
    break;
  case Variant::UC64MS:
    set_result(context, uchrono64ms((uint64_t)id).get_timestamp());
    break;
  case Variant::C64US:
    set_result(context, chrono64us((int64_t)id).get_timestamp());
    break;
  case Variant::UC64US:
    set_result(context, uchrono64us((uint64_t)id).get_timestamp());
    break;
  case Variant::C32W:
    set_result(context, chrono32w((int32_t)id).get_timestamp());
    break;
  case Variant::UC32W:
    set_result(context, uchrono32w((uint32_t)id).get_timestamp());
    break;
  }
}

// --- Individual Functions via Token Concatenation ---

// Macro to define all functions for a variant
#define DEFINE_CHRONO_METHODS(name)                                            \
  static void name##_func(sqlite3_context *context, int argc,                  \
                          sqlite3_value **argv) {                              \
    (void)argc;                                                                \
    (void)argv;                                                                \
    set_result(context, name());                                               \
  }                                                                            \
  static void name##_get_time_func(sqlite3_context *context, int argc,         \
                                   sqlite3_value **argv) {                     \
    if (argc < 1)                                                              \
      return;                                                                  \
    sqlite3_result_text(                                                       \
        context,                                                               \
        name((uint64_t)sqlite3_value_int64(argv[0])).to_iso_string().c_str(),  \
        -1, SQLITE_TRANSIENT);                                                 \
  }                                                                            \
  static void name##_from_iso_func(sqlite3_context *context, int argc,         \
                                   sqlite3_value **argv) {                     \
    if (argc < 1)                                                              \
      return;                                                                  \
    try {                                                                      \
      set_result(context, name::from_iso_cstring(                              \
                              (const char *)sqlite3_value_text(argv[0])));     \
    } catch (const std::exception &e) {                                        \
      sqlite3_result_error(context, e.what(), -1);                             \
    }                                                                          \
  }

// Define implementation functions for each variant
DEFINE_CHRONO_METHODS(chrono32)
DEFINE_CHRONO_METHODS(uchrono32)
DEFINE_CHRONO_METHODS(chrono32h)
DEFINE_CHRONO_METHODS(uchrono32h)
DEFINE_CHRONO_METHODS(chrono32m)
DEFINE_CHRONO_METHODS(uchrono32m)
DEFINE_CHRONO_METHODS(chrono64)
DEFINE_CHRONO_METHODS(uchrono64)
DEFINE_CHRONO_METHODS(chrono64ms)
DEFINE_CHRONO_METHODS(uchrono64ms)
DEFINE_CHRONO_METHODS(chrono64us)
DEFINE_CHRONO_METHODS(uchrono64us)
DEFINE_CHRONO_METHODS(chrono32w)
DEFINE_CHRONO_METHODS(uchrono32w)

#define REGISTER_VARIANT(name)                                                 \
  sqlite3_create_function(db, #name, 0, SQLITE_UTF8, 0, name##_func, 0, 0);    \
  sqlite3_create_function(db, #name "_to_iso", 1,                              \
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,               \
                          name##_get_time_func, 0, 0);                         \
  sqlite3_create_function(db, #name "_from_iso", 1,                            \
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,               \
                          name##_from_iso_func, 0, 0);                         \
  sqlite3_create_function(db, #name "_get_time", 1,                            \
                          SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,               \
                          name##_get_time_func, 0, 0);

extern "C" {
int sqlite3_chronoid_init(sqlite3 *db, char **pzErrMsg,
                          const sqlite3_api_routines *pApi) {
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;

  // Generic functions
  sqlite3_create_function(db, "chrono_new", -1, SQLITE_UTF8, 0, chrono_new_func,
                          0, 0);
  sqlite3_create_function(db, "chrono_to_iso", 2, SQLITE_UTF8, 0,
                          chrono_to_iso_func, 0, 0);
  sqlite3_create_function(db, "chrono_from_iso", 2, SQLITE_UTF8, 0,
                          chrono_from_iso_func, 0, 0);
  sqlite3_create_function(db, "chrono_get_time", 2, SQLITE_UTF8, 0,
                          chrono_get_time_func, 0, 0);

  // Individual variant registration (Names match Postgres extension)
  REGISTER_VARIANT(chrono32);
  REGISTER_VARIANT(uchrono32);
  REGISTER_VARIANT(chrono32h);
  REGISTER_VARIANT(uchrono32h);
  REGISTER_VARIANT(chrono32m);
  REGISTER_VARIANT(uchrono32m);
  REGISTER_VARIANT(chrono64);
  REGISTER_VARIANT(uchrono64);
  REGISTER_VARIANT(chrono64ms);
  REGISTER_VARIANT(uchrono64ms);
  REGISTER_VARIANT(chrono64us);
  REGISTER_VARIANT(uchrono64us);
  REGISTER_VARIANT(chrono32w);
  REGISTER_VARIANT(uchrono32w);

  return SQLITE_OK;
}
}
