#pragma once

/*
 * Container Base class designed to use CRTP to avoid runtime type deduction.
 * This also has the added benefit of avoiding compiling functions with limited access. (Like stack/queue push at index)
 */

#include <functional>

#ifdef __cpp_lib_ranges
#include <algorithm>
#endif

#include "cppns/util/Comparison.h"

#ifdef __cpp_lib_ranges_find_last
#define FIND_LAST_IF(c, func, ...) std::ranges::find_last_if(c, func).begin()
#else
#define FIND_LAST_IF(c, func, ...) std::find_if(c.rbegin(), c.rend(), func)
#endif

#ifdef __cpp_lib_ranges
#define FIND(c, x, ...) std::ranges::find_if(c, [&x](const auto& v) { return v == x; })
#define FIND_IF(c, func, ...) std::ranges::find_if(c, func)
#define ERASE(c, x, ...) c.erase(FIND(c, x, __VA_ARGS__))
#define DISTANCE(c, x, ...) std::ranges::distance(c.begin(), FIND(c, x, __VA_ARGS__))
#define DISTANCE_IF(c, func, ...) std::ranges::distance(c.begin(), FIND_IF(c, func, __VA_ARGS__))
#define DISTANCE_LAST_IF(c, func, ...) std::ranges::distance(c.begin(), FIND_LAST_IF(c, func, __VA_ARGS__))
#define SIZE(c) std::ranges::distance(c)
#define SHUFFLE(c, r) std::ranges::shuffle(c, r);
#define SORT(c) std::ranges::sort(c)
#define SORT_F(c, func) std::ranges::sort(c, func)
#else
#define FIND(c, x, ...) std::find(c.begin(), c.end(), x)
#define FIND_IF(c, func, ...) std::find_if(c.begin(), c.end(), func)
#define ERASE(c, x, ...) c.erase(FIND(c, x, __VA_ARGS__))
#define DISTANCE(c, x, ...) std::distance(c.begin(), FIND(c, x, __VA_ARGS__))
#define DISTANCE_IF(c, func, ...) std::distance(c.begin(), FIND_IF(c, func, __VA_ARGS__))
#define DISTANCE_LAST_IF(c, func, ...) std::distance(c.begin(), FIND_LAST_IF(c, func, __VA_ARGS__))
#define SIZE(c) std::distance(c.begin(), c.end())
#define SHUFFLE(c, r) std::shuffle(c.begin(), c.end(), r);
#define SORT(c) std::sort(c.begin(), c.end())
#define SORT_F(c, func) std::sort(c.begin(), c.end(), func)
#endif

#define CONTAINS(c, x, ...) FIND(c, x, __VA_ARGS__) != c.end()
#define CONTAINS_IF(c, func, ...) FIND_IF(c, func, __VA_ARGS__) != c.end()

template <typename>
struct TContainerTraits;

template <typename>
struct TSequenceContainer;

template <typename>
struct TAssociativeContainer;

template <typename>
struct TSelfAssociativeContainer;

enum class EContainerType : uint8 {
	SEQUENCE,
	ASSOCIATIVE,
	SELF_ASSOCIATIVE
};

template <typename TContainerType, typename TItrType>
struct TVirtualIterator {};

//TODO: container lambda functions

// Allow subclasses to get each other's containers without public access
struct SContainer {

protected:

	template <typename TContainerType>
	static TContainerType& derived(TSequenceContainer<TContainerType>& self) {
		return static_cast<TContainerType&>(self);
	}

	template <typename TContainerType>
	static const TContainerType& derived(const TSequenceContainer<TContainerType>& self) {
		return static_cast<const TContainerType&>(self);
	}

	template <typename TContainerType>
	static TContainerType& derived(TAssociativeContainer<TContainerType>& self) {
		return static_cast<TContainerType&>(self);
	}

	template <typename TContainerType>
	static const TContainerType& derived(const TAssociativeContainer<TContainerType>& self) {
		return static_cast<const TContainerType&>(self);
	}

	template <typename TContainerType>
	static TContainerType& derived(TSelfAssociativeContainer<TContainerType>& self) {
		return static_cast<TContainerType&>(self);
	}

	template <typename TContainerType>
	static const TContainerType& derived(const TSelfAssociativeContainer<TContainerType>& self) {
		return static_cast<const TContainerType&>(self);
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(TSequenceContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(const TSequenceContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(TAssociativeContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(const TAssociativeContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(TSelfAssociativeContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

	template <typename TContainerType>
	static decltype(auto) getSubcontainer(const TSelfAssociativeContainer<TContainerType>& self) {
		return derived(self).getSubcontainer();
	}

};
