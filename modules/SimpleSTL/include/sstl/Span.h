#pragma once

#include "ForwardList.h"

// Generic span, unlike std::span this type supports all container types as input
// Including maps and sets (Maps will be pairs)
// Can only be const since the other types are limited
// (TType is enforced, while TContainerType is interpreted from construction)
template <typename TType, typename TOptValueType = void>
struct TSpan {

    using TTrueType = std::conditional_t<std::is_same_v<TOptValueType, void>, TType, TPair<TType, TOptValueType>>;

    TSpan() = delete;

    template <typename TOtherContainerType,
        std::enable_if_t<std::is_same_v<TTrueType, typename TContainerTraits<TOtherContainerType>::Type>, int> = 0
    >
    TSpan(const TSequenceContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    template <typename TOtherContainerType,
        std::enable_if_t<
            std::conjunction_v<
                std::is_same<TType, typename TContainerTraits<TOtherContainerType>::KeyType>,
                std::is_same<TOptValueType, typename TContainerTraits<TOtherContainerType>::ValueType>
            >
        , int> = 0
    >
    TSpan(const TAssociativeContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    template <typename TOtherContainerType,
        std::enable_if_t<std::is_same_v<TTrueType, typename TContainerTraits<TOtherContainerType>::Type>, int> = 0
    >
    TSpan(const TSelfAssociativeContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    [[nodiscard]] size_t getSize() const { return m_Container.getSize(); }

    [[nodiscard]] bool isEmpty() const { return m_Container.isEmpty(); }

    [[nodiscard]] const TType& top() const { return m_Container.top(); }

    [[nodiscard]] decltype(auto) begin() const { return m_Container.begin(); }

    [[nodiscard]] decltype(auto) end() const { return m_Container.end(); }

    [[nodiscard]] bool isValid(size_t index) const { return m_Container.isValid(index); }

    template <typename TOtherType>
    [[nodiscard]] bool contains(const TOtherType& obj) const { return m_Container.contains(obj); }

    template <typename TOtherType>
    [[nodiscard]] size_t find(const TOtherType& obj) const { return m_Container.find(obj); }

private:

    TForwardList<std::reference_wrapper<const TTrueType>> m_Container;
};

template <typename TContainerType>
TSpan(const TSequenceContainer<TContainerType>&) -> TSpan<typename TContainerTraits<TContainerType>::Type>;

template <typename TContainerType>
TSpan(const TAssociativeContainer<TContainerType>&) -> TSpan<typename TContainerTraits<TContainerType>::KeyType, typename TContainerTraits<TContainerType>::ValueType>;

template <typename TContainerType>
TSpan(const TSelfAssociativeContainer<TContainerType>&) -> TSpan<typename TContainerTraits<TContainerType>::Type>;