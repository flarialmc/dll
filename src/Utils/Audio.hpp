#pragma once

#include "../lib/miniaudio.h"
#include "Utils/Logger/Logger.hpp"

namespace Audio {
    extern ma_engine engine;

    void init();
    void play(char* audio);
    void cleanup();
}
