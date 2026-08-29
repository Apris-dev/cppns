#pragma once

#include <limits>

#ifdef __cpp_lib_bitops
#include <bit>
#endif

#include <cstring>

#include "Archive.h"

class CHashArchive : public COutputArchive {

public:

    [[nodiscard]] size_t get() const { return hash; }

    void operator+=(const size_t inHash) {
#ifdef __cpp_lib_bitops
        hash = std::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(inHash);
#else
        hash = rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(inHash);
#endif
    }

    size_t write(const void* inValue, const size_t inElementSize) override {
        return write(inValue, inElementSize, 1);
    }

    virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
        auto bytes = static_cast<const uint8_t*>(inValue);

        size_t total = inElementSize * inCount;

        while (total >= sizeof(size_t)) {
            size_t chunk;
            memcpy(&chunk, bytes, sizeof(size_t));

            *this += chunk;

            bytes += sizeof(size_t);
            total -= sizeof(size_t);
        }

        if (total > 0) {
            size_t remainder = 0;
            memcpy(&remainder, bytes, total);
            *this += remainder;
        }

        // Return the total number of bytes written to hash
        return total;
    }

private:

    /*
     * Thanks to Wolfgang Brehm on Stack Overflow for this hashing implementation
     */

    template<typename TType>
    constexpr TType xorshift(const TType& n, int i) noexcept {
        return n ^ (n >> i);
    }

    // a hash function with another name as to not confuse with std::hash
    /*constexpr uint32_t distribute(const uint32_t& n) noexcept {
        return 3423571495ul * xorshift(0x55555555ul * xorshift(n,16),16);
    }*/

    // a hash function with another name as to not confuse with std::hash
    constexpr size_t distribute(const size_t& n) noexcept {
        return 17316035218449499591ull * xorshift(0x5555555555555555ull * xorshift(n,32),32);
    }

#ifndef __cpp_lib_bitops
    template <typename TType,
        std::enable_if_t<std::is_unsigned_v<TType>, int> = 0
    >
    constexpr TType rotl(const TType& n, const int rotation) noexcept {
        const TType m = (std::numeric_limits<TType>::digits - 1);
        const TType c = rotation & m;
        return (n << c) | (n >> ((TType(0) - c) & m));
    }
#endif
    
    size_t hash = 0;
};