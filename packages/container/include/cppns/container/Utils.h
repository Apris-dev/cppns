#pragma once

#ifdef USING_CPPNS_ARCHIVE
#include "cppns/archive/HashArchive.h"
#endif

template <typename TType>
struct TContainerHasher {
    size_t operator()(const TType& p) const noexcept {
#ifdef USING_CPPNS_ARCHIVE
        CHashArchive archive;
        archive << p;
        return archive.get();
#else
        return std::hash<TType>()(p);
#endif
    }
};
