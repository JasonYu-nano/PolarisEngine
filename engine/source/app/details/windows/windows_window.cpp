#include "details/windows/windows_window.hpp"
#include "core_minimal_public.hpp"

namespace Engine
{
    const char_t WindowsWindow::WinClassName[] = _T("PLWindow");

    void WindowsWindow::Init(HINSTANCE instance)
    {
        int heightAdjust = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
        int widthAdjust = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

        // create the window and use the result as the handle
        // TODO: move args to window definition
        HWnd = CreateWindowEx(
            0,
            WinClassName,                               // name of the window class
            _T("Title"),                                // title of the window
            WS_OVERLAPPEDWINDOW,                        // window style
            CW_USEDEFAULT,                              // x-position of the window
            CW_USEDEFAULT,                              // y-position of the window
            800 + widthAdjust,           // width of the window
            600 + heightAdjust,         // height of the window
            NULL,                                       // we have no parent window, NULL
            NULL,                                       // we aren't using menus, NULL
            instance,                                   // application handle
            NULL);                                      // pass pointer to current object

        if (HWnd == nullptr)
        {
            uint32 errorCode = (uint32)::GetLastError();
            LOG_ERROR("Application", "CreateWindowEx failed, error code: {0}", errorCode);
            return;
        }

        HDc = GetDC(HWnd);

        // display the window on the screen
        ::ShowWindow(HWnd, SW_SHOW);
    }

    void WindowsWindow::GetWindowSize(int32& width, int32& height) const
    {
        RECT area;
        GetClientRect(HWnd, &area);
        width = area.right;
        height = area.bottom;
    }
}