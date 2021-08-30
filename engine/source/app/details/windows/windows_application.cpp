#include "core_minimal_public.hpp"
#include "details/windows/windows_application.hpp"
#include "core_minimal_public.hpp"

namespace Engine
{
    WindowsApplication* AppInstance = nullptr;

    ApplicationBase* WindowsApplication::CreateApplication()
    {
        AppInstance = new WindowsApplication();
        AppInstance->Init();
        return AppInstance;
    }

    ApplicationBase* WindowsApplication::GetApplication()
    {
        return AppInstance;
    }

    void WindowsApplication::DestroyApplication()
    {
        delete AppInstance;
        AppInstance = nullptr;
    }

    void WindowsApplication::Tick()
    {
        MSG msg;
        // we use PeekMessage instead of GetMessage here
        // because we should not block the thread at anywhere
        // except the engine execution driver module
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate keystroke messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);
        }
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
        RegisteClass();
        Window = MakeSharedPtr<WindowsWindow>();
        Window->Init(HInstance);
    }

    void WindowsApplication::RegisteClass()
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
        auto result = RegisterClassEx(&wc);
        if (result == 0)
        {
            PL_ERROR("Application", TC("RegisterClassEx failed in windows platform"));
        }
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
            result = ::DefWindowProc(hWnd, message, wParam, lParam);
        }

        return result;
    }
    
}