#pragma once
#include <memory>
#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    template <typename Type>
    using SharedPtr = std::shared_ptr<Type>;

    template <typename Type, typename... Args>
    NODISCARD SharedPtr<Type> MakeSharedPtr(Args&&... args)
    {
        return std::make_shared<Type>(Forward<Args>(args)...);
    }

    /*template <typename Type>
    using UniquePtr = std::unique_ptr<Type>;*/

    template <typename Type, typename Del = std::default_delete<Type>>
    using UniquePtr = std::unique_ptr<Type, Del>;

    template <typename Type, typename... Args>
    NODISCARD UniquePtr<Type> MakeUniquePtr(Args&&... args)
    {
        return std::make_unique<Type>(Forward<Args>(args)...);
    }

    template <typename Type>
    using WeakPtr = std::weak_ptr<Type>;
}
