#pragma once

#include "foundation/type_traits.hpp"

namespace Engine
{
namespace Private
{
    enum EForwardConstructor
    {
        ForwardConstructor
    };

    enum ETupleConstructor
    {
        TupleConstructor
    };

    template <typename T, uint32 Index, uint32 TupleSize>
    struct TupleElement
    {
        using Type = T;

        TupleElement() : Value() {}

        template <typename ArgType, typename std::enable_if_t<std::is_constructible_v<T, ArgType&&>, bool> = false>
        explicit TupleElement(EForwardConstructor, ArgType&& val) : Value(std::forward<ArgType>(val)) {}

        TupleElement(TupleElement&&) = default;
        TupleElement(const TupleElement&) = default;
        TupleElement& operator=(TupleElement&&) noexcept = default;
        TupleElement& operator=(const TupleElement&) = default;

        Type Value;
    };

    template <typename T>
    struct TupleElement<T, 0, 2>
    {
    public:
        using Type = T;

        TupleElement() : Key() {}

        template <typename ArgType, typename std::enable_if_t<std::is_constructible_v<T, ArgType&&>, bool> = false>
        explicit TupleElement(EForwardConstructor, ArgType&& val) : Key(std::forward<ArgType>(val)) {}

        TupleElement(TupleElement&&) = default;
        TupleElement(const TupleElement&) = default;
        TupleElement& operator=(TupleElement&&) noexcept = default;
        TupleElement& operator=(const TupleElement&) = default;

        Type Key;
    };

    template <typename... ArgTypes>
    struct TypeNumInPack;

    template <typename T>
    struct TypeNumInPack<T>
    {
        constexpr static uint32 Value = 0;
    };

    template <typename T, typename U, typename... Reset>
    struct TypeNumInPack<T, U, Reset...>
    {
        constexpr static uint32 Value = TypeNumInPack<T, Reset...>::Value + (std::is_same_v<T, U> ? 1 : 0);
    };

    template <uint32 Index, uint32 TupleSize>
    struct TupleElementIndexGetter
    {
        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, Index, TupleSize>&, TupleType& tuple)
        {
            using RetType = TupleElement<ElementType, Index, TupleSize>;
            return (static_cast<RetType&>(tuple).Value);
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, Index, TupleSize>&, const TupleType& tuple)
        {
            using RetType = TupleElement<ElementType, Index, TupleSize>;
            return (static_cast<const RetType&>(tuple).Value);
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, Index, TupleSize>&, TupleType&& tuple)
        {
            using RetType = TupleElement<ElementType, Index, TupleSize>;
            return (std::move(static_cast<RetType&&>(tuple).Value));
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, Index, TupleSize>&, const TupleType&& tuple)
        {
            using RetType = TupleElement<ElementType, Index, TupleSize>;
            return (std::move(static_cast<const RetType&&>(tuple).Value));
        }


        template <typename TupleType>
        static constexpr decltype(auto) Get(TupleType&& tuple)
        {
            return GetImpl(tuple, std::forward<TupleType>(tuple));
        }
    };

    template <>
    struct TupleElementIndexGetter<0, 2>
    {
        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, 0, 2>&, TupleType& tuple)
        {
            using RetType = TupleElement<ElementType, 0, 2>;
            return (static_cast<RetType&>(tuple).Key);
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, 0, 2>&, const TupleType& tuple)
        {
            using RetType = TupleElement<ElementType, 0, 2>;
            return (static_cast<const RetType&>(tuple).Key);
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, 0, 2>&, TupleType&& tuple)
        {
            using RetType = TupleElement<ElementType, 0, 2>;
            return (std::move(static_cast<RetType&&>(tuple).Key));
        }

        template <typename ElementType, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<ElementType, 0, 2>&, const TupleType&& tuple)
        {
            using RetType = TupleElement<ElementType, 0, 2>;
            return (std::move(static_cast<const RetType&&>(tuple).Key));
        }

        template <typename TupleType>
        static constexpr decltype(auto) Get(TupleType&& tuple)
        {
            return GetImpl(tuple, std::forward<TupleType>(tuple));
        }
    };

    template <typename Type, uint32 TupleSize>
    struct TupleElementTypeGetter
    {
        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, TupleSize>&, TupleType& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, TupleSize>;
            return (static_cast<RetType&>(tuple).Value);
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, TupleSize>&, const TupleType& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, TupleSize>;
            return (static_cast<const RetType&>(tuple).Value);
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, TupleSize>&, TupleType&& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, TupleSize>;
            return (std::move(static_cast<RetType&&>(tuple).Value));
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, TupleSize>&, const TupleType&& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, TupleSize>;
            return (std::move(static_cast<const RetType&&>(tuple).Value));
        }

        template <typename TupleType>
        static constexpr decltype(auto) Get(TupleType&& tuple)
        {
            return GetImpl(tuple, std::forward<TupleType>(tuple));
        }
    };

    template <typename Type>
    struct TupleElementTypeGetter<Type, 2>
    {
        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, 2>&, TupleType& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, 2>;
            if constexpr (std::is_same_v<decltype(tuple.Key), Type>)
            {
                return (static_cast<RetType&>(tuple).Key);
            }
            else
            {
                return (static_cast<RetType&>(tuple).Value);
            }
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, 2>&, const TupleType& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, 2>;
            if constexpr (std::is_same_v<decltype(tuple.Key), Type>)
            {
                return (static_cast<const RetType&>(tuple).Key);
            }
            else
            {
                return (static_cast<const RetType&>(tuple).Value);
            }
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, 2>&, TupleType&& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, 2>;
            if constexpr (std::is_same_v<decltype(tuple.Key), Type>)
            {
                return (std::move(static_cast<RetType&&>(tuple).Key));
            }
            else
            {
                return (std::move(static_cast<RetType&&>(tuple).Value));
            }
        }

        template <uint32 ElemIndex, typename TupleType>
        static constexpr decltype(auto) GetImpl(const TupleElement<Type, ElemIndex, 2>&, const TupleType&& tuple)
        {
            using RetType = TupleElement<Type, ElemIndex, 2>;
            if constexpr (std::is_same_v<decltype(tuple.Key), Type>)
            {
                return (std::move(static_cast<const RetType&&>(tuple).Key));
            }
            else
            {
                return (std::move(static_cast<const RetType&&>(tuple).Value));
            }
        }

        template <typename TupleType>
        static constexpr decltype(auto) Get(TupleType&& tuple)
        {
            return GetImpl(tuple, std::forward<TupleType>(tuple));
        }
    };

    template <typename Indices, typename... Types>
    struct TupleImpl;

    template <uint32... Indices, typename... Types>
    struct TupleImpl<std::integer_sequence<uint32, Indices...>, Types...> : public TupleElement<Types, Indices, sizeof...(Types)>...
    {
        TupleImpl() = default;
        TupleImpl(const TupleImpl&) = default;
        TupleImpl(TupleImpl&&) noexcept = default;
        TupleImpl& operator=(const TupleImpl&) = default;
        TupleImpl& operator=(TupleImpl&&) = default;

        /** Avoid duplicated with default constructor*/
        template <typename... ArgTypes>
        explicit TupleImpl(EForwardConstructor, ArgTypes&&... args) : TupleElement<Types, Indices, sizeof...(Types)>(ForwardConstructor, std::forward<ArgTypes>(args))...
        {}

        /** Avoid duplicated with default constructor*/
        template <typename OtherTupleType>
        TupleImpl(ETupleConstructor, OtherTupleType&& tuple) : TupleElement<Types, Indices, sizeof...(Types)>(ForwardConstructor, std::forward<OtherTupleType>(tuple).template Get<Indices>())...
        {}

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        constexpr decltype(auto) Get()                & { return TupleElementIndexGetter<Index, sizeof...(Types)>::Get(static_cast<TupleImpl&>(*this)); }

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        constexpr decltype(auto) Get() const          & { return TupleElementIndexGetter<Index, sizeof...(Types)>::Get(static_cast<const TupleImpl&>(*this)); }

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        constexpr decltype(auto) Get()                && { return TupleElementIndexGetter<Index, sizeof...(Types)>::Get(static_cast<TupleImpl&&>(*this)); }

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        constexpr decltype(auto) Get() const          && { return TupleElementIndexGetter<Index, sizeof...(Types)>::Get(static_cast<const TupleImpl&&>(*this)); }

        template <typename Type, std::enable_if_t<TypeNumInPack<Type, Types...>::Value == 1, bool> = false>
        constexpr decltype(auto) Get()                & { return TupleElementTypeGetter<Type, sizeof...(Types)>::Get(static_cast<TupleImpl&>(*this)); }

        template <typename Type, std::enable_if_t<TypeNumInPack<Type, Types...>::Value == 1, bool> = false>
        constexpr decltype(auto) Get() const          & { return TupleElementTypeGetter<Type, sizeof...(Types)>::Get(static_cast<const TupleImpl&>(*this)); }

        template <typename Type, std::enable_if_t<TypeNumInPack<Type, Types...>::Value == 1, bool> = false>
        constexpr decltype(auto) Get()                && { return TupleElementTypeGetter<Type, sizeof...(Types)>::Get(static_cast<TupleImpl&&>(*this)); }

        template <typename Type, std::enable_if_t<TypeNumInPack<Type, Types...>::Value == 1, bool> = false>
        constexpr decltype(auto) Get() const          && { return TupleElementTypeGetter<Type, sizeof...(Types)>::Get(static_cast<const TupleImpl&&>(*this)); }

        template <typename Callback>
        decltype(auto) Apply(Callback&& callback)
        {
            return std::invoke(std::forward<Callback>(callback), Get<Indices>()...);
        }

        template <typename Callback, typename... ArgTypes>
        decltype(auto) ApplyAfter(Callback&& callback, ArgTypes&&... args)
        {
            return std::invoke(std::forward<Callback>(callback), args..., Get<Indices>()...);
        }

        template <typename Callback, typename... ArgTypes>
        decltype(auto) ApplyBefore(Callback&& callback, ArgTypes&&... args)
        {
            return std::invoke(std::forward<Callback>(callback), Get<Indices>()..., args...);
        }

        friend bool operator== (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return ((lhs.template Get<Indices>() == rhs.template Get<Indices>()) && ...);
        }

        friend bool operator!= (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return ((lhs.template Get<Indices>() != rhs.template Get<Indices>()) || ...);
        }

        friend bool operator> (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return ((lhs.template Get<Indices>() > rhs.template Get<Indices>()) && ...);
        }

        friend bool operator< (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return ((lhs.template Get<Indices>() < rhs.template Get<Indices>()) && ...);
        }
    };
}

    template <typename... Types>
    struct Tuple : public Private::TupleImpl<std::make_integer_sequence<uint32, sizeof...(Types)>, Types...>
    {
    private:
        using Super = Private::TupleImpl<std::make_integer_sequence<uint32, sizeof...(Types)>, Types...>;
    public:
        Tuple() = default;
        Tuple(const Tuple& other) = default;
        Tuple(Tuple&& other) noexcept = default;
        Tuple& operator=(const Tuple&) = default;
        Tuple& operator=(Tuple&&) noexcept = default;

        template <typename... ArgTypes, std::enable_if_t<(sizeof...(ArgTypes) > 0 && std::conjunction_v<std::is_convertible<ArgTypes&&, Types>...>), bool> = false>
        explicit(!std::conjunction_v<std::is_convertible<ArgTypes&&, Types>...>) Tuple(ArgTypes&&... args) : Super(Private::ForwardConstructor, std::forward<ArgTypes>(args)...) {}

        template <typename... OtherTypes>
        explicit Tuple(const Tuple<OtherTypes...>& other) : Super(Private::TupleConstructor, other) {}

        template <typename... OtherTypes>
        explicit Tuple(Tuple<OtherTypes...>&& other) : Super(Private::TupleConstructor, std::move(other)) {}

        template <int N> friend constexpr decltype(auto) get(               Tuple&  val) { return static_cast<               Tuple& >(val).template Get<N>(); }
        template <int N> friend constexpr decltype(auto) get(const          Tuple&  val) { return static_cast<const          Tuple& >(val).template Get<N>(); }
        template <int N> friend constexpr decltype(auto) get(               Tuple&& val) { return static_cast<               Tuple&&>(val).template Get<N>(); }
        template <int N> friend constexpr decltype(auto) get(const          Tuple&& val) { return static_cast<const          Tuple&&>(val).template Get<N>(); }
    };

    template <typename... ArgTypes>
    Tuple<std::decay_t<ArgTypes>...> MakeTuple(ArgTypes... args)
    {
        return Tuple<std::decay_t<ArgTypes>...>(std::forward<ArgTypes>(args)...);
    }
}