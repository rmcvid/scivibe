#include "pch/pch.hpp"
#include "input/input.hpp"

namespace scivibe{
    class WindowsInput : public Input{
        protected:
            virtual std::pair<float,float> GetMousePositionImpl() override;
            virtual bool IsKeyPressedImpl(int keycode) override;
            virtual bool IsMouseButtonPressedImpl( int button) override;
            virtual float GetMouseXImpl() override;
            virtual float GetMouseYImpl() override;
    };
}