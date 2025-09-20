#include "Subtitles.hpp"

#include <fmt/format.h>

#include "Events/Game/SoundEnginePlayEvent.hpp"
#include "SoundDescriptions.hpp"

void Subtitles::onEnable() {
    Listen(this, RenderEvent, &Subtitles::onRender)
    Listen(this, SoundEnginePlayEvent, &Subtitles::onSoundEnginePlay)
    Module::onEnable();
}

void Subtitles::onDisable() {
    Deafen(this, RenderEvent, &Subtitles::onRender)
    Deafen(this, SoundEnginePlayEvent, &Subtitles::onSoundEnginePlay)
    Module::onDisable();
}

void Subtitles::defaultConfig() {
    setDef("enabled", false);
    Module::defaultConfig("all");
}

void Subtitles::settingsRender(float settingsOffset) {
    initSettingsPage();

    FlarialGUI::UnsetScrollView();
}

void Subtitles::onRender(RenderEvent& event) {
    if (!this->isEnabled()) return;

    std::string thing = fmt::format("{} {} {}", "jflkdsafjdl", "jfkdlsa", "jfkldsaf");

    this->normalRender(36, thing);
}

void Subtitles::onSoundEnginePlay(SoundEnginePlayEvent& event) {
    currentSoundName = SoundDescriptions::getSoundDescription(event.name);
    left = true ? "" : "<";
    right = true ? "" : ">";


    Logger::debug("Sound Name: {}", event.name);
    Logger::debug("Sound Description: {}", SoundDescriptions::getSoundDescription(event.name));
    Logger::debug("Sound Position: {}", (std::string) event.pos);
}


