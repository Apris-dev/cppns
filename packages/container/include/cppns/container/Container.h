#pragma once

/*
 * Container Base class designed to use CRTP to avoid runtime type deduction.
 * This also has the added benefit of avoiding compiling functions with limited access. (Like stack/queue push at index)
 */

#include <functional>
#include <stdexcept>
#include <algorithm>
#include <any>

#ifdef USING_CPPNS_ARCHIVE
#include "cppns/archive/Archive.h"
#include "cppns/archive/HashArchive.h"
#endif

#include "cppns/util/Comparison.h"

#ifdef __cpp_lib_ranges
#include <algorithm>
#endif

#ifdef USING_CPPNS_MEMORY
#include "cppns/memory/Memory.h"
#endif

#include "cppns/util/Pair.h"

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
#else
#define FIND(c, x, ...) std::find(c.begin(), c.end(), x)
#define FIND_IF(c, func, ...) std::find_if(c.begin(), c.end(), func)
#define ERASE(c, x, ...) c.erase(FIND(c, x, __VA_ARGS__))
#define DISTANCE(c, x, ...) std::distance(c.begin(), FIND(c, x, __VA_ARGS__))
#define DISTANCE_IF(c, func, ...) std::distance(c.begin(), FIND_IF(c, func, __VA_ARGS__))
#define DISTANCE_LAST_IF(c, func, ...) std::distance(c.begin(), FIND_LAST_IF(c, func, __VA_ARGS__))
#define SIZE(c) std::distance(c.begin(), c.end())
#define SHUFFLE(c, r) std::shuffle(c.begin(), c.end(), r);
#endif

#if USING_CXX20
	#define ASSOCIATIVE_CONTAINS(c, x) c.contains(x)
#else
	#define ASSOCIATIVE_CONTAINS(c, x) c.find(x) != c.end()
#endif

#define CONTAINS(c, x, ...) FIND(c, x, __VA_ARGS__) != c.end()
#define CONTAINS_IF(c, func, ...) FIND_IF(c, func, __VA_ARGS__) != c.end()

// Makes it easy to see if a function is guaranteed or not
#define GUARANTEED = 0;
#define NOT_GUARANTEED { throw std::runtime_error("Attempted Usage of unimplemented function in TContainer."); }

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

// A universal iterator that guarantees std::advance
template <typename TContainerType, typename TItrType>
struct TVirtualIterator {

	using Traits = TContainerTraits<TContainerType>;
	constexpr static EContainerType ContainerType = Traits::ContainerType;

	TVirtualIterator(const TItrType& inItr): itr(inItr) {}

	//TODO: verify offset or keep unsafe...

	decltype(auto) operator*() noexcept { //returns ref
		if constexpr (ContainerType == EContainerType::ASSOCIATIVE) {
			using KeyType = typename Traits::KeyType;
			using ValueType = typename Traits::ValueType;
			constexpr static bool isConst = std::is_const_v<std::remove_reference_t<decltype(*itr)>>;
			return reinterpret_cast<std::conditional_t<isConst, const TPair<KeyType, ValueType>&, TPair<KeyType, ValueType>&>>(*itr);
		} else {
			return *itr;
		}
	}

	decltype(auto) operator->() noexcept { //returns pointer
		if constexpr (ContainerType == EContainerType::ASSOCIATIVE) {
			using KeyType = typename Traits::KeyType;
			using ValueType = typename Traits::ValueType;
			return reinterpret_cast<TPair<KeyType, ValueType>*>(itr.operator->());
		} else {
			return itr.operator->();
		}
	}

	decltype(auto) operator[](const int offset) noexcept {
		return itr[offset];
	}

	decltype(auto) operator[](const int offset) const noexcept {
		return itr[offset];
	}

	bool operator==(TVirtualIterator otr) {
		return itr == otr.itr;
	}

	bool operator!=(TVirtualIterator otr) {
		return itr != otr.itr;
	}

	bool operator<(const TVirtualIterator& otr) const noexcept {
		return itr < otr.itr;
	}

	bool operator>(const TVirtualIterator& otr) const noexcept {
		return otr < *this;
	}

	bool operator<=(const TVirtualIterator& otr) const noexcept {
		return !(otr < *this);
	}

	bool operator>=(const TVirtualIterator& otr) const noexcept {
		return !(*this < otr);
	}

	TVirtualIterator& operator++() noexcept {
		std::advance(itr, 1);
		return *this;
	}

	TVirtualIterator operator++(int) noexcept {
		TVirtualIterator otr = *this;
    		++*this;
    		return otr;
	}

	TVirtualIterator& operator+=(const int offset) noexcept {
		std::advance(itr, offset);
		return *this;
	}

	TVirtualIterator operator+(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr += offset;
		return otr;
	}

	friend TVirtualIterator operator+(const int offset, TVirtualIterator otr) noexcept {
		otr += offset;
		return otr;
	}

	TVirtualIterator& operator--() noexcept {
		std::advance(itr, -1);
		return *this;
	}

	TVirtualIterator operator--(int) noexcept {
    		TVirtualIterator otr = *this;
    		--*this;
    		return otr;
	}

	TVirtualIterator& operator-=(const int offset) noexcept {
		std::advance(itr, -offset);
		return *this;
	}

	TVirtualIterator operator-(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr -= offset;
		return otr;
	}

	friend TVirtualIterator operator-(const int offset, TVirtualIterator otr) noexcept {
		otr -= offset;
		return otr;
	}

    TItrType itr;
};

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

// A basic container of any amount of objects
// A size of 0 implies a dynamic array
template <typename TContainerType>
struct TSequenceContainer : SContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;
	using Iterator = TVirtualIterator<TContainerType, typename Traits::Iterator>;
	using ReverseIterator = TVirtualIterator<TContainerType, typename Traits::ReverseIterator>;
	using ConstIterator = TVirtualIterator<TContainerType, typename Traits::ConstIterator>;
	using ConstReverseIterator = TVirtualIterator<TContainerType, typename Traits::ConstReverseIterator>;
	constexpr static bool bIsContiguousMemory = Traits::bIsContiguousMemory;
	constexpr static bool bIsLimitedAccess = Traits::bIsLimitedAccess;
	constexpr static bool bIsForwardOnly = Traits::bIsForwardOnly;
	constexpr static bool bIsLimitedSize = Traits::bIsLimitedSize;

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return derived(*this).getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return derived(*this).isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] TType& top() { return derived(*this).top(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] const TType& top() const { return derived(*this).top(); }

	// Gets the first element possible, or the 'top' of the container
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& bottom() {
		return derived(*this).bottom();
	}
	// Gets the first element possible, or the 'top' of the container
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& bottom() const {
		return derived(*this).bottom();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] Iterator begin() noexcept {
		return derived(*this).begin();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator begin() const noexcept {
		return derived(*this).begin();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess && !bIsForwardOnly)
	[[nodiscard]] ReverseIterator rbegin() noexcept {
		return derived(*this).rbegin();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess && !bIsForwardOnly)
	[[nodiscard]] ConstReverseIterator rbegin() const noexcept {
		return derived(*this).rbegin();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] Iterator end() noexcept {
		return derived(*this).end();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator end() const noexcept {
		return derived(*this).end();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess && !bIsForwardOnly)
	[[nodiscard]] ReverseIterator rend() noexcept {
		return derived(*this).rend();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess && !bIsForwardOnly)
	[[nodiscard]] ConstReverseIterator rend() const noexcept {
		return derived(*this).rend();
	}

	// Checks if a certain index is contained within the container
	[[nodiscard]] bool isValid(const size_t index) const  { return derived(*this).isValid(index); }

	// Checks if a certain object is contained within the container
	template <typename TOtherType>
	[[nodiscard]] bool contains(const TOtherType& obj) const { return derived(*this).contains(obj); }

	[[nodiscard]] bool contains(const std::function<bool(const TType&)>& inFunction) {
		return derived(*this).contains(inFunction);
	}

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		return derived(*this).containsAll(std::forward<TOtherType>(obj)...);
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		return derived(*this).containsAll(std::forward<TFunc>(inFunctions)...);
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
		return derived(*this).containsOne(std::forward<TFunc>(inFunctions)...);
	}

	// Find a certain element in the container
	template <typename TOtherType>
	[[nodiscard]] size_t find(const TOtherType& obj) const { return derived(*this).find(obj); }

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
		return derived(*this).findFirst(std::forward<TFunc>(inFunctions)...);
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
		return derived(*this).findLast(std::forward<TFunc>(inFunctions)...);
	}

	// Get an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& get(size_t index) {
		return derived(*this).get(index);
	}
	// Get an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& get(size_t index) const {
		return derived(*this).get(index);
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& operator[](const size_t index) {
		return get(index);
	}
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& operator[](const size_t index) const {
		return get(index);
	}

	// Fills container with n defaulted elements
	void resize(size_t amt) { derived(*this).resize(amt); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TType(size_t)> func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bIsContiguousMemory)
	void reserve(size_t amt) {
		derived(*this).reserve(amt);
	}

	// Adds a defaulted element to the container
	TType& push() { return derived(*this).push(); }
	// Adds an element to the container, returning the index where it was added
	size_t push(const TType& obj) { return derived(*this).push(obj); }
	// Adds an element to the container, returning the index where it was added
	size_t push(TType&& obj) { return derived(*this).push(std::move(obj)); }
	// Inserts an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void push(size_t index, const TType& obj) {
		derived(*this).push(index, obj);
	}
	// Inserts an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void push(size_t index, TType&& obj) {
		derived(*this).push(index, std::move(obj));
	}

	// Replaces an element at a specified index, and returns the original
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void replace(size_t index, const TType& obj) {
		derived(*this).push(index, obj);
	}
	// Replaces an element at a specified index, and returns the original
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void replace(size_t index, TType&& obj) {
		derived(*this).replace(index, std::move(obj));
	}

	// Removes all elements from the container
	void clear() { derived(*this).clear(); }

	// Removes the topmost element from the container
	void pop() { derived(*this).pop(); }
	// Removes an element at the specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void popAt(size_t index) {
		derived(*this).popAt(index);
	}
	// Removes a certain object from the container
	template <typename TOtherType, bool b = !bIsLimitedAccess,
		std::enable_if_t<b, int> = 0
	>
	void pop(const TOtherType& obj) {
		derived(*this).pop(obj);
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess && !bIsForwardOnly)
	void sort() {
		derived(*this).sort();
	}

	template <typename Func, bool b = !bIsLimitedAccess && !bIsForwardOnly,
		std::enable_if_t<b, int> = 0
	>
	void sort(Func&& func) {
		derived(*this).sort(std::forward<Func>(func));
	}

	// Moves an object at index from this to container otr
	template <typename TOtherContainerType>
	void transfer(TSequenceContainer<TOtherContainerType>& otr, const size_t index) {
		static_assert(!bIsLimitedAccess, "Limited Access Type cannot be transferred from");
		derived(*this).transfer(otr, index);
	}

	// Appends another container to this current one
	template <typename TOtherContainerType>
	void append(const TSequenceContainer<TOtherContainerType>& otr) {
		static_assert(!bIsLimitedSize, "Limited Size Type cannot be appended to");
		derived(*this).append(otr);
	}

	void doFor(const size_t index, const std::function<void(TType&)>& func) {
		func(get(index));
	}

	void doFor(const size_t index, const std::function<void(const TType&)>& func) const {
		func(get(index));
	}

	void doFor(const size_t start, const size_t end, const std::function<void(size_t, TType&)>& func) {
		for (size_t i = start; i < end; ++i) {
			func(i, get(i));
		}
	}

	void doFor(const size_t start, const size_t end, const std::function<void(size_t, const TType&)>& func) const {
		for (size_t i = start; i < end; ++i) {
			func(i, get(i));
		}
	}

#ifdef USING_CPPNS_ARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSequenceContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&](size_t) {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSequenceContainer& inValue) {
		inArchive << inValue.getSize();
		for (const TType& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}
#endif

};

// Designed to be a container with a key for indexing
// Note: always requires a comparable key type
/*template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerTraits<TContainerType>::KeyType, typename TContainerTraits<TContainerType>::KeyType>, int> = 0
>*/
template <typename TContainerType>
struct TAssociativeContainer : SContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TKeyType = typename Traits::KeyType;
	using TValueType = typename Traits::ValueType;
	using Iterator = TVirtualIterator<TContainerType, typename Traits::Iterator>;
	using ReverseIterator = TVirtualIterator<TContainerType, typename Traits::ReverseIterator>;
	using ConstIterator = TVirtualIterator<TContainerType, typename Traits::ConstIterator>;
	using ConstReverseIterator = TVirtualIterator<TContainerType, typename Traits::ConstReverseIterator>;
	constexpr static bool bHasHashing = Traits::bHasHashing;

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return derived(*this).getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return derived(*this).isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] TPair<TKeyType, const TValueType&> top() const { return derived(*this).top(); }

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] TPair<TKeyType, const TValueType&> bottom() const { return derived(*this).bottom(); }

	[[nodiscard]] Iterator begin() noexcept { return derived(*this).begin(); }

	[[nodiscard]] ConstIterator begin() const noexcept { return derived(*this).begin(); }

	[[nodiscard]] ReverseIterator rbegin() noexcept { return derived(*this).rbegin(); }

	[[nodiscard]] ConstReverseIterator rbegin() const noexcept { return derived(*this).rbegin(); }

	[[nodiscard]] Iterator end() noexcept { return derived(*this).end(); }

	[[nodiscard]] ConstIterator end() const noexcept { return derived(*this).end(); }

	[[nodiscard]] ReverseIterator rend() noexcept { return derived(*this).rend(); }

	[[nodiscard]] ConstReverseIterator rend() const noexcept { return derived(*this).rend(); }

	// Checks if a certain key is contained within the container
	[[nodiscard]] bool isValid(const TKeyType& key) const { return derived(*this).isValid(key); }

	// Checks if a certain object is contained within the container
	template <typename TOtherValueType>
	[[nodiscard]] bool contains(const TOtherValueType& obj) const { return derived(*this).contains(obj); }

	// Find a certain element in the container
	template <typename TOtherValueType>
	[[nodiscard]] TKeyType find(const TOtherValueType& obj) const { return derived(*this).find(obj); }

	// Get an element at a specified key
	[[nodiscard]] TValueType& get(const TKeyType& key) { return derived(*this).get(key); }
	// Get an element at a specified key
	[[nodiscard]] const TValueType& get(const TKeyType& key) const { return derived(*this).get(key); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TPair<TKeyType, TValueType>()> func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bHasHashing)
	void reserve(size_t amt) { derived(*this).reserve(amt); }

	// Adds a defaulted element to the container
	TPair<TKeyType, const TValueType&> push() { return derived(*this).push(); }
	// Adds a defaulted element at key to the container
	TValueType& push(const TKeyType& key) { return derived(*this).push(key); }
	// Adds an element value at key to the container
	TValueType& push(const TKeyType& key, const TValueType& value) { return derived(*this).push(key, value); }
	// Adds an element value at key to the container
	TValueType& push(const TKeyType& key, TValueType&& value) { return derived(*this).push(key, std::move(value)); }
	// Adds an element to the container
	void push(const TPair<TKeyType, TValueType>& pair) { derived(*this).push(pair); }
	// Adds an element to the container
	void push(TPair<TKeyType, TValueType>&& pair) { derived(*this).push(std::move(pair)); }
	// Replaces a specified element at key with another element
	void replace(const TKeyType& key, const TValueType& obj) { derived(*this).replace(key, obj); }
	// Replaces a specified element at key with another element
	void replace(const TKeyType& key, TValueType&& obj) { derived(*this).replace(key, std::move(obj)); }

	// Removes all elements from the container
	void clear() { derived(*this).clear(); }

	// Removes the topmost element from the container
	void pop() { derived(*this).pop(); }
	// Removes an element at key from the container
	void pop(const TKeyType& key) { derived(*this).pop(key); }

	// Moves an object at key from this to container otr
	template <typename TOtherContainerType>
	void transfer(TAssociativeContainer<TOtherContainerType>& otr, const TKeyType& key) {
		derived(*this).transfer(otr, key);
	}

	// Appends another container to this current one
	template <typename TOtherContainerType>
	void append(const TAssociativeContainer<TOtherContainerType>& otr) {
		derived(*this).append(otr);
	}

#ifdef USING_CPPNS_ARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TAssociativeContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TPair<TKeyType, TValueType> pair;
			inArchive >> pair;
			return pair;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TAssociativeContainer& inValue) {
		inArchive << inValue.getSize();
		for (const auto& pair : inValue) {
			inArchive << pair;
		}
		return inArchive;
	}
#endif

};

// Designed to be a container without indexing
// Note: always requires a comparable type
/*template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerTraits<TContainerType>::Type, typename TContainerTraits<TContainerType>::Type>, int> = 0
>*/
template <typename TContainerType>
struct TSelfAssociativeContainer : SContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;
	using Iterator = TVirtualIterator<TContainerType, typename Traits::Iterator>;
	using ReverseIterator = TVirtualIterator<TContainerType, typename Traits::ReverseIterator>;
	using ConstIterator = TVirtualIterator<TContainerType, typename Traits::ConstIterator>;
	using ConstReverseIterator = TVirtualIterator<TContainerType, typename Traits::ConstReverseIterator>;
	constexpr static bool bHasHashing = Traits::bHasHashing;
	constexpr static bool bIsForwardOnly = Traits::bIsForwardOnly;

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return derived(*this).getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return derived(*this).isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] const TType& top() const { return derived(*this).top(); }

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] const TType& bottom() const { return derived(*this).bottom(); }

	[[nodiscard]] Iterator begin() noexcept { return derived(*this).begin(); }

	[[nodiscard]] ConstIterator begin() const noexcept { return derived(*this).begin(); }

	ENABLE_FUNC_IF(!bIsForwardOnly)
	[[nodiscard]] ReverseIterator rbegin() noexcept { return derived(*this).rbegin(); }

	ENABLE_FUNC_IF(!bIsForwardOnly)
	[[nodiscard]] ConstReverseIterator rbegin() const noexcept { return derived(*this).rbegin(); }

	[[nodiscard]] Iterator end() noexcept { return derived(*this).end(); }

	[[nodiscard]] ConstIterator end() const noexcept { return derived(*this).end(); }

	ENABLE_FUNC_IF(!bIsForwardOnly)
	[[nodiscard]] ReverseIterator rend() noexcept { return derived(*this).rend(); }

	ENABLE_FUNC_IF(!bIsForwardOnly)
	[[nodiscard]] ConstReverseIterator rend() const noexcept { return derived(*this).rend(); }

	// Checks if a certain index is contained within the container
	[[nodiscard]] bool isValid(const size_t index) const {
		return index < getSize();
	}

	// Checks if a certain object is contained within the container
	template <typename TOtherType>
	[[nodiscard]] bool contains(const TOtherType& obj) const { return derived(*this).contains(obj); }

	// Fills container with n defaulted elements
	void resize(size_t amt) { derived(*this).resize(amt); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TType()> func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bHasHashing)
	void reserve(size_t amt) { derived(*this).reserve(amt); }

	// Adds a defaulted element to the container
	const TType& push() { return derived(*this).push(); }
	// Adds an element to the container
	void push(const TType& obj) { derived(*this).push(obj); }
	// Adds an element to the container
	void push(TType&& obj) { derived(*this).push(std::move(obj)); }

	// Replaces a specified element with another element
	void replace(const TType& tgt, const TType& obj) { derived(*this).replace(tgt, obj); }
	// Replaces a specified element with another element
	void replace(const TType& tgt, TType&& obj) { derived(*this).replace(tgt, std::move(obj)); }

	// Removes all elements from the container
	void clear() { derived(*this).clear(); }

	// Removes the topmost element from the container
	void pop() { derived(*this).pop(); }
	// Removes an element from the container
	template <typename TOtherType>
	void pop(const TOtherType& obj) { derived(*this).pop(obj); }

	// Moves an object from this to container otr
	template <typename TOtherContainerType, typename TOtherType>
	void transfer(TSelfAssociativeContainer<TOtherContainerType>& otr, TOtherType& obj) {
		derived(*this).transfer(otr, obj);
	}

	// Appends another container to this current one
	template <typename TOtherContainerType>
	void append(const TSelfAssociativeContainer<TOtherContainerType>& otr) {
		derived(*this).append(otr);
	}

#ifdef USING_CPPNS_ARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSelfAssociativeContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSelfAssociativeContainer& inValue) {
		inArchive << inValue.getSize();
		for (const TType& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}
#endif

};