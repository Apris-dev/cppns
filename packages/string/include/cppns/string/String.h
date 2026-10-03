#pragma once

#include "cppns/container/Container.h"
#include "cppns/util/ErrorHandling.h"

#if USING_CXX23
	#define STR_CONTAINS(c, x) c.contains(x)
#else
	#define STR_CONTAINS(c, x) c.find(x) != npos
#endif

namespace String {
	using UTF8 = std::u8string;
	using UTF8View = std::u8string_view;
	using UTF16 = std::u16string;
	using UTF16View = std::u16string_view;
	using UTF32 = std::u32string;
	using UTF32View = std::u32string_view;
}

namespace Error::String {
	// Generally only throws for single char values
	// Strings and Char arrays will automatically remove Invalid characters when added
	CREATE_ERROR_TYPE(NonASCIICharacter, "Added Non-ASCII Character Exception", "Tried to Add Non-ASCII character to CString!", Error::Runtime);
	CREATE_ERROR_TYPE(NonUCS2Character, "Added Non-UCS2 Character Exception", "Tried to Add Non-UCS2 character to CString!", Error::Runtime);
	CREATE_ERROR_TYPE(NonUCS4Character, "Added Non-UCS4 Character Exception", "Tried to Add Non-UCS4 character to CString!", Error::Runtime);
}

template <>
struct TContainerTraits<struct CString> {
	template<typename TType = void>
	using SubcontainerType = std::string;
	using SubcontainerTypeView = std::string_view;
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
	using TSubcontainerTypeView = Traits::SubcontainerTypeView;
	constexpr static auto npos = Traits::SubcontainerType<>::npos;

	constexpr_20 CString() = default;

	constexpr_20 CString(const TType* inArray): m_Container(inArray) {
		removeInvalidUTF8(m_Container);
	}

	constexpr_20 CString(const char8_t* inArray): CString(String::UTF8(inArray)) {}

	constexpr_20 CString(const char16_t* inArray): CString(String::UTF16(inArray)) {}

	constexpr_20 CString(const char32_t* inArray): CString(String::UTF32(inArray)) {}

	//TODO: support more char types
	constexpr_20 CString(const TType& inChar) {
		if (!isASCII(inChar))
			throw Error::String::NonASCIICharacter();
		push(inChar);
	}

	constexpr_20 CString(const TSubcontainerTypeView otr): m_Container(otr) {
		removeInvalidUTF8(m_Container);
	}

	constexpr_20 CString(const String::UTF8View otr) {
		const std::string_view view(reinterpret_cast<const char*>(otr.data()), otr.size());
		m_Container = view;
		removeInvalidUTF8(m_Container);
	}

	constexpr_20 CString(const String::UTF16View otr) {
		m_Container.reserve(otr.size());
		for (size_t i = 0; i < otr.size();) {
			const auto res = decodeUTF16(otr, i);
			appendUTF8(m_Container, res.cp);
			i += res.length;
		}
		removeInvalidUTF8(m_Container);
	}

	constexpr_20 CString(const String::UTF32View otr) {
		m_Container.reserve(otr.size());
		for (const char32_t c : otr)
			appendUTF8(m_Container, c);
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

	[[nodiscard]] constexpr_20 bool contains(const char8_t* inArray) const {
		auto* str = reinterpret_cast<const char*>(inArray);
		return STR_CONTAINS(m_Container, str);
	}

	[[nodiscard]] constexpr_20 bool contains(const char16_t* inArray) const {
		const CString str{inArray};
		return STR_CONTAINS(m_Container, str.m_Container);
	}

	[[nodiscard]] constexpr_20 bool contains(const char32_t* inArray) const {
		const CString str{inArray};
		return STR_CONTAINS(m_Container, str.m_Container);
	}

	[[nodiscard]] constexpr_20 bool contains(const TType& obj) const {
		return STR_CONTAINS(m_Container, obj);
	}

	[[nodiscard]] constexpr_20 bool contains(const char8_t& obj) const {
		return STR_CONTAINS(m_Container, obj);
	}

	[[nodiscard]] constexpr_20 bool contains(const char16_t& obj) const {
		if (!isUCS2(obj))
			return false;
		auto [bytes] = encodeUTF8(obj);
		return contains(bytes);
	}

	[[nodiscard]] constexpr_20 bool contains(const char32_t& obj) const {
		auto [bytes] = encodeUTF8(obj);
		return contains(bytes);
	}

	[[nodiscard]] constexpr_23 bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <typename... TOtherType>
	[[nodiscard]] constexpr_20 bool containsAll(const TOtherType*... inArrays) {
		bool res = true;
		((res &= contains(inArrays)), ...);
		return res;
	}

	template <typename... TOtherType>
	requires std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>
	[[nodiscard]] constexpr_20 bool containsAll(const TOtherType&... obj) {
		bool res = true;
		((res &= contains(obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 bool containsAll(const TFunc&... inFunctions) {
		bool res = true;
		((res &= contains(inFunctions)), ...);
		return res;
	}

	template <typename... TOtherType>
	[[nodiscard]] constexpr_20 bool containsOne(const TOtherType*... inArrays) {
		bool res = false;
		((res |= contains(inArrays)), ...);
		return res;
	}

	template <typename... TOtherType,
		std::enable_if_t<std::conjunction_v<sutil::is_equality_comparable<TType, TOtherType>...>, int> = 0
	>
	[[nodiscard]] constexpr_20 bool containsOne(const TOtherType&... obj) {
		bool res = false;
		((res |= contains(obj)), ...);
		return res;
	}

	template <typename... TFunc>
	requires std::conjunction_v<std::is_invocable_r<bool, TFunc, const TType&>...>
	[[nodiscard]] constexpr_20 bool containsOne(const TFunc&... inFunctions) {
		bool res = false;
		((res |= contains(inFunctions)), ...);
		return res;
	}

	[[nodiscard]] constexpr_20 size_t find(const TType* inArray) const {
		return m_Container.find(inArray);
	}

	[[nodiscard]] constexpr_20 size_t find(const char8_t* inArray) const {
		auto* str = reinterpret_cast<const char*>(inArray);
		return m_Container.find(str);
	}

	[[nodiscard]] constexpr_20 size_t find(const char16_t* inArray) const {
		const CString str{inArray};
		return m_Container.find(str.m_Container);
	}

	[[nodiscard]] constexpr_20 size_t find(const char32_t* inArray) const {
		const CString str{inArray};
		return m_Container.find(str.m_Container);
	}

	[[nodiscard]] constexpr_20 size_t find(const TType& obj) const {
		return m_Container.find(obj);
	}

	[[nodiscard]] constexpr_20 size_t find(const char8_t& obj) const {
		return m_Container.find(obj);
	}

	[[nodiscard]] constexpr_20 size_t find(const char16_t& obj) const {
		if (!isUCS2(obj))
			return 0;
		auto [bytes] = encodeUTF8(obj);
		return find(bytes);
	}

	[[nodiscard]] constexpr_20 size_t find(const char32_t& obj) const {
		auto [bytes] = encodeUTF8(obj);
		return find(bytes);
	}

	[[nodiscard]] size_t find(const std::function<bool(const TType&)>& inFunction) {
		return DISTANCE_IF(m_Container, inFunction);
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
		return m_Container.rfind(inArray);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char8_t* inArray) const {
		auto* str = reinterpret_cast<const char*>(inArray);
		return m_Container.rfind(str);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char16_t* inArray) const {
		const CString str{inArray};
		return m_Container.rfind(str.m_Container);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char32_t* inArray) const {
		const CString str{inArray};
		return m_Container.rfind(str.m_Container);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const TType& obj) const {
		return m_Container.rfind(obj);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char8_t& obj) const {
		return m_Container.rfind(obj);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char16_t& obj) const {
		if (!isUCS2(obj))
			return 0;
		auto [bytes] = encodeUTF8(obj);
		return findLast(bytes);
	}

	[[nodiscard]] constexpr_20 size_t findLast(const char32_t& obj) const {
		auto [bytes] = encodeUTF8(obj);
		return findLast(bytes);
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
			throw Error::String::NonASCIICharacter();
		m_Container.push_back(obj);
		return getSize() - 1;
	}

	constexpr_20 size_t push(const char8_t& obj) {
		return push(static_cast<const char&>(obj));
	}

	constexpr_20 size_t push(const char16_t& obj) {
		if (!isUCS2(obj))
			throw Error::String::NonUCS2Character();
		auto [bytes] = encodeUTF8(obj);
		m_Container.push_back(bytes[0]);
		const size_t res = getSize() - 1;
		if (bytes[1] != '\0')
			m_Container.push_back(bytes[1]);
		return res;
	}

	constexpr_20 size_t push(const char32_t& obj) {
		if (!isUCS4(obj))
			throw Error::String::NonUCS4Character();
		auto [bytes] = encodeUTF8(obj);
		m_Container.push_back(bytes[0]);
		const size_t res = getSize() - 1;
		if (bytes[1] != '\0')
			m_Container.push_back(bytes[1]);
		if (bytes[2] != '\0')
			m_Container.push_back(bytes[2]);
		if (bytes[3] != '\0')
			m_Container.push_back(bytes[3]);
		return res;
	}

	constexpr_20 size_t push(TType&& obj) {
		if (!isASCII(obj))
			throw Error::String::NonASCIICharacter();
		m_Container.push_back(std::move(obj));
		return getSize() - 1;
	}

	constexpr_20 size_t push(char8_t&& obj) {
		return push(static_cast<char&&>(obj));
	}

	constexpr_20 void push(const size_t index, const TType& obj) {
		if (!isASCII(obj))
			throw Error::String::NonASCIICharacter();
		m_Container.insert(m_Container.begin() + index, obj);
	}

	constexpr_20 void push(const size_t index, const char8_t& obj) {
		push(index, static_cast<const char&>(obj));
	}

	constexpr_20 void push(const size_t index, const char16_t& obj) {
		if (!isUCS2(obj))
			throw Error::String::NonUCS2Character();
		auto [bytes] = encodeUTF8(obj);
		if (bytes[1] != '\0')
			m_Container.insert(m_Container.begin() + index, bytes[1]);
		m_Container.insert(m_Container.begin() + index, bytes[0]);
	}

	constexpr_20 void push(const size_t index, const char32_t& obj) {
		if (!isUCS4(obj))
			throw Error::String::NonUCS4Character();
		auto [bytes] = encodeUTF8(obj);
		if (bytes[3] != '\0')
			m_Container.insert(m_Container.begin() + index, bytes[3]);
		if (bytes[2] != '\0')
			m_Container.insert(m_Container.begin() + index, bytes[2]);
		if (bytes[1] != '\0')
			m_Container.insert(m_Container.begin() + index, bytes[1]);
		m_Container.insert(m_Container.begin() + index, bytes[0]);
	}

	constexpr_20 void push(const size_t index, TType&& obj) {
		if (!isASCII(obj))
			throw Error::String::NonASCIICharacter();
		m_Container.insert(m_Container.begin() + index, std::move(obj));
	}

	constexpr_20 void push(const size_t index, char8_t&& obj) {
		push(index, static_cast<char&&>(obj));
	}

	constexpr_20 void replace(const size_t index, const TType& obj) {
		if (!isASCII(obj))
			throw Error::String::NonASCIICharacter();
		popAt(index);
		push(index, obj);
	}

	constexpr_20 void replace(const size_t index, const char8_t& obj) {
		replace(index, static_cast<const char&>(obj));
	}

	constexpr_20 void replace(const size_t index, const char16_t& obj) {
		if (!isUCS2(obj))
			throw Error::String::NonUCS2Character();
		popAt(index);
		push(index, obj);
	}

	constexpr_20 void replace(const size_t index, const char32_t& obj) {
		if (!isUCS4(obj))
			throw Error::String::NonUCS4Character();
		popAt(index);
		push(index, obj);
	}

	constexpr_20 void replace(const size_t index, TType&& obj) {
		if (!isASCII(obj))
			throw Error::String::NonASCIICharacter();
		popAt(index);
		push(index, std::move(obj));
	}

	constexpr_20 void replace(const size_t index, char8_t&& obj) {
		replace(index, static_cast<char&&>(obj));
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
		m_Container.erase(m_Container.find(inArray), std::strlen(inArray));
	}

	constexpr_20 void erase(const char8_t* inArray) {
		auto* str = reinterpret_cast<const char*>(inArray);
		erase(str);
	}

	constexpr_20 void erase(const char16_t* inArray) {
		m_Container.erase(find(inArray), std::char_traits<char16_t>::length(inArray) * sizeof(char16_t));
	}

	constexpr_20 void erase(const char32_t* inArray) {
		m_Container.erase(find(inArray), std::char_traits<char32_t>::length(inArray) * sizeof(char32_t));
	}

	constexpr_20 void erase(const TType& obj) {
		m_Container.erase(m_Container.find(obj), 1);
	}

	constexpr_20 void erase(const char8_t& obj) {
		erase(reinterpret_cast<const char&>(obj));
	}

	constexpr_20 void erase(const char16_t& obj) {
		if (!isUCS2(obj))
			return;
		auto [bytes] = encodeUTF8(obj);
		erase(bytes);
	}

	constexpr_20 void erase(const char32_t& obj) {
		auto [bytes] = encodeUTF8(obj);
		erase(bytes);
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

	constexpr_20 void append(const char8_t* inArray) {
		auto* str = reinterpret_cast<const char*>(inArray);
		append(str);
	}

	constexpr_20 void append(const char16_t* inArray) {
		const CString str{inArray};
		m_Container.append(str.m_Container);
	}

	constexpr_20 void append(const char32_t* inArray) {
		const CString str{inArray};
		m_Container.append(str.m_Container);
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

	constexpr_20 bool operator==(const CString& otr) const noexcept {
		return m_Container == otr.m_Container;
	}

	constexpr_20 std::strong_ordering operator<=>(const CString& otr) const {
		return m_Container <=> otr.m_Container;
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

	struct LeadCharInfo16 {
		size_t length;
		uint16 lo, hi;
	};

	// Result of decode
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

	constexpr static LeadCharInfo16 classifyLeadChar16(const uint16 b0) noexcept {
		if (b0 < 0xD800 || b0 > 0xDFFF)
			return {.length = 1, .lo = 0, .hi = 0};
		if (b0 <= 0xDBFF)
			return {.length = 2, .lo = 0xDC00, .hi = 0xDFFF};
		return {.length = 0, .lo = 0, .hi = 0};
	}

	constexpr static bool isASCII(const char c) noexcept {
		return classifyLeadChar(c).length == 1;
	}

	constexpr static bool isUCS2(const char16_t c) noexcept {
		return classifyLeadChar16(c).length <= 2;
	}

	constexpr static bool isUCS4(const char32_t cp) noexcept {
		return cp <= 0x10FFFF && (cp < 0xD800 || cp > 0xDFFF);
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

	static void appendUTF8(std::string& out, char32_t cp) {
		if (!isUCS4(cp)) cp = ReplacementChar;

		if (cp < 0x80) {
			out.push_back(static_cast<char>(cp));
		} else if (cp < 0x800) {
			out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		} else if (cp < 0x10000) {
			out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		} else {
			out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		}
	}

	constexpr static char32_t ReplacementChar = 0xFFFD;

	// Decodes UTF8 into a UTF32 char, assumes std::string_view contains UTF8
	// If non-UTF8 is present, it is replaced with 'ReplacementChar' and marked as invalid
	constexpr static DecodeResult decodeUTF8(const TSubcontainerTypeView str, const size_t i) {
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

	// Decodes UTF16 into a UTF32 char
	constexpr static DecodeResult decodeUTF16(const String::UTF16View s, const size_t i) noexcept {
		if (i >= s.size())
			return {.cp = ReplacementChar, .length = 0, .valid = false};

		const char16_t b0 = s[i];
		const auto info = classifyLeadChar16(b0);

		// Invalid
		if (info.length == 0)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// Codepoint fits inside the 2 bytes, ie: not surrogate
		if (info.length == 1)
			return {.cp = b0, .length = 1, .valid = true};

		// First half with no second half
		if (i + 1 >= s.size())
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// First half and second half cannot fit together
		const char16_t second = s[i + 1];
		if (second < info.lo || second > info.hi)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// Add the two halves together
		const char32_t cp = 0x10000 + ((static_cast<char32_t>(b0) - 0xD800) << 10)
									+ (static_cast<char32_t>(second) - 0xDC00);

		return {.cp = cp, .length = 2, .valid = true};
	}

	struct EncodedChar {
		char bytes[5];
	};

	constexpr static EncodedChar encodeUTF8(const char32_t cp) noexcept {
		if (cp < 0x80)
			return {static_cast<char>(cp)};
		if (cp < 0x800)
			return {static_cast<char>(0xC0 | (cp >> 6)),
				static_cast<char>(0x80 | (cp & 0x3F)),
				'\0'};
		if (cp < 0x10000)
			return {static_cast<char>(0xE0 | (cp >> 12)),
				static_cast<char>(0x80 | ((cp >> 6) & 0x3F)),
				static_cast<char>(0x80 | (cp & 0x3F)),
				'\0'};

		return {static_cast<char>(0xF0 | (cp >> 18)),
			static_cast<char>(0x80 | ((cp >> 12) & 0x3F)),
			static_cast<char>(0x80 | ((cp >> 6) & 0x3F)),
			static_cast<char>(0x80 | (cp & 0x3F)),
			'\0'};
	}

	constexpr static EncodedChar encodeUTF8(const char16_t cp) noexcept {
		return encodeUTF8(static_cast<char32_t>(cp));
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