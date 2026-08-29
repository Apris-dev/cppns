#pragma once

#include <type_traits>

/*
 * Contains various comparison type traits to know if a type is comparable
 */

namespace sutil {
#if defined(__cpp_concepts) && __cpp_concepts >= 201907L

    /*
    * Equality
    */

    template <typename TType, typename TOtherType = TType>
    concept is_equality_comparable_concept = requires(TType a, TOtherType b) { a == b; };

    template <typename TType, typename TOtherType = TType>
    struct is_equality_comparable : std::bool_constant<is_equality_comparable_concept<TType, TOtherType>> {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_equality_comparable_v = is_equality_comparable<TType, TOtherType>::value;

    /*
    * Less than
    */

    template <typename TType, typename TOtherType = TType>
    concept is_less_than_comparable_concept = requires(TType a, TOtherType b) { a < b; };

    template <typename TType, typename TOtherType = TType>
    struct is_less_than_comparable : std::bool_constant<is_less_than_comparable_concept<TType, TOtherType>> {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_less_than_comparable_v = is_less_than_comparable<TType, TOtherType>::value;

    /*
    * Less than or equal to
    */

    template <typename TType, typename TOtherType = TType>
    concept is_less_than_or_equal_to_comparable_concept = requires(TType a, TOtherType b) { a <= b; };

    template <typename TType, typename TOtherType = TType>
    struct is_less_than_or_equal_to_comparable : std::bool_constant<is_less_than_or_equal_to_comparable_concept<TType, TOtherType>> {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_less_than_or_equal_to_comparable_v = is_less_than_or_equal_to_comparable<TType, TOtherType>::value;

    /*
    * Greater than
    */

    template <typename TType, typename TOtherType = TType>
    concept is_greater_than_comparable_concept = requires(TType a, TOtherType b) { a > b; };

    template <typename TType, typename TOtherType = TType>
    struct is_greater_than_comparable : std::bool_constant<is_greater_than_comparable_concept<TType, TOtherType>> {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_greater_than_comparable_v = is_greater_than_comparable<TType, TOtherType>::value;

    /*
    * Greater than or equal to
    */

    template <typename TType, typename TOtherType = TType>
    concept is_greater_than_or_equal_to_comparable_concept = requires(TType a, TOtherType b) { a >= b; };

    template <typename TType, typename TOtherType = TType>
    struct is_greater_than_or_equal_to_comparable : std::bool_constant<is_greater_than_or_equal_to_comparable_concept<TType, TOtherType>> {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_greater_than_or_equal_to_comparable_v = is_greater_than_or_equal_to_comparable<TType, TOtherType>::value;
#else

    /*
    * Equality
    */

    template <typename TType, typename TOtherType = TType, typename = void>
    struct is_equality_comparable : std::false_type {};

    template <typename TType, typename TOtherType>
    struct is_equality_comparable<TType, TOtherType,
        std::void_t<decltype(std::declval<TType>() == std::declval<TOtherType>())>
    > : std::true_type {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_equality_comparable_v = is_equality_comparable<TType, TOtherType>::value;

    /*
    * Less than
    */

    template <typename TType, typename TOtherType = TType, typename = void>
    struct is_less_than_comparable : std::false_type {};

    template <typename TType, typename TOtherType>
    struct is_less_than_comparable
    <TType, TOtherType,
        std::void_t<decltype(std::declval<TType>() < std::declval<TOtherType>())>
    > : std::true_type {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_less_than_comparable_v = is_less_than_comparable<TType, TOtherType>::value;

    /*
    * Less than or equal to
    */

    template <typename TType, typename TOtherType = TType, typename = void>
    struct is_less_than_or_equal_to_comparable : std::false_type {};

    template <typename TType, typename TOtherType>
    struct is_less_than_or_equal_to_comparable
    <TType, TOtherType,
        std::void_t<decltype(std::declval<TType>() <= std::declval<TOtherType>())>
    > : std::true_type {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_less_than_or_equal_to_comparable_v = is_less_than_or_equal_to_comparable<TType, TOtherType>::value;

    /*
     * Greater than
     */

    template <typename TType, typename TOtherType = TType, typename = void>
    struct is_greater_than_comparable : std::false_type {};

    template <typename TType, typename TOtherType>
    struct is_greater_than_comparable
    <TType, TOtherType,
        std::void_t<decltype(std::declval<TType>() > std::declval<TOtherType>())>
    > : std::true_type {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_greater_than_comparable_v = is_greater_than_comparable<TType, TOtherType>::value;

    /*
    * Greater than or equal to
    */

    template <typename TType, typename TOtherType = TType, typename = void>
    struct is_greater_than_or_equal_to_comparable : std::false_type {};

    template <typename TType, typename TOtherType>
    struct is_greater_than_or_equal_to_comparable
    <TType, TOtherType,
        std::void_t<decltype(std::declval<TType>() >= std::declval<TOtherType>())>
    > : std::true_type {};

    template <typename TType, typename TOtherType = TType>
    constexpr bool is_greater_than_or_equal_to_comparable_v = is_greater_than_or_equal_to_comparable<TType, TOtherType>::value;
#endif

    /*template <typename TType, typename TOtherType = TType>
    constexpr bool is_fully_comparable_v =
        is_eq<TType, TOtherType>::value;*/
}

// TODO: more hashing types, can use std::hash for the more 'complicated' ones