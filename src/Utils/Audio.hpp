#pragma once

#include "miniaudio.h"
#include "Utils/Logger/Logger.hpp"

namespace Audio {
    extern ma_engine engine;

    void init();
    void play(std::string audio);
    void cleanup();
}
