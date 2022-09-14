#pragma once
#include <functional>

namespace Engine
{
    template<typename Func>
    using TFunction = std::function<Func>;
}