#pragma once

#include "foundation/type_traits.hpp"

namespace Engine
{
    template <typename T, uint32 Index, uint32 TupleSize>
    class TupleElement
    {
    public:
        using Type = T;

        TupleElement() : Value() {}

        explicit TupleElement(T&& val) : Value(Forward<T>(val)) {}

        Type Value;
    };

    template <uint32 Index, uint32 TupleSize>
    struct TupleElementGetter
    {
        template <typename ElementType, typename TupleType>
        static decltype(auto) GetImpl(const TupleElement<ElementType, Index, TupleSize>&, TupleType&& tuple)
        {
            using RetType = typename CopyQualifiersAndRefFrom<TupleType, TupleElement<ElementType, Index, TupleSize>>::Type;
            return (static_cast<RetType>(tuple).Value);
        }

        template <typename TupleType>
        static decltype(auto) Get(TupleType&& tuple)
        {
            return GetImpl(tuple, Forward<TupleType>(tuple));
        }
    };

    template <uint32 Index, uint32 TupleSize>
    struct TupleElementCompare
    {
        template <typename TupleType>
        static bool Equals(const TupleType& lhs, const TupleType& rhs)
        {
            return lhs.template Get<Index>() == rhs.template Get<Index>() && TupleElementCompare<Index + 1, TupleSize>::Equals(lhs, rhs);
        }
    };

    template <uint32 TupleSize>
    struct TupleElementCompare<TupleSize, TupleSize>
    {
        template <typename TupleType>
        static bool Equals(const TupleType& lhs, const TupleType& rhs)
        {
            return true;
        }
    };

    template <typename Indices, typename... Types>
    class TupleImpl;

    template <uint32... Indices, typename... Types>
    class TupleImpl<std::integer_sequence<uint32, Indices...>, Types...> : public TupleElement<Types, Indices, sizeof...(Types)>...
    {
    public:
        TupleImpl() = default;

        explicit TupleImpl(Types&&... args) : TupleElement<Types, Indices, sizeof...(Types)>(Forward<Types>(args))...
        {}

        template <typename OtherTupleType>
        TupleImpl(void* placeholder, OtherTupleType&& tuple) : TupleElement<Types, Indices, sizeof...(Types)>(tuple.template Get<Indices>())...
        {}

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        decltype(auto) Get()
        {
            return TupleElementGetter<Index, sizeof...(Types)>::Get(static_cast<TupleImpl&>(*this));
        }

        template <uint32 Index, std::enable_if_t<(Index < sizeof...(Types)), bool> = false>
        decltype(auto) Get() const
        {
            return TupleElementGetter<Index, sizeof...(Types)>::Get(static_cast<const TupleImpl&>(*this));
        }

        template <typename Callback>
        decltype(auto) Apply(Callback&& callback)
        {
            std::invoke(Forward<Callback>(callback), Get<Indices>()...);
        }

        template <typename Callback, typename... ArgTypes>
        decltype(auto) ApplyAfter(Callback&& callback, ArgTypes&&... args)
        {
            std::invoke(Forward<Callback>(callback), args..., Get<Indices>()...);
        }

        template <typename Callback, typename... ArgTypes>
        decltype(auto) ApplyBefore(Callback&& callback, ArgTypes&&... args)
        {
            std::invoke(Forward<Callback>(callback), Get<Indices>()..., args...);
        }

        friend bool operator== (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return TupleElementCompare<0, sizeof...(Types)>::Equals(lhs, rhs);
        }

        friend bool operator!= (const TupleImpl& lhs, const TupleImpl& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template <typename... Types>
    class Tuple : public TupleImpl<std::make_integer_sequence<uint32, sizeof...(Types)>, Types...>
    {
        using Super = TupleImpl<std::make_integer_sequence<uint32, sizeof...(Types)>, Types...>;
    public:
        Tuple() = default;
        Tuple(const Tuple& other) = default;
        Tuple(Tuple& other) = default;

        explicit Tuple(Types&&... args) : Super(Forward<Types>(args)...) {}

        template <typename... OtherTypes>
        explicit Tuple(const Tuple<OtherTypes...>& other) : Super(nullptr, other) {}

        template <typename... OtherTypes>
        Tuple(Tuple<OtherTypes...>&& other) : Super(nullptr, MoveTemp(other)) {}

        Tuple& operator=(Tuple&&) noexcept = default;
        Tuple& operator=(const Tuple&) = default;
    };

    template <typename... ArgTypes>
    Tuple<std::decay_t<ArgTypes>...> MakeTuple(ArgTypes... args)
    {
        return Tuple<std::decay_t<ArgTypes>...>(Forward<ArgTypes>(args)...);
    }
}