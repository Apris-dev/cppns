#pragma once

#include <unordered_set>
#include "Container.h"
#include "cppns/util/InitializerList.h"

template <typename TType>
struct TSet : TSelfAssociativeContainer<TSet<TType>> {

	using Super = TSelfAssociativeContainer<TSet>;

	TSet() = default;

	template <typename TOtherType = TType
	REQUIRES(std::is_copy_constructible_v<TOtherType>)
	TSet(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs
	REQUIRES(std::conjunction_v<std::is_constructible<TType, TArgs>...>)
	explicit TSet(TArgs&&... args) {
		m_Container.reserve(sizeof...(TArgs));
		(m_Container.emplace(std::forward<TArgs>(args)), ...);
	}

	TSet(const std::unordered_set<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] const TType& top() const {
		return *m_Container.begin();
	}

	[[nodiscard]] const TType& bottom() const {
		auto itr = m_Container.end();
		std::advance(itr, -1);
		return *itr;
	}

	[[nodiscard]] typename Super::Iterator begin() noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::ConstIterator begin() const noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::Iterator end() noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ConstIterator end() const noexcept {
		return m_Container.end();
	}

	template <typename TOtherType
	REQUIRES(sutil::is_equality_comparable_v<TType, TOtherType>)
	bool contains(const TOtherType& obj) const {
		if constexpr (std::is_same_v<TType, TOtherType>) {
			return ASSOCIATIVE_CONTAINS(m_Container, obj);
		} else {
			return CONTAINS(m_Container, obj);
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(const size_t amt) {
		for (size_t i = getSize(); i < amt; ++i) {
			m_Container.emplace();
		}
	}

	void resize(const size_t amt, std::function<TType()> func) {
		for (size_t i = getSize(); i < amt; ++i) {
			m_Container.emplace(std::forward<TType>(func()));
		}
	}

	void reserve(const size_t amt) {
		m_Container.reserve(amt);
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	const TType& push() {
		m_Container.emplace();
		return top();
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const TType& obj) {
		m_Container.emplace(obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(TType&& obj) {
		m_Container.emplace(std::move(obj));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void replace(const TType& tgt, const TType& obj) {
		// Since this container is unordered, replacing doesn't need to set at the same index
		pop(tgt);
		m_Container.insert(obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void replace(const TType& tgt, TType&& obj) {
		pop(tgt);
		m_Container.insert(std::move(obj));
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.erase(m_Container.begin());
	}

	template <typename TOtherType
	REQUIRES(sutil::is_equality_comparable_v<TType, TOtherType>)
	void pop(const TOtherType& obj) {
		if constexpr (std::is_same_v<TType, TOtherType>) {
			m_Container.erase(obj);
		} else {
			ERASE(m_Container, obj);
		}
	}

	// Moves an object from this to container otr
	template <typename TOtherContainerType, typename TOtherType>
	void transfer(TSelfAssociativeContainer<TOtherContainerType>& otr, TOtherType& obj) {
		if (!this->contains(obj)) return;
		typename decltype(m_Container)::node_type itr;
		if constexpr (std::is_same_v<TType, TOtherType>) {
			itr = m_Container.extract(m_Container.find(obj));
		} else {
			itr = m_Container.extract(FIND(m_Container, obj));
		}
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TOtherType>) {
			otr.push(std::move(itr.value()));
		} else {
			otr.push(itr.value());
		}
	}

	template <typename TOtherContainerType>
	void append(const TSelfAssociativeContainer<TOtherContainerType>& otr) {
#ifdef __cpp_lib_containers_ranges
		m_Container.insert_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	std::unordered_set<TType, TContainerHasher<TType>> m_Container;
};

template <typename TType>
struct TContainerTraits<TSet<TType>> {
	using Type = TType;
	using SubcontainerType = std::unordered_set<TType, TContainerHasher<TType>>;
	using Iterator = typename SubcontainerType::iterator;
	using ReverseIterator = typename SubcontainerType::iterator;
	using ConstIterator = typename SubcontainerType::const_iterator;
	using ConstReverseIterator = typename SubcontainerType::const_iterator;
	constexpr static auto ContainerType = EContainerType::SELF_ASSOCIATIVE;
	constexpr static bool bHasHashing = true;
	constexpr static bool bIsForwardOnly = false;
};

template <typename TType, typename... TArgs>
TSet(TType, TArgs...) -> TSet<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
