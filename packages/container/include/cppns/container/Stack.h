#pragma once

#include "Deque.h"

// std::stack is already a wrapper around a deque, so we do that here too, and skip std::stack's own abstraction
template <typename TType>
struct TStack : TSequenceContainer<TStack<TType>> {

	using Super = TSequenceContainer<TStack>;

	TStack() = default;

	template <typename TOtherType = TType
	REQUIRES(std::is_copy_constructible_v<TOtherType>)
	TStack(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs
	REQUIRES(std::conjunction_v<std::is_constructible<TType, TArgs>...>)
	explicit TStack(TArgs&&... args) {
		(m_Container.emplace_back(std::forward<TArgs>(args)), ...);
	}
	
	TStack(const std::deque<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TType& top() {
		return m_Container.back();
	}

	[[nodiscard]] const TType& top() const {
		return m_Container.back();
	}

	[[nodiscard]] typename Super::ConstIterator begin() const noexcept { return m_Container.begin(); }

	[[nodiscard]] typename Super::ConstIterator end() const noexcept { return m_Container.end(); }

	[[nodiscard]] bool isValid(size_t index) const {
		return index < getSize();
	}

	template <typename TOtherType
	REQUIRES(sutil::is_equality_comparable_v<TType, TOtherType>)
	bool contains(const TOtherType& obj) const {
		return CONTAINS(m_Container, obj);
	}

	[[nodiscard]] bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <typename TOtherType
	REQUIRES(sutil::is_equality_comparable_v<TType, TOtherType>)
	size_t find(const TOtherType& obj) const {
		return DISTANCE(m_Container, obj);
	}

	size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
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
		return top();
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_back(obj);
		return 0;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_back(std::move(obj));
		return 0;
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.pop_back();
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
struct TContainerTraits<TStack<TType>> {
	using Type = TType;
	using SubcontainerType = std::deque<TType>;
	using Iterator = typename SubcontainerType::iterator;
	using ReverseIterator = typename SubcontainerType::reverse_iterator;
	using ConstIterator = typename SubcontainerType::const_iterator;
	using ConstReverseIterator = typename SubcontainerType::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = false;
	constexpr static bool bIsLimitedAccess = true;
	constexpr static bool bIsForwardOnly = true;
	constexpr static bool bIsLimitedSize = false;
};

template <typename TType, typename... TArgs>
TStack(TType, TArgs...) -> TStack<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
