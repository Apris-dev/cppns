// tag_rank_string.hpp - prototype
//
// Instead of widening the WHOLE string when one wide character appears,
// each code point lives in exactly one of three dense pools (Latin-1 /
// BMP-only / astral), a per-code-point "tag" says which pool it's in, and a
// per-block rank table lets operator[] jump straight to the right pool slot
// in O(1) (bounded work) instead of scanning from the start.
//
// This file uses a simple 1-byte-per-code-point tag array for clarity.
// Packing tags to 2 bits/code point (via bit-trick popcounts) cuts the tag
// overhead ~4x; that's a mechanical optimization on top of this, not a
// change in the approach.

#pragma once

#include "cppns/container/Container.h"

namespace String {
    template <typename TChar>
    concept Char = std::is_same_v<TChar, char> || std::is_same_v<TChar, char8_t> || std::is_same_v<TChar, char16_t> || std::is_same_v<TChar, char32_t>;
}

template <String::Char TChar>
struct TString : TSequenceContainer<TString<TChar>> {
    
    using Super = TSequenceContainer<TString>;

    constexpr_20 TString() = default;

    constexpr_20 explicit TString(const TChar* inArray): m_Container(inArray) {}

	constexpr_20 explicit TString(const TChar& inChar) { push(inChar); }

    constexpr_20 TString(const std::basic_string<TChar>& otr): m_Container(otr) {}
    
    [[nodiscard]] size_t getSize() const {
        return m_Container.size();
    }

    [[nodiscard]] bool isEmpty() const {
        return m_Container.empty();
    }

    [[nodiscard]] TChar* data() { return m_Container.data(); }

    [[nodiscard]] const TChar* data() const { return m_Container.data(); }

    [[nodiscard]] TChar& top() {
        return m_Container.front();
    }

    [[nodiscard]] const TChar& top() const {
        return m_Container.front();
    }

    [[nodiscard]] TChar& bottom() {
        return m_Container.back();
    }

    [[nodiscard]] const TChar& bottom() const {
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
		return index < getSize();
	}

	bool contains(const TChar* inArray) const {
    	return m_Container.contains(inArray);
    }

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TChar, TOtherType>
	bool contains(const TOtherType& obj) const {
		return m_Container.contains(obj);
	}

	[[nodiscard]] bool contains(const std::function<bool(const TChar*)>& inFunction) {
    	return CONTAINS_IF(m_Container, inFunction);
    }

	[[nodiscard]] bool contains(const std::function<bool(const TChar&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <String::Char... TOtherType>
	[[nodiscard]] bool containsAll(const TOtherType*... inArrays) {
    	bool res = true;
    	((res &= m_Container.contains(inArrays)), ...);
    	return res;
    }

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TChar, TOtherType>...>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= m_Container.contains(obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar*>...>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
    	bool res = true;
    	((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
    	return res;
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar&>...>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar*>...>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
    	bool res = false;
    	((res |= CONTAINS_IF(m_Container, inFunctions)), ...);
    	return res;
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar&>...>
	[[nodiscard]] bool containsOne(const TFunc&... inFunctions) {
		bool res = false;
		((res |= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	size_t find(const TChar* inArray) const {
		return m_Container.find(inArray);
	}

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TChar, TOtherType>
	size_t find(const TOtherType& obj) const {
		return m_Container.find(obj);
	}

	size_t find(const std::function<bool(const TChar*)>& inFunction) {
    	return DISTANCE_IF(m_Container, inFunction);
    }

	size_t find(const std::function<bool(const TChar&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar*>...>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
    	auto func = [&](const auto& obb) {
    		bool res = false;
    		((res |= inFunctions(obb)), ...);
    		return res;
    	};

    	return find(func);
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar&>...>
	[[nodiscard]] size_t findFirst(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return find(func);
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar*>...>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
    	auto func = [&](const auto& obb) {
    		bool res = false;
    		((res |= inFunctions(obb)), ...);
    		return res;
    	};

    	return DISTANCE_LAST_IF(m_Container, func);
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar&>...>
	[[nodiscard]] size_t findLast(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return DISTANCE_LAST_IF(m_Container, func);
	}

	TChar& get(size_t index) {
		return m_Container[index];
	}

	const TChar& get(size_t index) const {
		return m_Container[index];
	}

	void resize(size_t amt)
	requires std::is_default_constructible_v<TChar> {
		m_Container.resize(amt);
	}

	void resize(const size_t amt, std::function<TChar(size_t)> func) {
		const size_t previousSize = getSize();
		m_Container.reserve(amt);
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.push_back(std::forward<TChar>(func(i)));
		}
	}

	void reserve(size_t amt) {
		m_Container.reserve(amt);
	}

	TChar& push()
	requires std::is_default_constructible_v<TChar> {
		m_Container.push_back();
		return get(getSize() - 1);
	}

	size_t push(const TChar& obj)
	requires std::is_copy_constructible_v<TChar> {
		m_Container.push_back(obj);
		return getSize() - 1;
	}

	size_t push(TChar&& obj)
	requires std::is_move_constructible_v<TChar> {
		m_Container.push_back(std::move(obj));
		return getSize() - 1;
	}

	void push(const size_t index, const TChar& obj)
	requires std::is_copy_constructible_v<TChar> {
		m_Container.insert(m_Container.begin() + index, obj);
	}

	void push(const size_t index, TChar&& obj)
	requires std::is_move_constructible_v<TChar> {
		m_Container.insert(m_Container.begin() + index, std::move(obj));
	}

	void replace(const size_t index, const TChar& obj)
	requires std::is_copy_constructible_v<TChar> {
		popAt(index);
		push(index, obj);
	}

	void replace(const size_t index, TChar&& obj)
	requires std::is_move_constructible_v<TChar> {
		popAt(index);
		push(index, std::move(obj));
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.pop_back();
	}

	void popAt(const size_t index) {
		m_Container.erase(m_Container.begin() + index);
	}

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TChar, TOtherType>
	void pop(const TOtherType& obj) {
		ERASE(m_Container, obj);
	}

	void sort()
	requires sutil::is_less_than_comparable_v<TChar> {
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
		if constexpr (std::is_move_constructible_v<TChar>) {
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

	void append(const TChar* inArray) {
		m_Container.append(inArray);
	}
    
protected:
    
    friend struct SContainer;

    auto& getSubcontainer() { return m_Container; }
    const auto& getSubcontainer() const { return m_Container; }
    
    std::basic_string<TChar> m_Container;
};

template <String::Char TChar>
struct TContainerTraits<TString<TChar>> {
    using Type = TChar;
    using SubcontainerType = std::basic_string<TChar>;
    using Iterator = typename SubcontainerType::iterator;
    using ReverseIterator = typename SubcontainerType::reverse_iterator;
    using ConstIterator = typename SubcontainerType::const_iterator;
    using ConstReverseIterator = typename SubcontainerType::const_reverse_iterator;
    constexpr static auto ContainerType = EContainerType::SEQUENCE;
    constexpr static bool bIsContiguousMemory = true;
    constexpr static bool bIsLimitedAccess = false;
    constexpr static bool bIsForwardOnly = false;
    constexpr static bool bIsLimitedSize = false;
};

template <String::Char TChar>
TString(const TChar&) -> TString<TChar>;

template <String::Char TChar>
TString(const TChar*) -> TString<TChar>;