#pragma once

#include "global.hpp"
#include "meta_types.hpp"
#include "foundation/string.hpp"
#include "mos_test_header.gen.hpp"

class META() MOSTestClassBase
{
public:
    MOC_GENERATED(MOSTestClassBase)

    MOSTestClassBase() = default;

    META()
    int32 GetID() const
    {
        return ID;
    }

    META()
    int32& GetID()
    {
        return ID;
    }

    META()
    bool SetID(int32 newID)
    {
        ID = newID;
        return true;
    }

private:
    META(ReadOnly, DisplayName = "ID of class")
    int32 ID{ 0 };
};

class META() MOSTestClassChild : public MOSTestClassBase
{
    MOC_GENERATED(MOSTestClassChild)
public:
    META()
    static void Log()
    {

    }

    META()
    String GetName() const
    {
        return Name;
    }

private:
    META(ReadWrite)
    String Name;
};

struct META() MOSTestStruct
{
    MOC_GENERATED(MOSTestStruct)

    META(ReadWrite)
    int64 Index;
};

enum class EMOSTestEnum : uint8
{
    Item1 META(DisplayName = "1", Tooltips = "Tips 1") = 0,
    Item2 META(DisplayName = "2"),
    Item3 META(DisplayName = "3")
};