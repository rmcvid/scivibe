#include "plateform/windowsFrameTimer.hpp"

#ifdef _WIN32
#include <windows.h>
#include <cmath>
#include <system_error>

namespace scivibe {
    WindowsFrameTimer::WindowsFrameTimer() {
        m_Timer = CreateWaitableTimerExW(nullptr, nullptr,
            CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            TIMER_MODIFY_STATE | SYNCHRONIZE);

        // Older Windows versions do not support high-resolution waitable timers.
        if (!m_Timer) {
            m_Timer = CreateWaitableTimerExW(nullptr, nullptr, 0,
                TIMER_MODIFY_STATE | SYNCHRONIZE);
        }
        if (!m_Timer) {
            throw std::system_error(GetLastError(), std::system_category(),
                "Cannot create frame timer");
        }
    }

    WindowsFrameTimer::~WindowsFrameTimer() {
        CloseHandle(m_Timer);
    }

    void WindowsFrameTimer::WaitForEvents(double seconds) {
        if (!std::isfinite(seconds) || seconds <= 0.0)
            return;

        // Negative values are relative deadlines, expressed in units of 100 ns.
        // Application waits are at most one second (the minimum target is 1 FPS).
        LARGE_INTEGER due{};
        due.QuadPart = -static_cast<LONGLONG>(std::ceil(seconds * 10'000'000.0));
        if (!SetWaitableTimer(m_Timer, &due, 0, nullptr, nullptr, FALSE)) {
            throw std::system_error(GetLastError(), std::system_category(),
                "Cannot set frame timer");
        }

        // Wake on input/close as well as the deadline. GLFW dispatches messages
        // after this returns; Application rechecks the remaining frame budget.
        const DWORD result = MsgWaitForMultipleObjectsEx(1, &m_Timer, INFINITE,
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_FAILED) {
            throw std::system_error(GetLastError(), std::system_category(),
                "Cannot wait for frame timer");
        }
    }
}
#endif
