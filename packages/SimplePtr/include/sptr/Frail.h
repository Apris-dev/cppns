#pragma once

#include "sptr/Common.h"
#include "sutil/InitializerList.h"
#include "sptr/Weak.h"
#include "sptr/Unique.h"

template <typename TType>
struct TFrail {

	TFrail(const TWeak<TType>& ptr) noexcept
	: m_ptr(ptr.m_ptr.lock().get()) {}

	TFrail(TWeak<TType>& ptr) noexcept
	: m_ptr(ptr.m_ptr.lock().get()) {}

	TFrail(const std::weak_ptr<TType>& ptr) noexcept
	: m_ptr(ptr.lock().get()) {}

	TFrail(std::weak_ptr<TType>& ptr) noexcept
	: m_ptr(ptr.lock().get()) {}

	template <typename TOtherType>
	TFrail(const TShared<TOtherType>& shared) noexcept
	: m_ptr(shared.m_ptr.get()) {}

	template <typename TOtherType>
	TFrail(TShared<TOtherType>& shared) noexcept
	: m_ptr(shared.m_ptr.get()) {}

	template <typename TOtherType>
	TFrail(const std::shared_ptr<TOtherType>& shared) noexcept
	: m_ptr(shared.get()) {}

	template <typename TOtherType>
	TFrail(std::shared_ptr<TOtherType>& shared) noexcept
	: m_ptr(shared.get()) {}

	template <typename TOtherType>
	TFrail(const TUnique<TOtherType>& unique) noexcept
	: m_ptr(unique.m_ptr.get()) {}

	template <typename TOtherType>
	TFrail(TUnique<TOtherType>& unique) noexcept
	: m_ptr(unique.m_ptr.get()) {}

	template <typename TOtherType>
	TFrail(const std::unique_ptr<TOtherType>& unique) noexcept
	: m_ptr(unique.get()) {}

	template <typename TOtherType>
	TFrail(std::unique_ptr<TOtherType>& unique) noexcept
	: m_ptr(unique.get()) {}

	TFrail() = default;

	TFrail(TType* ptr): m_ptr(ptr) {}

	TFrail(std::nullptr_t) noexcept {}

	TFrail& operator=(std::nullptr_t) noexcept {
		m_ptr = nullptr;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail(const TFrail<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail(TFrail<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	TFrail(const TFrail& otr) noexcept
	: m_ptr(otr.m_ptr) {}

	TFrail(TFrail& otr) noexcept
	: m_ptr(otr.m_ptr) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail(TFrail<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(std::move(otr.m_ptr)) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail& operator=(const TFrail<TOtherType>& otr) = delete;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail& operator=(TFrail<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	TFrail& operator=(const TFrail& otr) noexcept {
		this->m_ptr = otr.m_ptr;
		return *this;
	}

	TFrail& operator=(TFrail& otr) noexcept {
		this->m_ptr = otr.m_ptr;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TFrail& operator=(TFrail<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
noexcept {
#endif
		this->m_ptr = std::move(otr.m_ptr);
		return *this;
	}

	template <typename TOtherType>
	TFrail<TOtherType> staticCast() const noexcept {
		return TFrail<TOtherType>{static_cast<TOtherType*>(m_ptr)};
	}

	template <typename TOtherType>
	TFrail<TOtherType> dynamicCast() const noexcept {
		return TFrail<TOtherType>{dynamic_cast<TOtherType*>(m_ptr)};
	}

	template <typename TOtherType>
	TFrail<TOtherType> reinterpretCast() const noexcept {
		return TFrail<TOtherType>{reinterpret_cast<TOtherType*>(m_ptr)};
	}

	template <typename TOtherType>
	TFrail<TOtherType> constCast() const noexcept {
		return TFrail<TOtherType>{const_cast<TOtherType*>(m_ptr)};
	}

	TType* operator->() const noexcept {
		return m_ptr;
	}

	TType& operator*() const noexcept {
		return *m_ptr;
	}

	TType* get() const noexcept { return m_ptr; }

	operator bool() const noexcept {
		return m_ptr != nullptr;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator<(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() < snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator<=(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() <= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator>(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() > snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator>=(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() >= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator==(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() == snd.get();
	}

	// Compare raw pointer
	friend bool operator==(const TFrail& fst, const void* snd) noexcept {
		return fst.get() == snd;
	}

	// Compare raw pointer
	friend bool operator==(const void* fst, const TFrail& snd) noexcept {
		return snd == fst;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	friend bool operator!=(const TFrail& fst, const TOtherType& snd) noexcept {
		return fst.get() != snd.get();
	}

	// Compare raw pointer
	friend bool operator!=(const TFrail& fst, const void* snd) noexcept {
		return fst.get() != snd;
	}

	// Compare raw pointer
	friend bool operator!=(const void* fst, const TFrail& snd) noexcept {
		return snd != fst;
	}

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TFrail& inValue) {
		inArchive >> inValue.m_ptr;
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TFrail& inValue) {
		inArchive << inValue.m_ptr;
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

	TType* m_ptr = nullptr;
};

template <typename TType>
struct TUnfurled<TFrail<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TFrail<TType>::get;
};

template <typename TType>
TFrail(TUnique<TType>) -> TFrail<typename sstl::EnforceConvertible<TType>::Type>;

template <typename TType>
TFrail(TShared<TType>) -> TFrail<typename sstl::EnforceConvertible<TType>::Type>;

template <typename TType>
TFrail(TWeak<TType>) -> TFrail<typename sstl::EnforceConvertible<TType>::Type>;

#ifndef USING_SIMPLEARCHIVE
template<typename TType>
struct std::hash<TFrail<TType>> {
	size_t operator()(const TFrail<TType>& obj) const noexcept {
		std::hash<TType*> ptrHash;
		return ptrHash(obj.m_ptr);
	}
};
#endif