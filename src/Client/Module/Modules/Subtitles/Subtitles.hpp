#pragma once

#include "Events/Game/SoundEnginePlayEvent.hpp"

class Subtitles : public Module {
private:
    std::string currentSoundName;
    std::string left;
    std::string right;

public:
    Subtitles() : Module("Subtitles", "Adds Audio Subtitles",
        IDR_ANIMATIONS_PNG, "", false, {"AudioSubtitles"}) {
    };

    void onEnable() override;

    void onDisable() override;

    void onRender(RenderEvent& event);

    void onSoundEnginePlay(SoundEnginePlayEvent& event);

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};
