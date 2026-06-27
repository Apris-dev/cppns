#pragma once

#include <type_traits>

#include "sutil/PlatformDefinition.h"

#ifdef USING_SIMPLEARCHIVE
#include "sarch/Archive.h"
#endif

namespace sstl {
#ifndef SIMPLEPTR_INIT
	template <typename>
	constexpr bool is_initializable_v = false;

	template <typename TType>
	struct is_initializable : std::bool_constant<is_initializable_v<TType>> {};
#else
	#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
		template <typename TType>
		concept is_initializable_v =
		requires(TType& obj) {
			obj.init();
		};

		template <typename TType>
		struct is_initializable : std::bool_constant<is_initializable_v<TType>> {};
	#else
		template <typename, typename>
		struct is_initializable : std::false_type {};

		template <typename TType>
		struct is_initializable<
			std::void_t<decltype(std::declval<TType&>().init())>,
			TType
		> : std::true_type {};

	template <typename TType>
	constexpr bool is_initializable_v = is_initializable<void, TType>::value;
	#endif
#endif

#ifndef SIMPLEPTR_DESTROY
	template <typename>
	constexpr bool is_destroyable_v = false;

	template <typename TType>
	struct is_destroyable : std::bool_constant<is_destroyable_v<TType>> {};
#else
	#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
		template <typename TType>
		concept is_destroyable_v =
		requires(TType& obj) {
			obj.destroy();
		};

		template <typename TType>
		struct is_destroyable : std::bool_constant<is_destroyable_v<TType>> {};
	#else
		template <typename, typename = void>
		struct is_destroyable : std::false_type {};

		template <typename TType>
		struct is_destroyable<
			std::void_t<decltype(std::declval<TType&>().destroy())>,
			TType
		> : std::true_type {};

		template <typename TType>
		constexpr bool is_destroyable_v = is_destroyable<TType>::value;
	#endif
#endif

#ifndef SIMPLEPTR_RELEASE
	template <typename>
	constexpr bool is_releasable_v = false;

	template <typename TType>
	struct is_releasable : std::bool_constant<is_releasable_v<TType>> {};
#else
	#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
		template <typename TType>
		concept is_releasable =
		requires(TType& obj) {
			obj.release();
		};

		template <typename TType>
		constexpr bool is_releasable_v = is_releasable<TType>;

	#else
		template <typename, typename = void>
		struct is_releasable : std::false_type {};

		template <typename TType>
		struct is_releasable<TType, std::void_t<decltype(std::declval<TType&>().release())>>
			: std::true_type {};

		template <typename TType>
		constexpr bool is_releasable_v = is_releasable<TType>::value;
	#endif
#endif

	template <typename TType>
	struct deleter {
		constexpr deleter() noexcept = default;

		template <typename TOtherType, std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0>
		deleter(const deleter<TOtherType>&) noexcept {}

		void operator()(TType* ptr) const noexcept {
			static_assert(0 < sizeof(TType), "Can't delete an incomplete type!");
			if constexpr (sstl::is_releasable_v<TType>) {
				ptr->release();
			} else {
				if constexpr (sstl::is_destroyable_v<TType>) {
					ptr->destroy();
				}
				delete ptr;
			}
		}
	};

	template <typename TType>
	void delete_impl(void* p) noexcept {
		static_assert(0 < sizeof(TType), "Can't delete an incomplete type!");
		auto* ptr = static_cast<TType*>(p);
		if constexpr (sstl::is_releasable_v<TType>) {
			ptr->release();
		} else {
			if constexpr (sstl::is_destroyable_v<TType>) {
				ptr->destroy();
			}
			delete ptr;
		}
	}

	template <typename TType>
	struct delayed_deleter {
		typedef void (*Func)(void* ptr);
		Func fn = nullptr;

		constexpr delayed_deleter() noexcept = default;
		constexpr delayed_deleter(const Func fn) noexcept: fn(fn) {}

		template <typename TOtherType, std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0>
		delayed_deleter(const delayed_deleter<TOtherType>& otr) noexcept: fn(otr.fn) {}

		void operator()(void* ptr) const noexcept {
			if (fn) fn(ptr);
		}
	};

	template <typename TType>
	struct SharedDeleter {
		using Type = TType;

		void operator()(TType* ptr) const noexcept {
			static_assert(0 < sizeof(TType), "Can't delete an incomplete type!");
			if constexpr (sstl::is_destroyable_v<TType>) {
				ptr->destroy();
			}
			delete ptr;
		}
	};
}

template <typename TType>
struct TUnfurled {
	using Type = TType;
	constexpr static bool isManaged = false;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType, TType>, int> = 0
	>
	constexpr static TType create(TArgs&&... args)
#ifdef __cpp_lib_is_nothrow_convertible
noexcept(std::is_nothrow_convertible_v<TOtherType, TType>) {
#else
	noexcept {
#endif
		TOtherType obj{std::forward<TArgs>(args)...};
		if constexpr (sstl::is_initializable_v<TOtherType>) {
			obj.init();
		}
		return obj;
	}
};

namespace sstl {
	template <typename TType>
	constexpr bool is_managed_v = TUnfurled<TType>::isManaged;

	template <typename TType>
	struct is_managed : std::bool_constant<is_managed_v<TType>> {};

	template <typename TType>
	_CONSTEXPR23 TType* getUnfurled(TType* type) {
		return type;
	}

	template <typename TType>
	_CONSTEXPR23 const TType* getUnfurled(const TType* type) {
		return type;
	}

	template <typename TType>
	_CONSTEXPR23 typename TUnfurled<TType>::Type* getUnfurled(TType& type) {
		if constexpr (sstl::is_managed_v<TType>) {
			return type.get();
		} else {
			return &type;
		}
	}

	template <typename TType>
	_CONSTEXPR23 const typename TUnfurled<TType>::Type* getUnfurled(const TType& type) {
		if constexpr (sstl::is_managed_v<TType>) {
			return type.get();
		} else {
			return &type;
		}
	}
}