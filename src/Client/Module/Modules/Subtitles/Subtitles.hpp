#pragma once

#include "SoundDescriptions.hpp"
#include "Events/Game/SoundEnginePlayEvent.hpp"

struct Sound {
    std::string name;
    Vec3<float> pos;
    double timestamp;

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

    static double Microtime() {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

    static void updateSoundVec(std::vector<Sound> &vec, float diff) {
        double microTime = Microtime();
        std::erase_if(vec, [microTime, diff](const Sound &obj) {
            return (microTime - obj.timestamp) > diff;
        });
    }

public:
    Subtitles() : Module("Subtitles", "Adds Audio Subtitles.",
        IDR_ANIMATIONS_PNG, "", false, {"audiosubtitles", "audio subtitles", "java"}) {
    };

    void onEnable() override;

    void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

    void onSoundEnginePlay(SoundEnginePlayEvent& event);

    void normalRenderCore(int index, std::string& text) override;

    void onRender(RenderEvent& event);
};
