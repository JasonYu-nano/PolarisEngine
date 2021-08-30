#include "core_minimal_public.hpp"
#include "details/windows/windows_application.hpp"

namespace Engine
{
    WindowsApplication* AppInstance = nullptr;

    WindowsApplication* WindowsApplication::CreateApplication()
    {
        AppInstance = new WindowsApplication();
        AppInstance->Init();
        return AppInstance;
    }

    void WindowsApplication::DestroyApplication()
    {
        delete AppInstance;
        AppInstance = nullptr;
    }

    HINSTANCE WindowsApplication::GetHInstance()
    {
        ASSERT(AppInstance != nullptr);

        return AppInstance->HInstance;
    }

    HWND WindowsApplication::GetHWnd()
    {
        ASSERT(AppInstance != nullptr);
        ASSERT(AppInstance->Window != nullptr);

        return AppInstance->Window->GetHWnd();
    }

    WindowsApplication::~WindowsApplication()
    {
        
    }

    void WindowsApplication::Init()
    {
        RegisterClass();
        Window = MakeSharedPtr<WindowsWindow>();
        Window->Init(HInstance);
    }

    void WindowsApplication::RegisterClass()
    {
        HInstance = GetModuleHandle(NULL);

        // this struct holds information for the window class
        WNDCLASSEX wc;

        // clear out the window class for use
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        // fill in the struct with the needed information
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = HandleWinMsg;
        wc.hInstance = HInstance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = WindowsWindow::WinClassName;

        // register the window class
        auto ret = RegisterClassEx(&wc);
    }

    LRESULT CALLBACK WindowsApplication::HandleWinMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = 0;

        switch (message)
        {
        case WM_DESTROY:
        {

            WindowsApplication::RequestExitApp();
            PostQuitMessage(0);
        } break;
        default:
            ::DefWindowProc(hWnd, message, wParam, lParam);
        }

        return result;
    }
    
}