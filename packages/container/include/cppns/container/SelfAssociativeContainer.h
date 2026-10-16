#pragma once

#include "Container.h"

template <typename TContainerType, typename TItrType>
requires (TContainerTraits<TContainerType>::ContainerType == EContainerType::SELF_ASSOCIATIVE)
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

// Designed to be a container without indexing
// Note: always requires a comparable type
/*template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerTraits<TContainerType>::Type, typename TContainerTraits<TContainerType>::Type>, int> = 0
>*/
template <typename TContainerType>
struct TSelfAssociativeContainer : SContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;

	template <typename TOtherType = TType>
	using TSubcontainerType = typename Traits::template SubcontainerType<TOtherType>;

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

	[[nodiscard]] ReverseIterator rbegin() noexcept requires (!bIsForwardOnly) { return derived(*this).rbegin(); }

	[[nodiscard]] ConstReverseIterator rbegin() const noexcept requires (!bIsForwardOnly) { return derived(*this).rbegin(); }

	[[nodiscard]] Iterator end() noexcept { return derived(*this).end(); }

	[[nodiscard]] ConstIterator end() const noexcept { return derived(*this).end(); }

	[[nodiscard]] ReverseIterator rend() noexcept requires (!bIsForwardOnly) { return derived(*this).rend(); }

	[[nodiscard]] ConstReverseIterator rend() const noexcept requires (!bIsForwardOnly) { return derived(*this).rend(); }

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
	void resize(size_t amt, const std::function<TType()>& func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	void reserve(size_t amt) requires bHasHashing{ derived(*this).reserve(amt); }

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
	void erase(const TOtherType& obj) { derived(*this).erase(obj); }

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

};