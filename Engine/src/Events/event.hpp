
#pragma once
#include <ostream>
#include <string>
#include <type_traits>
#include <spdlog/fmt/ostr.h>

namespace scivibe {

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput       = 1 << 1,
        EventCategoryKeyboard    = 1 << 2,
        EventCategoryMouse       = 1 << 3,
        EventCategoryMouseButton = 1 << 4
    };
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                                virtual EventType GetEventType() const override { return GetStaticType(); }\
                                virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


    class Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }
        bool GetHandled() const {return m_Handled;}
        inline bool IsInCategory(EventCategory category) {
            return GetCategoryFlags() & category;
        }
    protected:
        bool m_Handled = false;
    };

    class EventDispatcher {
    public:
        EventDispatcher(Event& event)
            : m_Event(event) {}

        template<typename T, typename F>
        bool Dispatch(const F& func) {
            if (m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.m_Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.ToString();
    }

} // namespace scivibe

// overload << pour le log
template <typename T>
struct fmt::formatter<T, 
    char,
    std::enable_if_t<std::is_base_of_v<scivibe::Event, T>>
    > 
    : fmt::ostream_formatter {};
