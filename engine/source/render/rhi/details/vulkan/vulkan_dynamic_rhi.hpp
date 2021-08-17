#pragma once
#include "definitions_render.hpp"
#include "foundation/functional.hpp"

namespace Engine
{
    class RENDER_API VulkanDynamicRHI
    {
        template<typename Type>
        using VKUniquePtr = UniquePtr<Type, TFunction<void(Type*)>>;

    public:
        void Init();

        void Shutdown();

    private:
        void InitInstance();

        VkInstance Instance;
    };
}