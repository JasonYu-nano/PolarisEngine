#pragma once
#include "windows/minimal_windows.hpp"
#include "definitions_app.hpp"

namespace Engine
{
    class APP_API WindowsWindow
    {
    public:
        void Init(HINSTANCE instance);

        void Shutdown() {}

        HWND GetHWnd() const { return HWnd; }

        void GetWindowSize(int32& width, int32& height) const;

    public:
        static const char_t WinClassName[];
    private:
        HWND HWnd{ nullptr };
        HDC HDc{ nullptr };

    };
}