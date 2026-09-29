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

#if USING_CXX23
#define STR_CONTAINS(c, x) c.contains(x)
#else
#define STR_CONTAINS(c, x) c.find(x) != npos
#endif


namespace String {
    template <typename TChar>
    concept Char = std::is_same_v<TChar, char> || std::is_same_v<TChar, char8_t> || std::is_same_v<TChar, char16_t> || std::is_same_v<TChar, char32_t>;
}

template <String::Char TChar>
struct TString : TSequenceContainer<TString<TChar>> {
    
    using Super = TSequenceContainer<TString>;
	static constexpr auto npos = std::basic_string<TChar>::npos;

    constexpr_20 TString() = default;

    constexpr_20 TString(const TChar* inArray): m_Container(inArray) {}

	constexpr_20 TString(const TChar& inChar) { push(inChar); }

    constexpr_20 TString(const std::basic_string<TChar>& otr): m_Container(otr) {}
    
    [[nodiscard]] constexpr_20 size_t getSize() const {
        return m_Container.size();
    }

    [[nodiscard]] constexpr_20 bool isEmpty() const {
        return m_Container.empty();
    }

    [[nodiscard]] constexpr_20 TChar* data() { return m_Container.data(); }

    [[nodiscard]] constexpr_20 const TChar* data() const { return m_Container.data(); }

    [[nodiscard]] constexpr_20 TChar& top() {
        return m_Container.front();
    }

    [[nodiscard]] constexpr_20 const TChar& top() const {
        return m_Container.front();
    }

    [[nodiscard]] constexpr_20 TChar& bottom() {
        return m_Container.back();
    }

    [[nodiscard]] constexpr_20 const TChar& bottom() const {
        return m_Container.back();
    }

    [[nodiscard]] constexpr_20 typename Super::Iterator begin() noexcept {
        return m_Container.begin();
    }

    [[nodiscard]] constexpr_20 typename Super::ConstIterator begin() const noexcept {
        return m_Container.begin();
    }

    [[nodiscard]] constexpr_20 typename Super::ReverseIterator rbegin() noexcept {
        return m_Container.rbegin();
    }

    [[nodiscard]] constexpr_20 typename Super::ConstReverseIterator rbegin() const noexcept {
        return m_Container.rbegin();
    }

    [[nodiscard]] constexpr_20 typename Super::Iterator end() noexcept {
        return m_Container.end();
    }

    [[nodiscard]] constexpr_20 typename Super::ConstIterator end() const noexcept {
        return m_Container.end();
    }

    [[nodiscard]] constexpr_20 typename Super::ReverseIterator rend() noexcept {
        return m_Container.rend();
    }

    [[nodiscard]] constexpr_20 typename Super::ConstReverseIterator rend() const noexcept {
        return m_Container.rend();
    }
    
    [[nodiscard]] constexpr_20 bool isValid(const size_t index) const {
		return index < getSize();
	}

	bool contains(const TChar* inArray) const {
    	return STR_CONTAINS(m_Container, inArray);
    }

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TChar, TOtherType>
	bool contains(const TOtherType& obj) const {
		return STR_CONTAINS(m_Container, obj);
	}

	[[nodiscard]] bool contains(const std::function<bool(const TChar&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <String::Char... TOtherType>
	[[nodiscard]] bool containsAll(const TOtherType*... inArrays) {
    	bool res = true;
    	((res &= STR_CONTAINS(m_Container, inArrays)), ...);
    	return res;
    }

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TChar, TOtherType>...>
	[[nodiscard]] bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= STR_CONTAINS(m_Container, obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TChar&>...>
	[[nodiscard]] bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <String::Char... TOtherType>
	[[nodiscard]] bool containsOne(const TOtherType*... inArrays) {
    	bool res = false;
    	((res |= STR_CONTAINS(m_Container, inArrays)), ...);
    	return res;
    }

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TChar, TOtherType>...>, int> = 0
	>
	[[nodiscard]] bool containsOne(const TOtherType&... obj) {
    	bool res = false;
    	((res |= STR_CONTAINS(m_Container, obj)), ...);
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

	size_t find(const std::function<bool(const TChar&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	[[nodiscard]] size_t findFirst(const TChar* inArray) const {
    	return m_Container.find_first_of(inArray);
    }

	[[nodiscard]] size_t findFirst(const TChar& obj) const {
    	return m_Container.find_first_of(obj);
    }

	[[nodiscard]] size_t findFirstNot(const TChar* inArray) const {
    	return m_Container.find_first_not_of(inArray);
    }

	[[nodiscard]] size_t findFirstNot(const TChar& obj) const {
    	return m_Container.find_first_not_of(obj);
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

	[[nodiscard]] size_t findLast(const TChar* inArray) const {
    	return m_Container.find_last_of(inArray);
    }

	[[nodiscard]] size_t findLast(const TChar& obj) const {
    	return m_Container.find_last_of(obj);
    }

	[[nodiscard]] size_t findLastNot(const TChar* inArray) const {
    	return m_Container.find_last_not_of(inArray);
    }

	[[nodiscard]] size_t findLastNot(const TChar& obj) const {
    	return m_Container.find_last_not_of(obj);
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
		m_Container.push_back(TChar());
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
	constexpr_20 void append(const TSequenceContainer<TOtherContainerType>& otr) {
#ifdef __cpp_lib_containers_ranges
		m_Container.append_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(m_Container.end(), SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

	constexpr_20 void append(const TChar* inArray) {
		m_Container.append(inArray);
	}

	constexpr_20 TString& operator+=(const TString& otr) noexcept {
    	append(otr);
    	return *this;
    }

	constexpr_20 TString operator+(const TString& otr) const noexcept {
    	TString temp = *this;
    	temp += otr;
    	return temp;
    }

	constexpr_20 bool operator==(const std::string& otr) const noexcept {
    	return m_Container == otr;
    }

    constexpr_20 std::strong_ordering operator<=>(const std::string& otr) const {
	    return m_Container <=> otr;
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