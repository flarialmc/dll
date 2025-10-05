#pragma once

#include "../Module.hpp"
#include "../../../../Assets/Assets.hpp"

class ClearChat : public Module {
public:
    ClearChat() : Module("Clear Chat", "Clears the chat from the screen.",
        IDR_CLEARBG_PNG, "") {
    }

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;
    void onSetupAndRender(SetupAndRenderEvent& event);

private:
    static inline std::string layerName = "chat_panel";
    void clearChat();
};