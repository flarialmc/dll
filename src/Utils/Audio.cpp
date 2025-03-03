#define MINIAUDIO_IMPLEMENTATION // trust needed
#include "Audio.hpp"

namespace Audio {
    ma_engine engine;

    void init() {
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
            Logger::error("Failed to init sound engine");
        } else {
            Logger::success("Sound engine was initialized");
        }
    }

    void play(std::string audio) {
        if (ma_engine_play_sound(&engine, audio.c_str(), NULL) != MA_SUCCESS) {
            Logger::error("Failed to play audio file: {}", audio);
            ma_engine_uninit(&engine);
        }
    }

    void cleanup() {
        ma_engine_uninit(&engine);
    }
}