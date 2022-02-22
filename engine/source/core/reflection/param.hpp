#pragma once

#include "reflection/meta_property.hpp"

class CORE_API Param
{
public:
    bool HasDefault() const
    {
        return Default.IsValid();
    }

    const Variant& GetDefault() const
    {
        return Default;
    }

private:
    Variant Default;
};