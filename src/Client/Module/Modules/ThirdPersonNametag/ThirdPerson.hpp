#pragma once

#include "../Module.hpp"


class ThirdPerson : public Module {
private:
    static inline std::vector<uint8_t> original;
    static inline uintptr_t address;
public:
    ThirdPerson();;

    void defaultConfig() override;

    void onEnable() override;

    void onDisable() override;

    static void patch();

    static void unpatch();
};