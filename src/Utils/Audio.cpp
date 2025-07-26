#define MINIAUDIO_IMPLEMENTATION
#include "Audio.hpp"
#include "miniaudio.h"

    ma_engine Audio::engine;

    void Audio::init() {
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
            LOG_ERROR("Failed to init sound engine");
        } else {
            Logger::success("Sound engine was initialized");
        }
    }

    void Audio::play(std::string audio) {
        if (ma_engine_play_sound(&engine, audio.c_str(), NULL) != MA_SUCCESS) {
            LOG_ERROR("Failed to play audio file: {}", audio);
            ma_engine_uninit(&engine);
        }
    }

    void Audio::cleanup() {
        ma_engine_uninit(&engine);
    }
