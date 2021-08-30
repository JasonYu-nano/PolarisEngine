#include "details/windows/windows_window.hpp"

namespace Engine
{
    const tchar WindowsWindow::WinClassName[] = TC("PLWindow");

    void WindowsWindow::Init(HINSTANCE instance)
    {
        int heightAdjust = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
        int widthAdjust = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

        // create the window and use the result as the handle
        // TODO: move args to window definition
        HWnd = CreateWindowEx(
            0,
            WinClassName,                               // name of the window class
            TC("Title"),                                // title of the window
            WS_OVERLAPPEDWINDOW,                        // window style
            CW_USEDEFAULT,                              // x-position of the window
            CW_USEDEFAULT,                              // y-position of the window
            800 + widthAdjust,           // width of the window
            600 + heightAdjust,         // height of the window
            NULL,                                       // we have no parent window, NULL
            NULL,                                       // we aren't using menus, NULL
            instance,                                   // application handle
            NULL);                                      // pass pointer to current object

        HDc = GetDC(HWnd);

        // display the window on the screen
        ::ShowWindow(HWnd, SW_SHOW);
    }
}