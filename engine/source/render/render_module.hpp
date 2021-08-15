#pragma once

#include "defination_render.hpp"

namespace Engine
{
    class RENDER_API RenderModule
    {
    public:
        static RenderModule& Get()
        {
            static RenderModule render;
            return render;
        }
    };
}