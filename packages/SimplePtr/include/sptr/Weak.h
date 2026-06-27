#pragma once

#include "sptr/Common.h"
#include "sptr/Shared.h"

template <typename TType>
struct TWeak {

	TWeak(const std::weak_ptr<TType>& ptr) noexcept
	: m_ptr(ptr) {}

	TWeak(std::weak_ptr<TType>& ptr) noexcept
	: m_ptr(ptr) {}

	TWeak(std::weak_ptr<TType>&& ptr) noexcept
	: m_ptr(std::move(ptr)) {}

	template <typename TOtherType>
	TWeak(const TShared<TOtherType>& shared) noexcept
	: m_ptr(shared.m_ptr) {}

	template <typename TOtherType>
	TWeak(TShared<TOtherType>& shared) noexcept
	: m_ptr(shared.m_ptr) {}

	template <typename TOtherType>
	TWeak(const std::shared_ptr<TOtherType>& shared) noexcept
	: m_ptr(shared) {}

	template <typename TOtherType>
	TWeak(std::shared_ptr<TOtherType>& shared) noexcept
	: m_ptr(shared) {}

	TWeak() = default;

	TWeak(std::nullptr_t) noexcept {}

	TWeak& operator=(std::nullptr_t) noexcept {
		m_ptr.reset();
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak(const TWeak<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak(TWeak<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	TWeak(const TWeak& otr) noexcept
	: m_ptr(otr.m_ptr) {}

	TWeak(TWeak& otr) noexcept
	: m_ptr(otr.m_ptr) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak(TWeak<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(std::move(otr.m_ptr)) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak& operator=(const TWeak<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak& operator=(TWeak<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	TWeak& operator=(const TWeak& otr) noexcept {
		this->m_ptr = otr.m_ptr;
		return *this;
	}

	TWeak& operator=(TWeak& otr) noexcept {
		this->m_ptr = otr.m_ptr;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TWeak& operator=(TWeak<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
noexcept {
#endif
		this->m_ptr = std::move(otr.m_ptr);
		return *this;
	}

	// Resets weak pointer to an empty state, since this pointer has no ownership it will no destroy any objects
	void destroy() noexcept {
		m_ptr.reset();
	}

	template <typename TOtherType>
	TShared<TOtherType> staticCast() const noexcept {
		if (auto ptr = m_ptr.lock()) {
			return TShared<TOtherType>{std::static_pointer_cast<TOtherType, TType>(ptr)};
		}
		return nullptr;
	}

	template <typename TOtherType>
	TShared<TOtherType> dynamicCast() const noexcept {
		if (auto ptr = m_ptr.lock()) {
			return TShared<TOtherType>{std::dynamic_pointer_cast<TOtherType, TType>(ptr)};
		}
		return nullptr;
	}

	template <typename TOtherType>
	TShared<TOtherType> reinterpretCast() const noexcept {
		if (auto ptr = m_ptr.lock()) {
			return TShared<TOtherType>{std::reinterpret_pointer_cast<TOtherType, TType>(ptr)};
		}
		return nullptr;
	}

	template <typename TOtherType>
	TShared<TOtherType> constCast() const noexcept {
		if (auto ptr = m_ptr.lock()) {
			return TShared<TOtherType>{std::const_pointer_cast<TOtherType, TType>(ptr)};
		}
		return nullptr;
	}

	// Workaround for not being able to reference TWeak from TShared
	operator std::shared_ptr<TType>() const noexcept {
		return m_ptr.lock();
	}

	operator TShared<TType>() const noexcept {
		return TShared<TType>{m_ptr.lock()};
	}

	TShared<TType> operator->() const noexcept {
		return TShared<TType>{m_ptr.lock()};
	}

	TShared<TType> operator*() const noexcept {
		return TShared<TType>{m_ptr.lock()};
	}

	TShared<TType> get() const noexcept { return TShared<TType>{m_ptr.lock()}; }

	operator bool() const noexcept {
		return !m_ptr.expired();
	}

	friend bool operator<(const TWeak& fst, const TWeak& snd) noexcept {
		return fst.m_ptr.owner_before(snd.m_ptr);
	}

	friend bool operator<=(const TWeak& fst, const TWeak& snd) noexcept {
		return !snd.m_ptr.owner_before(fst.m_ptr);
	}

	friend bool operator>(const TWeak& fst, const TWeak& snd) noexcept {
		return snd.m_ptr.owner_before(fst.m_ptr);
	}

	friend bool operator>=(const TWeak& fst, const TWeak& snd) noexcept {
		return !fst.m_ptr.owner_before(snd.m_ptr);
	}

	// TODO: c++26 owner_equal c++26 owner_hash
	friend bool operator==(const TWeak& fst, const TWeak& snd) noexcept {
		return !fst.m_ptr.owner_before(snd.m_ptr) &&
			!snd.m_ptr.owner_before(fst.m_ptr);
	}

	// Compare raw pointer
	friend bool operator==(const TWeak& fst, const void* snd) noexcept {
		auto ptr = fst.m_ptr.lock();
		return ptr && ptr.get() == snd;
	}

	// Compare raw pointer
	friend bool operator==(const void* fst, const TWeak& snd) noexcept {
		return snd == fst;
	}

	friend bool operator!=(const TWeak& fst, const TWeak& snd) noexcept {
		return fst.m_ptr.owner_before(snd.m_ptr) ||
			snd.m_ptr.owner_before(fst.m_ptr);
	}

	// Compare raw pointer
	friend bool operator!=(const TWeak& fst, const void* snd) noexcept {
		auto ptr = fst.m_ptr.lock();
		return !ptr || ptr.get() != snd;
	}

	// Compare raw pointer
	friend bool operator!=(const void* fst, const TWeak& snd) noexcept {
		return snd != fst;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator<(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() < snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator<=(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() <= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator>(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() > snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator>=(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() >= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator==(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() == snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator!=(const TWeak& fst, const TOtherType& snd) noexcept {
		return fst.get() != snd.get();
	}

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TWeak& inValue) {
		if (auto ptr = inValue.m_ptr.lock()) {
			inArchive >> *inValue.get();
		}
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TWeak& inValue) {
		if (auto ptr = inValue.m_ptr.lock()) {
			inArchive << *inValue.get();
		}
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

	std::weak_ptr<TType> m_ptr;
};

template <typename TType>
struct TUnfurled<TWeak<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TWeak<TType>::get;
};

#ifndef USING_SIMPLEARCHIVE
template<typename TType>
struct std::hash<TWeak<TType>> {
	size_t operator()(const TWeak<TType>& obj) const noexcept {
		if (auto ptr = obj.m_ptr.lock()) {
			std::hash<std::shared_ptr<TType>> ptrHash;
			return ptrHash(ptr);
		}
		return 0;
	}
};
#endif

template <typename TType>
template <typename TOtherType>
_CONSTEXPR23 TShared<TType>::TShared(const TWeak<TOtherType>& shared) noexcept
	: m_ptr(shared.m_ptr) {}

template <typename TType>
template <typename TOtherType>
_CONSTEXPR23 TShared<TType>::TShared(TWeak<TOtherType>& shared) noexcept
: m_ptr(shared.m_ptr) {}