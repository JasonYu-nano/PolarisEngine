#pragma once
#include "definitions_app.hpp"
#include "application_base.hpp"
#include "foundation/smart_ptr.hpp"
#include "details/windows/windows_window.hpp"

namespace Engine
{
    class APP_API WindowsApplication : public ApplicationBase
    {
    public:
        /**thread unsafe, don't call multiple times*/
        static ApplicationBase* CreateApplication();

        static ApplicationBase* GetApplication();

        /**thread unsafe, don't call multiple times*/
        static void DestroyApplication();

        virtual void Tick() override;

        static HINSTANCE GetHInstance();

        static HWND GetHWnd();

        static void GetFrameBufferSize(int& width, int& height);
    private:
        static LRESULT CALLBACK HandleWinMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        /** hide default constructor */
        WindowsApplication() = default;
        ~WindowsApplication();

        void Init();

        void RegisterWinClass();

    private:
        SharedPtr<WindowsWindow> Window{ nullptr };

        HINSTANCE HInstance{ nullptr };
    };

    typedef WindowsApplication PlatformApplication;
}