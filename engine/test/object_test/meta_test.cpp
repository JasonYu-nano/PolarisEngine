#include "gtest/gtest.h"
#include "meta_object/meta_method.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    TEST(MetaTest, Pack)
    {
        int32 a = 0;
        float b = 1.0f;
        String s = "x";

        intptr* _pack = reinterpret_cast<intptr*>(Memory::Malloc(sizeof(intptr) * 3));
        int32* pa = &a;
        float* pb = &b;
        String* ps = &s;
        Memory::Memcpy(_pack, &pa, sizeof(intptr));
        Memory::Memcpy(_pack + 1, &pb, sizeof(intptr));
        Memory::Memcpy(_pack + 2, &ps, sizeof(intptr));
    }
}