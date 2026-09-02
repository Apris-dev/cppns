#pragma once

#include <deque>
#include "Container.h"
#include "cppns/util/InitializerList.h"

template <typename TType>
struct TDeque : TSequenceContainer<TDeque<TType>> {

	using Super = TSequenceContainer<TDeque>;

	TDeque() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TDeque(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TDeque(TArgs&&... args) {
		(m_Container.emplace_back(std::forward<TArgs>(args)), ...);
	}

	TDeque(const std::deque<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
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

	template <typename TOtherType,
		std::enable_if_t<sutil::is_equality_comparable_v<TType, TOtherType>, int> = 0
	>
	size_t find(const TOtherType& obj) const {
		return DISTANCE(m_Container, obj);
	}

	size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	TType& get(size_t index) {
		return m_Container[index];
	}

	const TType& get(size_t index) const {
		return m_Container[index];
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(size_t amt) {
		m_Container.resize(amt);
	}

	void resize(const size_t amt, std::function<TType(size_t)> func) {
		const size_t previousSize = getSize();
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.emplace_back(std::forward<TType>(func(i)));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		m_Container.emplace_back();
		return get(getSize() - 1);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_back(obj);
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_back(std::move(obj));
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const size_t index, const TType& obj) {
		m_Container.insert(m_Container.begin() + index, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(const size_t index, TType&& obj) {
		m_Container.insert(m_Container.begin() + index, std::move(obj));
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
		m_Container.pop_front();
	}

	void popAt(const size_t index) {
		m_Container.erase(m_Container.begin() + index);
	}

	template <typename TOtherType,
		std::enable_if_t<sutil::is_equality_comparable_v<TType, TOtherType>, int> = 0
	>
	void pop(const TOtherType& obj) {
		ERASE(m_Container, obj);
	}

	ENABLE_FUNC_IF(sutil::is_less_than_comparable_v<TType>)
	void sort() {
		std::sort(m_Container.begin(), m_Container.end());
	}

	template <typename Func>
	void sort(Func&& func) {
		std::sort(m_Container.begin(), m_Container.end(), std::forward<Func>(func));
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

	template <typename TOtherContainerType>
	void append(const TSequenceContainer<TOtherContainerType>& otr) {
#ifdef __cpp_lib_containers_ranges
		m_Container.append_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(m_Container.end(), SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	std::deque<TType> m_Container;
};

template <typename TType>
struct TContainerTraits<TDeque<TType>> {
	using Type = TType;
	using SubcontainerType = std::deque<TType>;
	using Iterator = typename SubcontainerType::iterator;
	using ReverseIterator = typename SubcontainerType::reverse_iterator;
	using ConstIterator = typename SubcontainerType::const_iterator;
	using ConstReverseIterator = typename SubcontainerType::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = false;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = false;
	constexpr static bool bIsLimitedSize = false;
};

template <typename TType, typename... TArgs>
TDeque(TType, TArgs...) -> TDeque<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
