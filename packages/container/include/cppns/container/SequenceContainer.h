#pragma once

#include "Container.h"

template <typename TContainerType, typename TItrType>
requires (TContainerTraits<TContainerType>::ContainerType == EContainerType::SEQUENCE)
struct TVirtualIterator<TContainerType, TItrType> {

	using Traits = TContainerTraits<TContainerType>;

	constexpr_20 TVirtualIterator(const TItrType& inItr): itr(inItr) {}

	//TODO: verify offset or keep unsafe...

	constexpr_23 decltype(auto) operator*() noexcept { //returns ref
		return *itr;
	}

	constexpr_20 decltype(auto) operator->() noexcept { //returns pointer
		return itr.operator->();
	}

	constexpr_20 decltype(auto) operator[](const int offset) noexcept {
		return itr[offset];
	}

	constexpr_20 decltype(auto) operator[](const int offset) const noexcept {
		return itr[offset];
	}

	constexpr_20 bool operator==(TVirtualIterator otr) {
		return itr == otr.itr;
	}

	constexpr_20 bool operator!=(TVirtualIterator otr) {
		return itr != otr.itr;
	}

	constexpr_20 bool operator<(const TVirtualIterator& otr) const noexcept {
		return itr < otr.itr;
	}

	constexpr_20 bool operator>(const TVirtualIterator& otr) const noexcept {
		return otr < *this;
	}

	constexpr_20 bool operator<=(const TVirtualIterator& otr) const noexcept {
		return !(otr < *this);
	}

	constexpr_20 bool operator>=(const TVirtualIterator& otr) const noexcept {
		return !(*this < otr);
	}

	constexpr TVirtualIterator& operator++() noexcept {
		std::advance(itr, 1);
		return *this;
	}

	constexpr TVirtualIterator operator++(int) noexcept {
		TVirtualIterator otr = *this;
		++*this;
		return otr;
	}

	constexpr TVirtualIterator& operator+=(const int offset) noexcept {
		std::advance(itr, offset);
		return *this;
	}

	constexpr TVirtualIterator operator+(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr += offset;
		return otr;
	}

	constexpr friend TVirtualIterator operator+(const int offset, TVirtualIterator otr) noexcept {
		otr += offset;
		return otr;
	}

	constexpr TVirtualIterator& operator--() noexcept {
		std::advance(itr, -1);
		return *this;
	}

	constexpr TVirtualIterator operator--(int) noexcept {
		TVirtualIterator otr = *this;
		--*this;
		return otr;
	}

	constexpr TVirtualIterator& operator-=(const int offset) noexcept {
		std::advance(itr, -offset);
		return *this;
	}

	constexpr TVirtualIterator operator-(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr -= offset;
		return otr;
	}

	constexpr friend TVirtualIterator operator-(const int offset, TVirtualIterator otr) noexcept {
		otr -= offset;
		return otr;
	}

    TItrType itr;
};

// A basic container of any amount of objects
// A size of 0 implies a dynamic array
template <typename TContainerType>
struct TSequenceContainer : SContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;

	template <typename TOtherType = TType>
	using TSubcontainerType = typename Traits::template SubcontainerType<TOtherType>;

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
	[[nodiscard]] TType& bottom()
	requires (!bIsLimitedAccess) {
		return derived(*this).bottom();
	}
	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] const TType& bottom() const
	requires (!bIsLimitedAccess) {
		return derived(*this).bottom();
	}

	[[nodiscard]] Iterator begin() noexcept
	requires (!bIsLimitedAccess) {
		return derived(*this).begin();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator begin() const noexcept {
		return derived(*this).begin();
	}

	[[nodiscard]] ReverseIterator rbegin() noexcept
	requires (!bIsLimitedAccess && !bIsForwardOnly) {
		return derived(*this).rbegin();
	}

	[[nodiscard]] ConstReverseIterator rbegin() const noexcept
	requires (!bIsLimitedAccess && !bIsForwardOnly) {
		return derived(*this).rbegin();
	}

	[[nodiscard]] Iterator end() noexcept
	requires (!bIsLimitedAccess) {
		return derived(*this).end();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator end() const noexcept {
		return derived(*this).end();
	}

	[[nodiscard]] ReverseIterator rend() noexcept
	requires (!bIsLimitedAccess && !bIsForwardOnly) {
		return derived(*this).rend();
	}

	[[nodiscard]] ConstReverseIterator rend() const noexcept
	requires (!bIsLimitedAccess && !bIsForwardOnly) {
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

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		return derived(*this).containsAll(obj...);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		return derived(*this).containsAll(inFunctions...);
	}

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] bool containsOne(const TOtherType&... obj) {
		return derived(*this).containsOne(obj...);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
		return derived(*this).containsOne(inFunctions...);
	}

	// Find a certain element in the container
	template <typename TOtherType>
	[[nodiscard]] size_t find(const TOtherType& obj) const { return derived(*this).find(obj); }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
		return derived(*this).findFirst(inFunctions...);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
		return derived(*this).findLast(inFunctions...);
	}

	// Get an element at a specified index
	[[nodiscard]] TType& get(size_t index)
	requires (!bIsLimitedAccess) {
		return derived(*this).get(index);
	}
	// Get an element at a specified index
	[[nodiscard]] const TType& get(size_t index) const
	requires (!bIsLimitedAccess) {
		return derived(*this).get(index);
	}

	[[nodiscard]] TType& operator[](const size_t index)
	requires (!bIsLimitedAccess) {
		return get(index);
	}
	[[nodiscard]] const TType& operator[](const size_t index) const
	requires (!bIsLimitedAccess) {
		return get(index);
	}

	// Fills container with n defaulted elements
	void resize(size_t amt) { derived(*this).resize(amt); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, const std::function<TType(size_t)>& func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	void reserve(size_t amt)
	requires bIsContiguousMemory {
		derived(*this).reserve(amt);
	}

	// Adds a defaulted element to the container
	TType& push() { return derived(*this).push(); }
	// Adds an element to the container, returning the index where it was added
	size_t push(const TType& obj) { return derived(*this).push(obj); }
	// Adds an element to the container, returning the index where it was added
	size_t push(TType&& obj) { return derived(*this).push(std::move(obj)); }
	// Inserts an element at a specified index
	void push(size_t index, const TType& obj)
	requires (!bIsLimitedAccess) {
		derived(*this).push(index, obj);
	}
	// Inserts an element at a specified index
	void push(size_t index, TType&& obj)
	requires (!bIsLimitedAccess) {
		derived(*this).push(index, std::move(obj));
	}

	// Replaces an element at a specified index, and returns the original
	void replace(size_t index, const TType& obj)
	requires (!bIsLimitedAccess) {
		derived(*this).push(index, obj);
	}
	// Replaces an element at a specified index, and returns the original
	void replace(size_t index, TType&& obj)
	requires (!bIsLimitedAccess) {
		derived(*this).replace(index, std::move(obj));
	}

	// Removes all elements from the container
	void clear() { derived(*this).clear(); }

	// Removes the topmost element from the container
	void pop() { derived(*this).pop(); }
	// Removes an element at the specified index
	void popAt(size_t index)
	requires (!bIsLimitedAccess) {
		derived(*this).popAt(index);
	}
	// Removes a certain object from the container
	template <typename TOtherType>
	requires (!bIsLimitedAccess)
	void erase(const TOtherType& obj) {
		derived(*this).erase(obj);
	}

	void sort()
	requires (!bIsLimitedAccess && !bIsForwardOnly) {
		derived(*this).sort();
	}

	template <typename Func>
	requires (!bIsLimitedAccess && !bIsForwardOnly)
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

};