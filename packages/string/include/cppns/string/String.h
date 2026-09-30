#pragma once

#include "cppns/container/Container.h"
#include "cppns/util/ErrorHandling.h"

#if USING_CXX23
	#define STR_CONTAINS(c, x) c.contains(x)
#else
	#define STR_CONTAINS(c, x) c.find(x) != npos
#endif

namespace String {
	//TODO: add easy conversions to each (including char arrays)
	using UTF8 = std::u8string;
	using UTF16 = std::u16string;
	using UTF32 = std::u32string;
}

namespace Error::String {
	// Generally only throws for single char values
	// Strings and Char arrays will automatically remove UTF8 characters when added
	CREATE_ERROR_TYPE(NonUTF8Character, "Added Non-UTF8 Character Exception", "Tried to Add Non-UTF8 character to CString!", Error::Runtime);
}

template <>
struct TContainerTraits<struct CString> {
	template<typename TType = void>
	using SubcontainerType = std::string;
	using Type = SubcontainerType<>::value_type;
	using Iterator = SubcontainerType<>::iterator;
	using ReverseIterator = SubcontainerType<>::reverse_iterator;
	using ConstIterator = SubcontainerType<>::const_iterator;
	using ConstReverseIterator = SubcontainerType<>::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = true;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = false;
	constexpr static bool bIsLimitedSize = false;
};

// Provides Weak UTF8 guarantee
// Non-UTF8 can still be added through direct data access like data() or get()
// But is unlikely to actually happen
struct CString : TSequenceContainer<CString> {

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

public:

    using Super = TSequenceContainer;
	constexpr static auto npos = Traits::SubcontainerType<>::npos;

    constexpr_20 CString() = default;

    constexpr_20 CString(const TType* inArray): m_Container(inArray) {
	    removeInvalidUTF8(m_Container);
    }

	//TODO: support more char types
	constexpr_20 CString(const TType& inChar) {
    	if (!isASCII(inChar))
    		throw Error::String::NonUTF8Character();
    	push(inChar);
    }

    constexpr_20 CString(const TSubcontainerType<>& otr): m_Container(otr) {
	    removeInvalidUTF8(m_Container);
    }
    
    [[nodiscard]] constexpr_20 size_t getSize() const {
        return m_Container.size();
    }

    [[nodiscard]] constexpr_20 bool isEmpty() const {
        return m_Container.empty();
    }

    [[nodiscard]] constexpr_20 TType* data() { return m_Container.data(); }

    [[nodiscard]] constexpr_20 const TType* data() const { return m_Container.data(); }

    [[nodiscard]] constexpr_20 TType& top() {
        return m_Container.front();
    }

    [[nodiscard]] constexpr_20 const TType& top() const {
        return m_Container.front();
    }

    [[nodiscard]] constexpr_20 TType& bottom() {
        return m_Container.back();
    }

    [[nodiscard]] constexpr_20 const TType& bottom() const {
        return m_Container.back();
    }

    [[nodiscard]] constexpr_20 Iterator begin() noexcept {
        return m_Container.begin();
    }

    [[nodiscard]] constexpr_20 ConstIterator begin() const noexcept {
        return m_Container.begin();
    }

    [[nodiscard]] constexpr_20 ReverseIterator rbegin() noexcept {
        return m_Container.rbegin();
    }

    [[nodiscard]] constexpr_20 ConstReverseIterator rbegin() const noexcept {
        return m_Container.rbegin();
    }

    [[nodiscard]] constexpr_20 Iterator end() noexcept {
        return m_Container.end();
    }

    [[nodiscard]] constexpr_20 ConstIterator end() const noexcept {
        return m_Container.end();
    }

    [[nodiscard]] constexpr_20 ReverseIterator rend() noexcept {
        return m_Container.rend();
    }

    [[nodiscard]] constexpr_20 ConstReverseIterator rend() const noexcept {
        return m_Container.rend();
    }
    
    [[nodiscard]] constexpr_20 bool isValid(const size_t index) const {
		return index < getSize();
	}

	[[nodiscard]] constexpr_20 bool contains(const TType* inArray) const {
    	return STR_CONTAINS(m_Container, inArray);
    }

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	[[nodiscard]] constexpr_20 bool contains(const TOtherType& obj) const {
		return STR_CONTAINS(m_Container, obj);
	}

	[[nodiscard]] constexpr_23 bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <typename... TOtherType>
	[[nodiscard]] constexpr_20 bool containsAll(const TOtherType*... inArrays) {
    	bool res = true;
    	((res &= STR_CONTAINS(m_Container, inArrays)), ...);
    	return res;
    }

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>
	[[nodiscard]] constexpr_20 bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= STR_CONTAINS(m_Container, obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	template <typename... TOtherType>
	[[nodiscard]] constexpr_20 bool containsOne(const TOtherType*... inArrays) {
    	bool res = false;
    	((res |= STR_CONTAINS(m_Container, inArrays)), ...);
    	return res;
    }

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] constexpr_20 bool containsOne(const TOtherType&... obj) {
    	bool res = false;
    	((res |= STR_CONTAINS(m_Container, obj)), ...);
    	return res;
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 bool containsOne(const TFunc&... inFunctions) {
		bool res = false;
		((res |= CONTAINS_IF(m_Container, inFunctions)), ...);
		return res;
	}

	[[nodiscard]] constexpr_20 size_t find(const TType* inArray) const {
		return m_Container.find(inArray);
	}

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	[[nodiscard]] constexpr_20 size_t find(const TOtherType& obj) const {
		return m_Container.find(obj);
	}

	[[nodiscard]] size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
	}

	[[nodiscard]] constexpr_20 size_t findFirst(const TType* inArray) const {
    	return m_Container.find_first_of(inArray);
    }

	[[nodiscard]] constexpr_20 size_t findFirst(const TType& obj) const {
    	return m_Container.find_first_of(obj);
    }

	[[nodiscard]] constexpr_20 size_t findFirstNot(const TType* inArray) const {
    	return m_Container.find_first_not_of(inArray);
    }

	[[nodiscard]] constexpr_20 size_t findFirstNot(const TType& obj) const {
    	return m_Container.find_first_not_of(obj);
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 size_t findFirst(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return find(func);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const TType* inArray) const {
    	return m_Container.find_last_of(inArray);
    }

	[[nodiscard]] constexpr_20 size_t findLast(const TType& obj) const {
    	return m_Container.find_last_of(obj);
    }

	[[nodiscard]] constexpr_20 size_t findLastNot(const TType* inArray) const {
    	return m_Container.find_last_not_of(inArray);
    }

	[[nodiscard]] constexpr_20 size_t findLastNot(const TType& obj) const {
    	return m_Container.find_last_not_of(obj);
    }

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 size_t findLast(const TFunc&... inFunctions) {
		auto func = [&](const auto& obb) {
			bool res = false;
			((res |= inFunctions(obb)), ...);
			return res;
		};

		return DISTANCE_LAST_IF(m_Container, func);
	}

	[[nodiscard]] constexpr_20 TType& get(const size_t index) {
		return m_Container[index];
	}

	[[nodiscard]] constexpr_20 const TType& get(const size_t index) const {
		return m_Container[index];
	}

	constexpr_20 void resize(const size_t amt) {
		m_Container.resize(amt);
	}

	constexpr_20 void resize(const size_t amt, const std::function<TType(size_t)>& func) {
		const size_t previousSize = getSize();
		m_Container.reserve(amt);
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.push_back(std::forward<TType>(func(i)));
		}
	    removeInvalidUTF8(m_Container);
	}

	constexpr_20 void reserve(const size_t amt) {
		m_Container.reserve(amt);
	}

	constexpr_20 TType& push() {
		m_Container.push_back(TType());
		return get(getSize() - 1);
	}

	constexpr_20 size_t push(const TType& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		m_Container.push_back(obj);
		return getSize() - 1;
	}

	constexpr_20 size_t push(TType&& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		m_Container.push_back(std::move(obj));
		return getSize() - 1;
	}

	constexpr_20 void push(const size_t index, const TType& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		m_Container.insert(m_Container.begin() + index, obj);
	}

	constexpr_20 void push(const size_t index, TType&& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		m_Container.insert(m_Container.begin() + index, std::move(obj));
	}

	constexpr_20 void replace(const size_t index, const TType& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		popAt(index);
		push(index, obj);
	}

	constexpr_20 void replace(const size_t index, TType&& obj) {
    	if (!isASCII(obj))
    		throw Error::String::NonUTF8Character();
		popAt(index);
		push(index, std::move(obj));
	}

	constexpr_20 void clear() {
		m_Container.clear();
	}

	constexpr_20 void pop() {
		m_Container.pop_back();
	}

	constexpr_20 void popAt(const size_t index) {
		m_Container.erase(m_Container.begin() + index);
	}

	constexpr_20 void erase(const TType* inArray) {
    	m_Container.erase(m_Container.find(inArray));
    }

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	constexpr_20 void erase(const TOtherType& obj) {
    	m_Container.erase(m_Container.find(obj));
	}

	constexpr_20 void sort() {
    	SORT(m_Container);
	}

	template <typename Func>
	constexpr_20 void sort(Func&& func) {
    	SORT_F(m_Container, std::forward<Func>(func));
	}

	template <typename TOtherContainerType>
	constexpr_20 void transfer(TSequenceContainer<TOtherContainerType>& otr, const size_t index) {
		// Prefer move, but copy if not available
		auto& obj = get(index);
    	otr.push(std::move(obj));
		popAt(index);
	}

	template <typename TOtherContainerType>
	constexpr_20 void append(const TSequenceContainer<TOtherContainerType>& otr) {
#ifdef __cpp_lib_containers_ranges
		m_Container.append_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(m_Container.end(), SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
    	removeInvalidUTF8(m_Container);
	}

	constexpr_20 void append(const TType* inArray) {
    	std::string str = inArray;
    	removeInvalidUTF8(str);
		m_Container.append(str);
	}

	constexpr_20 CString& operator+=(const CString& otr) noexcept {
    	append(otr);
    	return *this;
    }

	constexpr_20 CString operator+(const CString& otr) const noexcept {
    	CString temp = *this;
    	temp += otr;
    	return temp;
    }

	constexpr_20 bool operator==(const TSubcontainerType<>& otr) const noexcept {
    	return m_Container == otr;
    }

    constexpr_20 std::strong_ordering operator<=>(const TSubcontainerType<>& otr) const {
	    return m_Container <=> otr;
    }

protected:
	TSubcontainerType<> m_Container;

private:

	/*
	 * Conversions
	 * TODO: move away when doing numeric conversions
	 */

	// A lead char is when the first char in a codepoint
	// In the case of ASCII, it is also the only char
	// lo and hi are the ranges that are possible for the second byte
	struct LeadCharInfo {
		size_t length;
		uint8 lo, hi;
	};

	// Result of decodeUTF8
	struct DecodeResult {
		char32_t cp;
		size_t length;
		bool valid;
	};

	constexpr static LeadCharInfo classifyLeadChar(const uint8 b0) noexcept {
		if (b0 < 0x80)
			return {.length = 1, .lo = 0x80, .hi = 0xBF};
		if (b0 < 0xC2)
			return {.length = 0, .lo = 0, .hi = 0};
		if (b0 <= 0xDF)
			return {.length = 2, .lo = 0x80, .hi = 0xBF};
		if (b0 == 0xE0)
			return {.length = 3, .lo = 0xA0, .hi = 0xBF};
		if (b0 == 0xED)
			return {.length = 3, .lo = 0x80, .hi = 0x9F};
		if (b0 <= 0xEF)
			return {.length = 3, .lo = 0x80, .hi = 0xBF};
		if (b0 == 0xF0)
			return {.length = 4, .lo = 0x90, .hi = 0xBF};
		if (b0 == 0xF4)
			return {.length = 4, .lo = 0x80, .hi = 0x8F};
		if (b0 <= 0xF3)
			return {.length = 4, .lo = 0x80, .hi = 0xBF};
		return {.length = 0, .lo = 0, .hi = 0};
	}

	constexpr static bool isASCII(const char c) noexcept {
		return classifyLeadChar(c).length == 1;
	}

	// Remove invalid UTF8
	constexpr static void removeInvalidUTF8(std::string& s) noexcept {
		const size_t n = s.size();
		// where we read from and where the next good byte goes
		size_t r = 0, w = 0;

		while (r < n) {
			const DecodeResult ch = decodeUTF8(s, r);

			if (ch.valid) {
				// Only copy if something was deleted
				if (w != r) {
					for (size_t k = 0; k < ch.length; ++k) {
						s[w + k] = s[r + k];
					}
				}
				w += ch.length;
			}
			// Continue reading past regardless of validity
			r += ch.length;
		}

		// Cut off the leftover tail
		s.resize(w);
	}

	constexpr static char32_t ReplacementChar = 0xFFFD;

	// Decodes UTF8 into a UTF32 char, assumes std::string_view contains UTF8
	// If non-UTF8 is present, it is replaced with 'ReplacementChar' and marked as invalid
	constexpr static DecodeResult decodeUTF8(const std::string_view str, const size_t& i) {
		if (i >= str.size())
				return {.cp = ReplacementChar, .length = 0, .valid = false};

		const auto b0 = static_cast<unsigned char>(str[i]);
		const auto [length, lo, hi] = classifyLeadChar(b0);

		if (length == 0)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		if (length == 1)
			return {.cp = b0, .length = 1, .valid = true};

		// Keep the useful bits of the first byte (the rest just says the length).
		char32_t cp = b0 & (0x7F >> length);

		for (size_t k = 1; k < length; ++k) {
			if (i + k >= str.size())
				return {.cp = ReplacementChar, .length = k, .valid = false};

			const auto b = static_cast<unsigned char>(str[i + k]);

			if (!(k == 1 ? b >= lo && b <= hi : (b & 0xC0) == 0x80))
				return {.cp = ReplacementChar, .length = k, .valid = false};

			// Add this byte's 6 bits
			cp = (cp << 6) | (b & 0x3F);
		}

		return {.cp = cp, .length = length, .valid = true};
	}

public:

	// Since CString guarantees UTF8, the conversion is simple
	String::UTF8 toUTF8() const {
		return String::UTF8(m_Container.begin(), m_Container.end());
	}

	String::UTF16 toUTF16() const {
		String::UTF16 str;
		str.reserve(getSize());  // upper bound in code units
		for (size_t i = 0; i < getSize();) {
			const auto res = decodeUTF8(m_Container, i);
			if (res.cp < 0x10000) {
				str.push_back(static_cast<char16_t>(res.cp));
			} else {
				const char32_t v = res.cp - 0x10000;
				str.push_back(static_cast<char16_t>(0xD800 + (v  >> 10)));
				str.push_back(static_cast<char16_t>(0xDC00 + (v & 0x3FF)));
			}
			i += res.length;
		}
		return str;
	}

	String::UTF32 toUTF32() const {
		String::UTF32 str;
		str.reserve(getSize());
		for (size_t i = 0; i < getSize();) {
			const auto res = decodeUTF8(m_Container, i);
			str.push_back(res.cp);
			i += res.length;
		}
		return str;
	}

};

#undef STR_CONTAINS