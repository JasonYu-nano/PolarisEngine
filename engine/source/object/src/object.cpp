#include "object.hpp"
#include "meta_object.hpp"

bool GObject::IsA(MetaClass* metaClass) const
{
    MetaClass* myMeta = MetaObject();
    return myMeta == metaClass or myMeta->IsChildOf(metaClass);
}
