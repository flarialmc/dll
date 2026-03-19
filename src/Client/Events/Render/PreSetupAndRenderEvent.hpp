#pragma once

#include "../Event.hpp"
#include "../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"

// Event that fires BEFORE a screen's SetupAndRender, allowing rendering under UI elements
class PreSetupAndRenderEvent : public Event {
private:
    MinecraftUIRenderContext *muirc;
    std::string screenName;

public:
    explicit PreSetupAndRenderEvent(MinecraftUIRenderContext *muirc, const std::string& screen)
        : muirc(muirc), screenName(screen) {
    }

    [[nodiscard]] MinecraftUIRenderContext *getMuirc() const {
        return muirc;
    }

    [[nodiscard]] const std::string& getScreenName() const {
        return screenName;
    }
};
