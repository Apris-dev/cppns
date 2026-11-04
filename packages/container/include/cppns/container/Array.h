#pragma once

#include <array>
#include "SequenceContainer.h"
#include "cppns/util/InitializerList.h"

template <typename TType, size_t TSize>
struct TArray : TSequenceContainer<TArray<TType, TSize>> {

	using Super = TSequenceContainer<TArray>;

	template <typename TOtherType = TType>
	using TSubcontainerType = Super::template TSubcontainerType<TOtherType>;

	constexpr_20 TArray() = default;

	template <typename... TArgs>
	requires std::conjunction_v<std::is_constructible<TType, TArgs>...>
	constexpr_20 TArray(TArgs&&... args) {
		if (sizeof...(TArgs) > TSize) {
			throw std::runtime_error("Varargs contains too many elements for TArray!");
		}

		size_t index = 0;
		(arrayArgsInit(std::forward<TArgs>(args), index), ...);
	}

	constexpr_20 TArray(const TSubcontainerType<>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TType* data() { return m_Container.data(); }

	[[nodiscard]] const TType* data() const { return m_Container.data(); }

	[[nodiscard]] TType& top() {
		return m_Container.front();
	}

	[[nodiscard]] const TType& top() const {
		return m_Container.front();
	}

	[[nodiscard]] TType& bottom() {
		return m_Container.back();
	}

	[[nodiscard]] const TType& bottom() const {
		return m_Container.back();
	}

	[[nodiscard]] typename Super::Iterator begin() noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::ConstIterator begin() const noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::ReverseIterator rbegin() noexcept {
		return m_Container.rbegin();
	}

	[[nodiscard]] typename Super::ConstReverseIterator rbegin() const noexcept {
		return m_Container.rbegin();
	}

	[[nodiscard]] typename Super::Iterator end() noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ConstIterator end() const noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ReverseIterator rend() noexcept {
		return m_Container.rend();
	}

	[[nodiscard]] typename Super::ConstReverseIterator rend() const noexcept {
		return m_Container.rend();
	}

	[[nodiscard]] bool isValid(const size_t index) const {
		return index < TSize;
	}

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	bool contains(const TOtherType& obj) const {
		return CONTAINS(m_Container, obj);
	}

	[[nodiscard]] bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= CONTAINS(m_Container, obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] bool containsOne(const TOtherType&... obj) {
		bool res = false;
		((res |= CONTAINS(m_Container, obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
		bool res = false;
		((res |= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	size_t find(const TOtherType& obj) const {
		return DISTANCE(m_Container, obj);
	}

	size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return find(func);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return DISTANCE_LAST_IF(m_Container, func);
	}

	TType& get(size_t index) {
		return m_Container[index];
	}

	const TType& get(size_t index) const {
		return m_Container[index];
	}

	void fill()
	requires std::is_default_constructible_v<TType> {
		m_Container.fill(TType{});
	}

	void fill(const TType& obj) {
		m_Container.fill(obj);
	}

	void fill(const std::function<TType(size_t)>& func) {
		for (size_t i = 0; i < TSize; ++i) {
			replace(i, std::forward<TType>(func(i)));
		}
	}

	void push(const size_t index, const TType& obj)
	requires std::is_copy_constructible_v<TType> {
		replace(index, obj);
	}

	void push(const size_t index, TType&& obj)
	requires std::is_move_constructible_v<TType> {
		replace(index, std::move(obj));
	}

	void replace(const size_t index, const TType& obj)
	requires std::is_copy_constructible_v<TType> {
		m_Container[index] = obj;
	}

	void replace(const size_t index, TType&& obj)
	requires std::is_move_constructible_v<TType> {
		m_Container[index] = std::move(obj);
	}

	// Array is on stack, it is destroyed when out of scope
	// Instead, fill with dummy variables
	void clear()
	requires std::is_default_constructible_v<TType> {
		fill();
	}

	void sort()
	requires sutil::is_less_than_comparable_v<TType> {
		SORT(m_Container);
	}

	template <typename Func>
	void sort(Func&& func) {
		SORT_F(m_Container, std::forward<Func>(func));
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	void arrayArgsInit(const TType& obj, size_t& index) noexcept {
		m_Container[index] = obj;
		index++;
	}

	void arrayArgsInit(TType&& obj, size_t& index) noexcept {
		m_Container[index] = std::move(obj);
		index++;
	}

	TSubcontainerType<> m_Container;
};

template <typename TType, size_t TSize>
struct TContainerTraits<TArray<TType, TSize>> {
	using Type = TType;
	template<typename TOtherType = TType>
	using SubcontainerType = std::array<TOtherType, TSize>;
	using Iterator = typename SubcontainerType<>::iterator;
	using ReverseIterator = typename SubcontainerType<>::reverse_iterator;
	using ConstIterator = typename SubcontainerType<>::const_iterator;
	using ConstReverseIterator = typename SubcontainerType<>::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = true;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = false;
	constexpr static bool bIsLimitedSize = true;
};

template <typename TType, typename... TArgs>
TArray(TType, TArgs...) -> TArray<typename sstl::EnforceConvertible<TType, TArgs...>::Type, sizeof...(TArgs) + 1>;