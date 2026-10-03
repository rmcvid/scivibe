#pragma once

#include "window/window.hpp"
#include "pch/pch.hpp"
#include <stdexcept>
#include "core/log.hpp"

#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "Events/keyEvent.hpp"
#include "Events/mouseEvent.hpp"

#include "plateform/OpenGl/OpenGLContext.hpp"
#include <GLFW/glfw3.h>

namespace scivibe {
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow(); 

        void onUpdate() override;

        inline unsigned int GetWidth() const override {return m_Data.Width;}
        inline unsigned int GetHeight() const override {return m_Data.Height;}

        inline void SetEventCallback(const EventCallBackFn& callback) override { m_Data.EventCallback = callback;}
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline void* GetNativeWindow() const { return m_Window;};

    private :
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

        GLFWwindow* m_Window;
        GraphicsContext* m_Context;
        struct WindowData{
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallBackFn EventCallback;
        };

        WindowData m_Data;
    };

}