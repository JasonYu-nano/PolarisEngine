#pragma once
#include <memory>
#include "global.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    template <typename Type>
    using SharedPtr = std::shared_ptr<Type>;

    template <typename Type, typename... Args>
    NODISCARD SharedPtr<Type> MakeShared(Args&&... args)
    {
        return std::make_shared<Type>(Forward<Args>(args)...);
    }

    /*template <typename Type>
    using UniquePtr = std::unique_ptr<Type>;*/

    template <typename Type, typename Del = std::default_delete<Type>>
    using UniquePtr = std::unique_ptr<Type, Del>;

    template <typename Type, typename... Args>
    NODISCARD UniquePtr<Type> MakeUnique(Args&&... args)
    {
        return std::make_unique<Type>(Forward<Args>(args)...);
    }

    template <typename Type>
    using WeakPtr = std::weak_ptr<Type>;
}
