#include "math/quaternion.hpp"

namespace Engine
{
    const Quat Quat::Identity = Quat(0.0f);

    Quat::Quat(const Rotator& rotator)
    {
        *this = rotator.ToQuaternion();
    }
}