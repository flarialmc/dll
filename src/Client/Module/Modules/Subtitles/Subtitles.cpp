#include "Subtitles.hpp"

#include <fmt/format.h>

#include "Events/Game/SoundEnginePlayEvent.hpp"
#include "SoundDescriptions.hpp"
#include "glm/glm/ext/quaternion_geometric.hpp"

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
    std::string singleSubtitle = fmt::format("{} {} {}", left, currentSoundName, right);
    this->normalRender(36, singleSubtitle);
}

void Subtitles::onSoundEnginePlay(SoundEnginePlayEvent& event) {
    currentSoundName = SoundDescriptions::getSoundDescription(event.name);

    Vec3<float> diff = SDK::clientInstance->getLocalPlayer()->getPosition()->sub(event.pos).normalize();
    Vec3<float> up(0.0f, 1.0f, 0.0f);
    Vec3<float> side = diff.cross(up);

    left = side.z < 0 ? "" : "<";
    right = side.z > 0 ? "" : ">";


    Logger::debug("Sound Name: {}", event.name);
    Logger::debug("Sound Description: {}", SoundDescriptions::getSoundDescription(event.name));
    Logger::debug("Sound Position : {}, {}, {}", event.pos.x, event.pos.y, event.pos.z);
}


