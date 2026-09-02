#pragma once

#include <memory>
#include "Common.h"

template <typename>
struct TWeak;

template <typename TType>
struct TShared {

	constexpr_23 TShared(const std::shared_ptr<TType>& ptr) noexcept
	: m_ptr(ptr) {}

	constexpr_23 TShared(std::shared_ptr<TType>&& ptr) noexcept
	: m_ptr(std::move(ptr)) {}

	template <typename TOtherType>
	constexpr_23 TShared(const TWeak<TOtherType>& shared) noexcept;

	template <typename TOtherType>
	constexpr_23 TShared(const std::weak_ptr<TOtherType>& shared) noexcept
	: m_ptr(shared) {}

	constexpr_23 TShared() noexcept {
		// If not default constructible, default to nullptr
		if constexpr (std::is_default_constructible_v<TType>) {
			m_ptr = std::shared_ptr<TType>(new TType(), sstl::deleter<TType>());
			if constexpr (sstl::is_initializable_v<TType>) {
				m_ptr->init();
			}
		}
	}

	constexpr_23 TShared(std::nullptr_t) noexcept {}

	constexpr_23 TShared& operator=(std::nullptr_t) noexcept {
		m_ptr = nullptr;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	constexpr_23 explicit TShared(TOtherType* ptr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(ptr, sstl::deleter<TType>()) {}

	// prefer init because SharedFrom works there
	template <typename... TArgs,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_null_pointer<std::decay_t<TArgs>>>...,
				std::negation<sstl::is_managed<TArgs>>...
			>,
			int> = 0
	>
	constexpr_23 explicit TShared(TArgs&&... args) noexcept {
		m_ptr = std::shared_ptr<TType>(new TType(std::forward<TArgs>(args)...), sstl::deleter<TType>());
		if constexpr (sstl::is_initializable_v<TType>) {
			m_ptr->init();
		}
	}

	template <typename TChildType,
		std::enable_if_t<std::is_base_of_v<TType, TChildType>, int> = 0
	>
	constexpr_23 explicit TShared(TChildType&& obj) noexcept {
		m_ptr = std::shared_ptr<TType>(new TChildType(std::forward<TChildType>(obj)), sstl::deleter<TType>());
		if constexpr (sstl::is_initializable_v<TType>) {
			m_ptr->init();
		}
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	constexpr_23 TShared(const TShared<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	constexpr_23 TShared(const TShared& otr) noexcept
	: m_ptr(otr.m_ptr) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	constexpr_23 TShared(TShared<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: m_ptr(std::move(otr.m_ptr)) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TShared& operator=(const TShared<TOtherType>& otr) = delete;

	/*
	 * Allow copies of same type
	 */

	TShared& operator=(const TShared& otr) noexcept = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	constexpr_23 TShared& operator=(TShared<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		this->m_ptr = std::move(otr.m_ptr);
		return *this;
	}

	[[nodiscard]] size_t count() const noexcept {
		return m_ptr.use_count();
	}

	// Releases ownership of the pointer, note the object will not be destroyed unless all other shared pointers are
	void destroy() noexcept {
		m_ptr.reset();
	}

	template <typename TOtherType>
	constexpr_23 TShared<TOtherType> staticCast() const noexcept {
		return TShared<TOtherType>{std::static_pointer_cast<TOtherType, TType>(this->m_ptr)};
	}

	template <typename TOtherType>
	constexpr_23 TShared<TOtherType> dynamicCast() const noexcept {
		return TShared<TOtherType>{std::dynamic_pointer_cast<TOtherType, TType>(this->m_ptr)};
	}

	template <typename TOtherType>
	constexpr_23 TShared<TOtherType> reinterpretCast() const noexcept {
		return TShared<TOtherType>{std::reinterpret_pointer_cast<TOtherType, TType>(this->m_ptr)};
	}

	template <typename TOtherType>
	constexpr_23 TShared<TOtherType> constCast() const noexcept {
		return TShared<TOtherType>{std::const_pointer_cast<TOtherType, TType>(this->m_ptr)};
	}

	constexpr_23 TType* operator->() const noexcept {
		return m_ptr.get();
	}

	constexpr_23 TType& operator*() const noexcept {
		return *m_ptr.get();
	}

	constexpr_23 TType* get() const noexcept { return m_ptr.get(); }

	constexpr_23 operator bool() const noexcept {
		return static_cast<bool>(m_ptr);
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator<(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() < snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator<(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst < otr;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator<=(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() <= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator<=(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst <= otr;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator>(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() > snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator>(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst > otr;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator>=(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() >= snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator>=(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst >= otr;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator==(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() == snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator==(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst == otr;
	}

	// Compare raw pointer
	constexpr_23 friend bool operator==(const TShared& fst, const void* snd) noexcept {
		return fst.get() == snd;
	}

	// Compare raw pointer
	constexpr_23 friend bool operator==(const void* fst, const TShared& snd) noexcept {
		return snd == fst;
	}

	template <typename TOtherType,
		std::enable_if_t<sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator!=(const TShared& fst, const TOtherType& snd) noexcept {
		return fst.get() != snd.get();
	}

	template <typename TOtherType,
		std::enable_if_t<!sstl::is_managed<TOtherType>::value, int> = 0
	>
	constexpr_23 friend bool operator!=(const TShared& fst, const TOtherType& otr) noexcept {
		return *fst != otr;
	}

	// Compare raw pointer
	constexpr_23 friend bool operator!=(const TShared& fst, const void* snd) noexcept {
		return fst.get() != snd;
	}

	// Compare raw pointer
	constexpr_23 friend bool operator!=(const void* fst, const TShared& snd) noexcept {
		return snd != fst;
	}

#ifdef USING_CPPNS_ARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TShared& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TShared& inValue) {
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

	std::shared_ptr<TType> m_ptr = nullptr;

};

template <typename TType>
struct TUnfurled<TShared<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TShared<TType>::get;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	constexpr_23 static TShared<TType> create(TArgs&&... args)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		return TShared<TOtherType>(std::forward<TArgs>(args)...);
	}
};

#ifndef USING_CPPNS_ARCHIVE
template<typename TType>
struct std::hash<TShared<TType>> {
	size_t operator()(const TShared<TType>& obj) const noexcept {
		std::hash<std::shared_ptr<TType>> ptrHash;
		return ptrHash(obj.m_ptr);
	}
};
#endif

// Template argument deduction for input of a single type
template <typename TType>
TShared(TType) -> TShared<TType>;