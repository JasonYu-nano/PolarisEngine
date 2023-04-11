#include "variant.hpp"

bool Variant::CanConvertTo(MetaType metaType) const
{
    //TODO: Check me
    if (TypeID == metaType.ID())
    {
        return true;
    }

    return false;
}
