#pragma once

#include "meta_field.hpp"
#include "meta_property.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GMetaProperty : public GMetaField
    {
        MOC_GENERATED()
    public:
        GMetaProperty(StringID name, size_t offset, EMetaFlag flags)
            : Name(name)
            , Offset(offset)
        {
            Flags = flags;
        }

        StringID GetPropertyName() const { return Name; }

        template <typename T>
        T const* GetValuePtr(void const* data) const
        {
            byte const* ptr = reinterpret_cast<byte const*>(data) + Offset;
            return reinterpret_cast<T const*>(ptr);
        }

        template <typename T>
        T * GetValuePtr(void* data) const
        {
            byte* ptr = reinterpret_cast<byte*>(data) + Offset;
            return reinterpret_cast<T*>(ptr);
        }

        template <typename T>
        void SetValuePtr(void* data, T&& value) const
        {
            T* ptr = GetValuePtr<T>(data);
            *ptr = std::forward<T>(value);
        }

        template <typename T>
        void SetValuePtr(void* data, const T& value) const
        {
            T* ptr = GetValuePtr<T>(data);
            *ptr = value;
        }

    private:
        StringID Name;
        size_t Offset{ 0 };
    };

    class OBJECT_API META() GBoolProperty final : public GMetaProperty
    {
        MOC_GENERATED()
    public:
        bool GetBoolean(void const* data) const
        {
            return *GetValuePtr<bool>(data);
        }

        void SetBoolean(void* data, bool value) const
        {
            SetValuePtr<bool>(data, value);
        }
    };

    class OBJECT_API META() GNumericProperty : public GMetaProperty
    {
        MOC_GENERATED()
    public:
        virtual bool IsInteger() const { return false; }

        virtual bool IsFloatPoint() const { return false; }

        virtual int64 GetInteger(void const* data) const { return 0; }

        virtual uint64 GetUnsignedInteger(void const* data) const { return 0; }

        virtual double GetFloatPoint(void const* data) const { return 0.0; }

        virtual void SetInteger(void* data, int64 value) const  { ENSURE(0); }

        virtual void SetUnsignedInteger(void* data, uint64 value) const { ENSURE(0); }

        virtual void SetFloatPoint(void* data, double value) const  { ENSURE(0); }
    };

    class OBJECT_API META() GEnumProperty : public GMetaProperty
    {
        MOC_GENERATED()
    public:
        class GMetaEnum* GetEnum() const;

        int64 GetSignedEnumValue(void const* data) const;

        uint64 GetUnsignedEnumValue(void const* data) const;

    private:
        class GMetaEnum* Enum;
    };
}
