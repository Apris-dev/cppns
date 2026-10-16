#pragma once

#include "Container.h"
#include "cppns/util/Pair.h"

template <typename TContainerType, typename TItrType>
requires (TContainerTraits<TContainerType>::ContainerType == EContainerType::ASSOCIATIVE)
struct TVirtualIterator<TContainerType, TItrType> {

	using Traits = TContainerTraits<TContainerType>;

	constexpr_20 TVirtualIterator(const TItrType& inItr): itr(inItr) {}

	//TODO: verify offset or keep unsafe...

	constexpr_23 decltype(auto) operator*() noexcept { //returns ref
		using KeyType = typename Traits::KeyType;
		using ValueType = typename Traits::ValueType;
		constexpr static bool isConst = std::is_const_v<std::remove_reference_t<decltype(*itr)>>;
		return reinterpret_cast<std::conditional_t<isConst, const TPair<KeyType, ValueType>&, TPair<KeyType, ValueType>&>>(*itr);
	}

	constexpr_20 decltype(auto) operator->() noexcept { //returns pointer
		using KeyType = typename Traits::KeyType;
		using ValueType = typename Traits::ValueType;
		return reinterpret_cast<TPair<KeyType, ValueType>*>(itr.operator->());
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

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType>
	using TSubcontainerType = typename Traits::template SubcontainerType<TOtherKeyType, TOtherValueType>;

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
	void resize(size_t amt, const std::function<TPair<TKeyType, TValueType>()>& func) { derived(*this).resize(amt, func); }

	// Reserves memory for n elements
	void reserve(size_t amt) requires bHasHashing { derived(*this).reserve(amt); }

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

};