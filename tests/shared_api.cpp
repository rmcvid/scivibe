#include <scivibe.h>

#include <memory>
#include <string>

#ifdef SCIVIBE_BUILD_DLL
#error The client must import the engine, not build its exports.
#endif

namespace {
class ClientApplication : public scivibe::Application {
public:
    explicit ClientApplication(bool& destroyed) : destroyed_(destroyed) {}
    ~ClientApplication() override { destroyed_ = true; }

private:
    bool& destroyed_;
};
}

int main() {
    scivibe::Log::Init();
    // These pointers are initialized in the DLL and read by the client.
    const auto core = scivibe::Log::GetCoreLogger();
    const auto client = scivibe::Log::GetClientLogger();
    if (!core || !client || core == client || core->name() != "SCIVIBE") {
        return 1;
    }

    scivibe::Log::info("Exported info method");
    scivibe::Log::warning("Exported warning method");
    scivibe::Log::error("Exported error method");

    // Header-only types and logging must also work without the engine's PCH.
    const scivibe::WindowResizeEvent event(1280, 720);
    if (fmt::format("{}", event) != "WindowResizeEvent: 1280, 720") {
        return 2;
    }
    SCIVIBE_TRACE(event);

    bool destroyed = false;
    {
        std::unique_ptr<scivibe::Application> application =
            std::make_unique<ClientApplication>(destroyed);
    }
    return destroyed ? 0 : 3;
}
