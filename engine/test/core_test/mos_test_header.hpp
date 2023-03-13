#include "global.hpp"
#include "mos/meta_types.hpp"
#include "foundation/string.hpp"

class META() MOSTestClassBase
{
public:
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
    META(ReadWrite)
    int64 Index;
};

enum class META() EMOSTestEnum
{
    Item1 META(DisplayName = "1", Tooltips = "Tips 1") = 0,
    Item2 META(DisplayName = "2"),
    Item3 META(DisplayName = "3")
};