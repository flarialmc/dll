#pragma once

#include "SoundDescriptions.hpp"
#include "Events/Game/SoundEnginePlayEvent.hpp"
#include "Utils/Utils.hpp"

struct Sound {
    std::string name;
    Vec3<float> pos;
    std::string formatted;
    double timestamp;
    float currentAlpha = 0.0f;
    float targetAlpha = 1.0f;

    [[nodiscard]] std::pair<std::string, std::string> getSides() const {
        Vec3<float> diff = SDK::clientInstance->getLocalPlayer()->getPosition()->sub(pos).normalize();
        Vec3<float> up(0.0f, 1.0f, 0.0f);
        Vec3<float> side = diff.cross(up);
        std::string left = side.z < 0 ? " " : "<";
        std::string right = side.z > 0 ? " " : ">";
        return std::pair{left, right};
    }

    [[nodiscard]] std::string getDescription() const {
        return SoundDescriptions::getSoundDescription(name);
    }

};

class Subtitles : public Module {

private:

    std::vector<Sound> sounds;

    static void updateSoundVec(std::vector<Sound> &vec, float diff) {
        double microTime = Utils::Microtime();
        std::erase_if(vec, [microTime, diff](const Sound &obj) {
            return (microTime - obj.timestamp) > diff;
        });
    }

    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
private:
    int prevYAlign = -1; // 0 top | 1 middle | 2 bottom
    float currentRectWidth = 0.0f;
    float currentRectHeight = 0.0f;
    float targetRectWidth = 0.0f;
    float targetRectHeight = 0.0f;

public:
    Subtitles() : Module("Subtitles", "Adds Audio Subtitles.",
        IDR_CAPTIONS_PNG, "", false, {"audiosubtitles", "audio subtitles", "java", "caption"}) {
    };

    void onEnable() override;

    void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

    void onSoundEnginePlay(SoundEnginePlayEvent& event);

    // void normalRenderCore(int index, std::string& text) override;

    void onRender(RenderEvent& event);
};
