#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <cstring>
#include "Hook/Hooks/Hook.hpp"

struct SplashDrawData {
    int64_t client = 0;
    int64_t font = 0;
    std::string* str = nullptr;
    Vec2<float>* pos = nullptr;
    mce::Color* color = nullptr;
    unsigned int flags = 0;
    unsigned int area = 0;
    bool valid = false;
};

class SplashTextHook : public Hook {
public:
    SplashTextHook() : Hook("SplashTextHook", GET_SIG_ADDRESS("SplashFontDraw")) {}

    // callback signature matches the game's fontdraw wrapper
    static void callback(int64_t client, int64_t font, std::string* str, Vec2<float>* pos,
                         mce::Color* color, unsigned int flags, unsigned int area)
    {


            funcOriginal(client, font, str, pos, color, flags, area);

    }

    typedef void(__fastcall *original)(int64_t client, int64_t font, std::string* str,
                                      Vec2<float>* pos, mce::Color* color, unsigned int flags,
                                      unsigned int area);

    static inline original funcOriginal = nullptr;

    void enableHook() override {
        this->autoHook((void*)callback, (void**)&funcOriginal);
    }
};
