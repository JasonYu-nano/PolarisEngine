#pragma once

#include "global.hpp"

namespace Engine
{
    template <uint32 Size, uint32 Align>
    struct AlignedBytes;

    template <uint32 Size>
    struct AlignedBytes<Size, 1>
    {
        uint8 Pad[Size];
    };

#define MAKE_ALIGNED_BYTES(align) \
    template <uint32 Size> \
    struct AlignedBytes<Size, align> \
    { \
        struct alignas(2) PaddingType \
        { \
            uint8 Pad[Size]; \
        }; \
        PaddingType Padding; \
    };

    MAKE_ALIGNED_BYTES(2);
    MAKE_ALIGNED_BYTES(4);
    MAKE_ALIGNED_BYTES(8);
    MAKE_ALIGNED_BYTES(16);

    template <typename ValueType>
    struct UntypedData : public AlignedBytes<sizeof(ValueType), alignof(ValueType)>
    {
        ValueType* GetData() { return (ValueType*)this; }

        const ValueType* GetData() const { return (ValueType*)this; }
    };
}
