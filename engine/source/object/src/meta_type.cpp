#include "meta_type.hpp"

template <>
MetaType MetaType::FromType<void>()
{
    return MetaType(0);
}
