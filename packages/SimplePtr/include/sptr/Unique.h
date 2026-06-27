#pragma once

#ifndef _MSC_VER
#include <bits/unique_ptr.h>
#else
#include <memory>
#endif

#include "sptr/Common.h"

template <typename TType>
struct TUnique {

	_CONSTEXPR23 TUnique(std::unique_ptr<TType, sstl::delayed_deleter<TType>>&& ptr) noexcept
	: m_ptr(std::move(ptr)) {}

	_CONSTEXPR23 TUnique() noexcept {
		// If not default constructible, default to nullptr
		if constexpr (std::is_default_constructible_v<TType>) {
			m_ptr = std::unique_ptr<TType, sstl::delayed_deleter<TType>>(new TType(), sstl::delayed_deleter<TType>(&sstl::delete_impl<TType>));
			if constexpr (sstl::is_initializable_v<TType>) {
				m_ptr->init();
			}
		}
	}

	_CONSTEXPR23 TUnique(std::nullptr_t) noexcept {}

	_CONSTEXPR23 TUnique& operator=(std::nullptr_t) noexcept {
		m_ptr = nullptr;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 explicit TUnique(TOtherType* ptr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(ptr, sstl::delayed_deleter<TOtherType>(&sstl::delete_impl<TOtherType>)) {}

	template <typename... TArgs,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_null_pointer<std::decay_t<TArgs>>>...,
				std::negation<sstl::is_managed<TArgs>>...
			>,
			int> = 0
	>
	_CONSTEXPR23 explicit TUnique(TArgs&&... args) noexcept {
		m_ptr = std::unique_ptr<TType, sstl::delayed_deleter<TType>>(new TType(std::forward<TArgs>(args)...), sstl::delayed_deleter<TType>(&sstl::delete_impl<TType>));
		if constexpr (sstl::is_initializable_v<TType>) {
			m_ptr->init();
		}
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique(const TUnique<TOtherType>&) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique(TUnique<TOtherType>&) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique(TUnique<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(std::move(otr.m_ptr)) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique& operator=(const TUnique<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique& operator=(TUnique<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 TUnique& operator=(TUnique<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		this->m_ptr = std::move(otr.m_ptr);
		return *this;
	}

	// Releases ownership of the pointer, in this case it will be destroyed
	void destroy() noexcept {
		m_ptr.reset();
	}

	template <typename TOtherType>
	_CONSTEXPR23 TOtherType* staticCast() const noexcept {
		return static_cast<TOtherType*>(this->m_ptr.get());
	}

	template <typename TOtherType>
	_CONSTEXPR23 TOtherType* dynamicCast() const noexcept {
		return dynamic_cast<TOtherType*>(this->m_ptr.get());
	}

	template <typename TOtherType>
	_CONSTEXPR23 TOtherType* reinterpretCast() const noexcept {
		return reinterpret_cast<TOtherType*>(this->m_ptr.get());
	}

	template <typename TOtherType>
	_CONSTEXPR23 TOtherType* constCast() const noexcept {
		return const_cast<TOtherType*>(this->m_ptr.get());
	}

	_CONSTEXPR23 TType* operator->() const noexcept {
		return m_ptr.get();
	}

	_CONSTEXPR23 TType& operator*() const noexcept {
		return *m_ptr.get();
	}

	_CONSTEXPR23 TType* get() const noexcept { return m_ptr.get(); }

	_CONSTEXPR23 operator bool() const noexcept {
		return static_cast<bool>(m_ptr);
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	_CONSTEXPR23 friend bool operator<(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() < snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	_CONSTEXPR23 friend bool operator<=(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() <= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	_CONSTEXPR23 friend bool operator>(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() > snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	_CONSTEXPR23 friend bool operator>=(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() >= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	_CONSTEXPR23 friend bool operator==(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() == snd.get();
	}

	// Compare raw pointer
	_CONSTEXPR23 friend bool operator==(const TUnique& fst, const void* snd) noexcept {
		return fst.get() == snd;
	}

	// Compare raw pointer
	_CONSTEXPR23 friend bool operator==(const void* fst, const TUnique& snd) noexcept {
		return snd == fst;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator!=(const TUnique& fst, const TOtherType& snd) noexcept {
		return fst.get() != snd.get();
	}

	// Compare raw pointer
	_CONSTEXPR23 friend bool operator!=(const TUnique& fst, const void* snd) noexcept {
		return fst.get() != snd;
	}

	// Compare raw pointer
	friend bool operator!=(const void* fst, const TUnique& snd) noexcept {
		return snd != fst;
	}

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TUnique& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TUnique& inValue) {
		inArchive << *inValue.get();
		return inArchive;
	}
#endif

private:
	template <typename>
	friend struct TUnique;

	template <typename>
	friend struct TShared;

	template <typename>
	friend struct TWeak;

	template <typename>
	friend struct TFrail;

	std::unique_ptr<TType, sstl::delayed_deleter<TType>> m_ptr = nullptr;

};

template <typename TType>
struct TUnfurled<TUnique<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TUnique<TType>::get;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 static TUnique<TType> create(TArgs&&... args)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		return TUnique<TOtherType>(std::forward<TArgs>(args)...);
	}
};

#ifndef USING_SIMPLEARCHIVE
template<typename TType>
struct std::hash<TUnique<TType>> {
	size_t operator()(const TUnique<TType>& obj) const noexcept {
		std::hash<std::unique_ptr<TType>> ptrHash;
		return ptrHash(obj.m_ptr);
	}
};
#endif

// Template argument deduction for input of a single type
template <typename TType>
TUnique(TType) -> TUnique<TType>;