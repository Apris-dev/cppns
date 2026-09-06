#pragma once

#include <map>
#include "Container.h"
#include "cppns/util/InitializerList.h"

template <typename TKeyType, typename TValueType
REQUIRES(sutil::is_less_than_comparable_v<TKeyType>)
struct TPriorityMultiMap : TAssociativeContainer<TPriorityMultiMap<TKeyType, TValueType>> {

	using Super = TAssociativeContainer<TPriorityMultiMap>;

	TPriorityMultiMap() = default;

	template <typename TOtherValueType = TValueType
	REQUIRES(std::is_copy_constructible_v<TOtherValueType>)
	TPriorityMultiMap(TInitializerList<TPair<TKeyType, TValueType>> init) {
		for (auto& pair : init) {
			m_Container.emplace(pair.first(), pair.second());
		}
	}

	template <typename... TPairs>
	explicit TPriorityMultiMap(TPairs&&... args) {
		(m_Container.emplace(std::forward<typename TPairs::KeyType>(args.first()), std::forward<typename TPairs::ValueType>(args.second())), ...);
	}

	TPriorityMultiMap(const std::multimap<TKeyType, TValueType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TPair<TKeyType, const TValueType&> top() const {
		return TPair<TKeyType, const TValueType&>{*m_Container.begin()};
	}

	[[nodiscard]] TPair<TKeyType, const TValueType&> bottom() const {
		auto itr = m_Container.end();
		--itr;
		return TPair<TKeyType, const TValueType&>{*itr};
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

	bool isValid(const TKeyType& key) const {
		return ASSOCIATIVE_CONTAINS(m_Container, key);
	}

	template <typename TOtherValueType
	REQUIRES(sutil::is_equality_comparable_v<TValueType, TOtherValueType>)
	bool contains(const TOtherValueType& obj) const {
		return std::find_if(m_Container.begin(), m_Container.end(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		}) != m_Container.end();
	}

	template <typename TOtherValueType
	REQUIRES(sutil::is_equality_comparable_v<TValueType, TOtherValueType>)
	[[nodiscard]] TKeyType find(const TOtherValueType& obj) const {
		return std::find_if(m_Container.begin(), m_Container.end(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		})->first;
	}

	TValueType& get(const TKeyType& key) {
		return m_Container.find(key)->second;
	}

	const TValueType& get(const TKeyType& key) const {
		return m_Container.find(key)->second;
	}

	void resize(const size_t amt, std::function<TPair<TKeyType, TValueType>()> func) {
		for (size_t i = getSize(); i < amt; ++i) {
			TPair<TKeyType, TValueType> pair = func();
			m_Container.emplace(std::forward<TKeyType>(pair.first()), std::forward<TValueType>(pair.second()));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TKeyType> && std::is_default_constructible_v<TValueType>)
	TPair<TKeyType, const TValueType&> push() {
		m_Container.emplace();
		return top();
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key) {
		push(TPair<TKeyType, TValueType>{key, {}});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key, const TValueType& value) {
		push(TPair<TKeyType, TValueType>{key, value});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key, TValueType&& value) {
		push(TPair<TKeyType, TValueType>{key, std::move(value)});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	void push(const TPair<TKeyType, TValueType>& pair) {
		m_Container.emplace(pair.first(), pair.second());
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	void push(TPair<TKeyType, TValueType>&& pair) {
		m_Container.emplace(std::move(pair.first()), std::move(pair.second()));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	void replace(const TKeyType& key, const TValueType& obj) {
		pop(key);
		push(TPair<TKeyType, TValueType>{key, obj});
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	void replace(const TKeyType& key, TValueType&& obj) {
		pop(key);
		push(TPair<TKeyType, TValueType>{key, std::move(obj)});
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		pop(m_Container.begin()->first);
	}

	void pop(const TKeyType& key) {
		m_Container.erase(key);
	}

	template <typename TOtherContainerType>
	void transfer(TAssociativeContainer<TOtherContainerType>& otr, const TKeyType& key) {
		auto itr = m_Container.extract(m_Container.find(key));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TValueType>) {
			otr.push(itr.key(), std::move(itr.mapped()));
		} else {
			otr.push(itr.key(), itr.mapped());
		}
	}

	template <typename TOtherContainerType>
	void append(const TAssociativeContainer<TOtherContainerType>& otr) {
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

	std::multimap<TKeyType, TValueType> m_Container;
};

template <typename TKeyType, typename TValueType>
struct TContainerTraits<TPriorityMultiMap<TKeyType, TValueType>> {
	using KeyType = TKeyType;
	using ValueType = TValueType;
	using SubcontainerType = std::multimap<TKeyType, TValueType>;
	using Iterator = typename SubcontainerType::iterator;
	using ReverseIterator = typename SubcontainerType::reverse_iterator;
	using ConstIterator = typename SubcontainerType::const_iterator;
	using ConstReverseIterator = typename SubcontainerType::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::ASSOCIATIVE;
	constexpr static bool bHasHashing = false;
	constexpr static bool bIsForwardOnly = false;
};

template <typename TKeyType, typename TValueType>
TPriorityMultiMap(TInitializerList<TPair<TKeyType, TValueType>>) -> TPriorityMultiMap<TKeyType, TValueType>;

template <typename TPair, typename... TPairs>
TPriorityMultiMap(TPair, TPairs...) -> TPriorityMultiMap<typename TPair::KeyType, typename TPair::ValueType>;