#pragma once

#include <type_traits>
#include <string>
#include <sstream>

/*
 * Converser
 *
 * A small helper that wraps a value of type T and exposes conversion
 * methods to the other primitive types.  The default constructed instance
 * holds a zero/false value so it is always safe to use.
 */

// Bool Convert
namespace Convert {
    inline char toChar(const bool b) { return b ? 't' : 'f'; }
    inline unsigned char toUChar(const bool b) { return b ? 't' : 'f'; }
    inline signed char toSChar(const bool b) { return b ? 't' : 'f'; }

    inline unsigned short toUShort(const bool b) { return b ? 1 : 0; }
    inline short toShort(const bool b) { return b ? 1 : 0; }

    inline unsigned int toUInt(const bool b) { return b ? 1u : 0u; }
    inline int toInt(const bool b) { return b ? 1 : 0; }

    inline unsigned long toULong(const bool b) { return b ? 1ul : 0ul; }
    inline long toLong(const bool b) { return b ? 1l : 0l; }

    inline unsigned long long toULongLong(const bool b) { return b ? 1ull : 0ull; }
    inline long long toLongLong(const bool b) { return b ? 1ll : 0ll; }

    inline float toFloat(const bool b) { return b ? 1.f : 0.f; }
    inline double toDouble(const bool b) { return b ? 1.0 : 0.0; }
    inline long double toLongDouble(const bool b) { return b ? 1.0 : 0.0; }

    inline const char* toString(const bool b) { return b ? "true" : "false"; }
}