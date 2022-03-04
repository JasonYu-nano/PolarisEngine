#pragma once

#include "reflection/meta_property.hpp"

class Param : public MetaObject
{
public:
    explicit Param(Variant&& defaultValue) : Default(defaultValue) {}

    bool HasDefault() const
    {
        return Default.IsValid();
    }

    const Variant& GetDefault() const
    {
        return Default;
    }

private:
    Variant Default{ Variant() };
};