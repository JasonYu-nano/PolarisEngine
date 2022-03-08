#include "reflect_class.hpp"

namespace Engine
{
    int32 ReflectClass::GetNumeric() const
    {
        return Numeric;
    }

    void ReflectClass::SetNumeric(int32 value)
    {
        Numeric = value;
    }
}
