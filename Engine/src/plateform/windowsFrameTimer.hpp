#pragma once

#ifdef _WIN32
namespace scivibe {
    // Owns a reusable timer. Window messages can interrupt the wait.
    class WindowsFrameTimer {
    public:
        WindowsFrameTimer();
        ~WindowsFrameTimer();

        WindowsFrameTimer(const WindowsFrameTimer&) = delete;
        WindowsFrameTimer& operator=(const WindowsFrameTimer&) = delete;

        void WaitForEvents(double seconds);

    private:
        void* m_Timer = nullptr;
    };
}
#endif
