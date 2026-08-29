#pragma once

#include <type_traits>

#if defined(_MSC_VER) || defined(__APPLE__)
#include <utility>
#else
#include <bits/stl_pair.h>
#endif

#ifndef _MSC_VER
template <typename TType>
using implicitly_default_constructible = std::__is_implicitly_default_constructible<TType>;
#else
template <typename TType>
using implicitly_default_constructible = std::_Is_implicitly_default_constructible<TType>;
#endif
// To keep it simpler than the default std::pair, we rewrite it to be more usable.
// Not used internally in maps, just for Container use for implicit conversion and easy usage
template <typename TKeyType, typename TValueType>
struct TPair {

	using KeyType = TKeyType;
	using ValueType = TValueType;

    template <class TOtherKeyType = TKeyType, class TOtherValueType = TValueType,
        std::enable_if_t<std::conjunction_v<std::is_default_constructible<TOtherKeyType>, std::is_default_constructible<TOtherValueType>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<implicitly_default_constructible<TOtherKeyType>, implicitly_default_constructible<TOtherValueType>>)
#endif
	TPair()
	noexcept(std::is_nothrow_default_constructible_v<TOtherKeyType> && std::is_nothrow_default_constructible_v<TOtherValueType>) // strengthened
	: _pair() {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const TPair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: _pair(pair._pair.first, pair._pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(TPair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: _pair(pair._pair.first, pair._pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(TPair<TOtherKeyType, TOtherValueType>&& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: _pair(std::forward<TOtherKeyType>(pair._pair.first), std::forward<TOtherValueType>(pair._pair.second)) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
    constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const std::pair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: _pair(pair._pair.first, pair._pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(std::pair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: _pair(pair._pair.first, pair._pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
    constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(std::pair<TOtherKeyType, TOtherValueType>&& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: _pair(std::forward<TOtherKeyType>(pair._pair.first), std::forward<TOtherValueType>(pair._pair.second)) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
    constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const TOtherKeyType& fst, const TOtherValueType& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: _pair(fst, snd) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(TOtherKeyType& fst, TOtherValueType& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: _pair(fst, snd) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#ifdef __cpp_conditional_explicit
    constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(TOtherKeyType&& fst, TOtherValueType&& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: _pair(std::forward<TOtherKeyType>(fst), std::forward<TOtherValueType>(snd)) {}

	operator std::pair<TKeyType, TValueType>() const {
		return _pair;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, const TOtherKeyType&>, std::is_assignable<TValueType&, const TOtherValueType&>>, int> = 0
	>
	TPair& operator=(const TPair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, const TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, const TOtherValueType&>) {
		_pair.first = otr._pair.first;
		_pair.second = otr._pair.second;
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType&>, std::is_assignable<TValueType&, TOtherValueType&>>, int> = 0
	>
	TPair& operator=(TPair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType&>) {
		_pair.first = otr._pair.first;
		_pair.second = otr._pair.second;
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType>, std::is_assignable<TValueType&, TOtherValueType>>, int> = 0
	>
	TPair& operator=(TPair<TOtherKeyType, TOtherValueType>&& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType>) {
		_pair.first = std::forward<TOtherKeyType>(otr._pair.first);
		_pair.second = std::forward<TOtherValueType>(otr._pair.second);
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, const TOtherKeyType&>, std::is_assignable<TValueType&, const TOtherValueType&>>, int> = 0
	>
	TPair& operator=(const std::pair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, const TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, const TOtherValueType&>) {
    		_pair.first = otr._pair.first;
    		_pair.second = otr._pair.second;
    		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType&>, std::is_assignable<TValueType&, TOtherValueType&>>, int> = 0
	>
	TPair& operator=(std::pair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType&>) {
    		_pair.first = otr._pair.first;
    		_pair.second = otr._pair.second;
    		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType>, std::is_assignable<TValueType&, TOtherValueType>>, int> = 0
	>
	TPair& operator=(std::pair<TOtherKeyType, TOtherValueType>&& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType>) {
    		_pair.first = std::forward<TOtherKeyType>(otr._pair.first);
    		_pair.second = std::forward<TOtherValueType>(otr._pair.second);
    		return *this;
	}

	template <class TOtherKeyType, class TOtherValueType>
	friend constexpr bool operator==(const TPair& lhs, const TPair<TOtherKeyType, TOtherValueType>& rhs) {
	    return lhs._pair.first == rhs._pair.first && lhs._pair.second == rhs._pair.second;
	}

	template <class TOtherKeyType, class TOtherValueType>
	friend constexpr bool operator==(const TPair& lhs, const std::pair<TOtherKeyType, TOtherValueType>& rhs) {
	    return lhs._pair.first == rhs._pair.first && lhs._pair.second == rhs._pair.second;
	}

	auto& first() { return _pair.first; }
	const auto& first() const { return _pair.first; }
	auto& key() { return _pair.first; }
	const auto& key() const { return _pair.first; }
	auto& index() { return _pair.first; }
	const auto& index() const { return _pair.first; }

	auto& second() { return _pair.second; }
	const auto& second() const { return _pair.second; }
	auto& value() { return _pair.second; }
	const auto& value() const { return _pair.second; }
	auto& object() { return _pair.second; }
	const auto& object() const { return _pair.second; }

private:

	std::pair<TKeyType, TValueType> _pair;

	template<typename, typename>
	friend struct TPair;
};

template <typename TKeyType, typename TValueType>
TPair(const std::pair<TKeyType, TValueType>&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(std::pair<TKeyType, TValueType>&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(std::pair<TKeyType, TValueType>&&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(const TKeyType& fst, const TValueType& snd) -> TPair<TKeyType, TValueType>;
