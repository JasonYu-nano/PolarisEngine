#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class VulkanAppDriver
    {
    public:
        void Init();

        void Shutdown();

    private:
        GLFWwindow* Window;
    };
}