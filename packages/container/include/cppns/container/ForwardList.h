#pragma once

#include <forward_list>
#include "Container.h"
#include "cppns/util/InitializerList.h"

template <typename TType>
struct TForwardList : TSequenceContainer<TForwardList<TType>> {

	using Super = TSequenceContainer<TForwardList>;

	TForwardList() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TForwardList(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TForwardList(TArgs&&... args) {
		(m_Container.emplace_front(std::forward<TArgs>(args)), ...);
	}

	TForwardList(const std::forward_list<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return SIZE(m_Container);
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TType& top() {
		return m_Container.front();
	}

	[[nodiscard]] const TType& top() const {
		return m_Container.front();
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

	[[nodiscard]] bool isValid(size_t index) const {
		return index < getSize();
	}

	template <typename TOtherType,
		std::enable_if_t<sutil::is_equality_comparable_v<TType, TOtherType>, int> = 0
	>
	bool contains(const TOtherType& obj) const {
		return CONTAINS(m_Container, obj);
	}

	[[nodiscard]] bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= CONTAINS(m_Container, obj)), ...);
		return res;
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
		bool res = false;
		((res |= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename TOtherType,
		std::enable_if_t<sutil::is_equality_comparable_v<TType, TOtherType>, int> = 0
	>
	size_t find(const TOtherType& obj) const {
		return DISTANCE(m_Container, obj);
	}

	size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return find(func);
	}

	template <typename... TFunc,
		std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>, int> = 0
	>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return DISTANCE_LAST_IF(m_Container, func);
	}

	TType& get(size_t index) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
	}

	const TType& get(size_t index) const {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(size_t amt) {
		m_Container.resize(amt);
	}

	void resize(const size_t amt, std::function<TType(size_t)> func) {
		const size_t previousSize = getSize();
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.emplace_front(std::forward<TType>(func(i)));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		m_Container.emplace_front();
		return get(getSize() - 1);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_front(obj);
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_front(std::move(obj));
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const size_t index, const TType& obj) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.insert_after(itr, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(const size_t index, TType&& obj) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.insert_after(itr, std::move(obj));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void replace(const size_t index, const TType& obj) {
		popAt(index);
		push(index, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void replace(const size_t index, TType&& obj) {
		popAt(index);
		push(index, std::move(obj));
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.erase_after(m_Container.before_begin());
	}

	void popAt(const size_t index) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.erase_after(itr);
	}

	template <typename TOtherType,
		std::enable_if_t<sutil::is_equality_comparable_v<TType, TOtherType>, int> = 0
	>
	void pop(const TOtherType& obj) {
		ERASE(m_Container, obj);
	}

	template <typename TOtherContainerType>
	void transfer(TSequenceContainer<TOtherContainerType>& otr, const size_t index) {
		// Prefer move, but copy if not available
		auto& obj = get(index);
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(obj));
		} else {
			otr.push(obj);
		}
		popAt(index);
	}

	// Forward List transfer can use splicing
	void transfer(TSequenceContainer<TForwardList>& list, const size_t index) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		SContainer::derived(list).m_Container.splice_after(SContainer::derived(list).m_Container.before_begin(), m_Container, itr);
	}

	template <typename TOtherContainerType>
	void append(const TSequenceContainer<TOtherContainerType>& otr) {
#ifdef __cpp_lib_containers_ranges
		m_Container.prepend_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert_after(m_Container.before_begin(),  SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	std::forward_list<TType> m_Container;
};

template <typename TType>
struct TContainerTraits<TForwardList<TType>> {
	using Type = TType;
	using SubcontainerType = std::forward_list<TType>;
	using Iterator = typename SubcontainerType::iterator;
	using ReverseIterator = typename SubcontainerType::iterator;
	using ConstIterator = typename SubcontainerType::const_iterator;
	using ConstReverseIterator = typename SubcontainerType::const_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = false;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = true;
	constexpr static bool bIsLimitedSize = false;
};

template <typename TType, typename... TArgs>
TForwardList(TType, TArgs...) -> TForwardList<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
