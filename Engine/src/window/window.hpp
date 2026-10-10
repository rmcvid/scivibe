#pragma once
#include "pch/pch.hpp"
#include "core/core.hpp"
#include "Events/event.hpp"
#include "core/capturedFrame.hpp"

namespace scivibe{
    struct WindowProps{
        std::string Title;
        unsigned int Width;
        unsigned int Heigth;

        WindowProps(const std::string& title = "Scivibe",
                    unsigned int width = 1280,
                    unsigned int heigth = 720
                ): Title(title), Width(width), Heigth(heigth)
                {}        
    };
    // la classe est virtuelle car elle doit etre traitée par plateforme
    class SCIVIBE_API Window{
        public : 
            using EventCallBackFn = std::function<void(Event&)>;
            virtual~ Window(){}
            virtual void onUpdate() = 0;
            virtual void PollEvents() = 0;
            virtual void SwapBuffers() = 0;
            // Dispatches pending events, waiting up to timeout seconds for input.
            virtual void WaitEvents(double timeout) = 0;
            virtual unsigned int GetWidth() const = 0;
            virtual unsigned int GetHeight() const = 0;
            virtual void SetEventCallback(const EventCallBackFn& callback)= 0;
            virtual void SetVSync(bool eanbled) = 0;
            virtual bool IsVSync() const = 0;
            virtual bool CaptureFrame(CapturedFrame& frame) = 0;
            virtual void* GetNativeWindow() const = 0;
            
            static Window* Create(const WindowProps& props = WindowProps());
    };
}
