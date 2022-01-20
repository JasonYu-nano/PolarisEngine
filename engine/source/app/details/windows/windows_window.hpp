#pragma once
#include <windows.h>
#include <windowsx.h>
#include "definitions_app.hpp"

namespace Engine
{
    class APP_API WindowsWindow
    {
    public:
        void Init(HINSTANCE instance);

        void Shudown() {}

        HWND GetHWnd() const { return HWnd; }

    public:
        static const char_t WinClassName[];
    private:
        HWND HWnd{ nullptr };
        HDC HDc{ nullptr };

    };
}