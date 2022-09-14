/*
#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "foundation/smart_ptr.hpp"
#include "foundation/functional.hpp"
#include "app/details/app_driver.hpp"

namespace Engine
{
   class VulkanAppDriver : public IAppDriver
   {
       using GLWindowUniquePtr = UniquePtr<GLFWwindow, TFunction<void(GLFWwindow*)>>;

   public:
       virtual void Init() override;

       virtual void Shutdown() override;

       virtual void Tick() override;
   private:
       GLWindowUniquePtr Window;
   };
}
*/