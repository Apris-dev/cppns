#pragma once

#include "cppns/container/Container.h"

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

template <>
struct TContainerTraits<struct CString> {
	template<typename TType = void>
	using SubcontainerType = std::string;
	using Type = SubcontainerType<>::value_type;
	static constexpr auto npos = SubcontainerType<>::npos;
	using Iterator = typename SubcontainerType<>::iterator;
	using ReverseIterator = typename SubcontainerType<>::reverse_iterator;
	using ConstIterator = typename SubcontainerType<>::const_iterator;
	using ConstReverseIterator = typename SubcontainerType<>::const_reverse_iterator;
	constexpr static auto ContainerType = EContainerType::SEQUENCE;
	constexpr static bool bIsContiguousMemory = true;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = false;
	constexpr static bool bIsLimitedSize = false;
};

struct CString : TSequenceContainer<CString> {

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

public:

    using Super = TSequenceContainer;

    constexpr_20 CString() = default;

    constexpr_20 CString(const TType* inArray): m_Container(inArray) {}

	constexpr_20 CString(const TType& inChar) { push(inChar); }

    constexpr_20 CString(const TSubcontainerType<>& otr): m_Container(otr) {}
    
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

	[[nodiscard]] constexpr_20 bool contains(const TType* inArray) const {
    	return STR_CONTAINS(m_Container, inArray);
    }

	template <typename TOtherType>
	requires sutil::is_equality_comparable_v<TType, TOtherType>
	[[nodiscard]] constexpr_20 bool contains(const TOtherType& obj) const {
		return STR_CONTAINS(m_Container, obj);
	}

	[[nodiscard]] constexpr_20 bool contains(const std::function<bool(const TType&)>& inFunction) {
		return CONTAINS_IF(m_Container, inFunction);
	}

	template <String::Char... TOtherType>
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

	template <String::Char... TOtherType>
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
	}

	constexpr_20 void reserve(const size_t amt) {
		m_Container.reserve(amt);
	}

	constexpr_20 TType& push() {
		m_Container.push_back(TType());
		return get(getSize() - 1);
	}

	constexpr_20 size_t push(const TType& obj) {
		m_Container.push_back(obj);
		return getSize() - 1;
	}

	constexpr_20 size_t push(TType&& obj) {
		m_Container.push_back(std::move(obj));
		return getSize() - 1;
	}

	constexpr_20 void push(const size_t index, const TType& obj) {
		m_Container.insert(m_Container.begin() + index, obj);
	}

	constexpr_20 void push(const size_t index, TType&& obj) {
		m_Container.insert(m_Container.begin() + index, std::move(obj));
	}

	constexpr_20 void replace(const size_t index, const TType& obj) {
		popAt(index);
		push(index, obj);
	}

	constexpr_20 void replace(const size_t index, TType&& obj) {
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
	}

	constexpr_20 void append(const TType* inArray) {
		m_Container.append(inArray);
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
};

#undef STR_CONTAINS