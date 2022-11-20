#pragma once

#include "foundation/type_traits.hpp"

namespace Engine
{
    struct ZeroArgPlaceholder {};

    struct OneArgPlaceholder {};

    template <typename T1, typename T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
    class CompressedPair : private T1
    {
        using Super = T1;
    public:
        CompressedPair() = default;

        template <typename... ArgType2>
        explicit CompressedPair(ZeroArgPlaceholder, ArgType2&&... arg2) : Super(), SecondVal(Forward<ArgType2>(arg2)...) {}

        template <typename ArgType1, typename... ArgType2>
        explicit CompressedPair(OneArgPlaceholder, ArgType1&& arg1, ArgType2&&... arg2) : Super(Forward<ArgType1>(arg1)), SecondVal(Forward<ArgType2>(arg2)...) {}

        T1& First()
        {
            return *this;
        }

        const T1& First() const
        {
            return *this;
        }

        T2& Second()
        {
            return SecondVal;
        }

        const T2& Second() const
        {
            return SecondVal;
        }

    private:
        T2 SecondVal;
    };

    template <typename T1, typename T2>
    class CompressedPair<T1, T2, false> final
    {
    public:
        template <typename... ArgType2>
        explicit CompressedPair(ArgType2&&... arg2) : FirstVal(), SecondVal(Forward<ArgType2>(arg2)...) {}

        template <typename ArgType1, typename... ArgType2>
        explicit CompressedPair(ArgType1&& arg1, ArgType2&&... arg2) : FirstVal(Forward<ArgType1>(arg1)), SecondVal(Forward<ArgType2>(arg2)...) {}

        T1& First()
        {
            return FirstVal;
        }

        const T1& First() const
        {
            return FirstVal;
        }

        T2& Second()
        {
            return SecondVal;
        }

        const T2& Second() const
        {
            return SecondVal;
        }

    private:
        T1 FirstVal;
        T2 SecondVal;
    };
}
