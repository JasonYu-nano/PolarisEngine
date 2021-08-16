#pragma once
#include "predefine/platform.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class IAppDriver
    {
    public:
        virtual void Init() = 0;

        virtual void Shutdown() = 0;
    };

    class AppDriverHelper
    {
    public:

        /** get a platform relative app driver */
        NODISCARD static UniquePtr<IAppDriver> GetDriver();
    };
}