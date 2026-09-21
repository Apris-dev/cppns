#pragma once

#include <type_traits>
#include <initializer_list>

#ifdef USING_CPPNS_ARCHIVE
#include "cppns/archive/Archive.h"
#endif

namespace sstl {
    template <class TType, class... TArgs>
    struct EnforceConvertible {
        static_assert(std::conjunction_v<std::is_convertible<TType, TArgs>...>, "An argument in TArgs is not convertible to TType!");
        using Type = TType;
    };
}

template <class TType>
using TInitializerList = std::initializer_list<TType>;

#ifdef USING_CPPNS_ARCHIVE
// Initializer lists cannot be written to, but can be read from
template <typename TType>
CSArchive& operator<<(CSArchive& inArchive, const TInitializerList<TType>& list) {
    for (const auto& obj : list)
        inArchive << obj;
    return inArchive;
}

// Initializer lists cannot be written to, but can be read from
template <typename TType>
COArchive& operator<<(COArchive& inArchive, const TInitializerList<TType>& list) {
    inArchive << list.size();
    for (const auto& obj : list)
        inArchive << obj;
    return inArchive;
}
#endif