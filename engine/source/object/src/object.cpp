#include "object.hpp"
#include "meta_object.hpp"

bool GObject::IsA(GMetaClass* metaClass) const
{
    GMetaClass* myMeta = MetaObject();
    return myMeta == metaClass or myMeta->IsChildOf(metaClass);
}
