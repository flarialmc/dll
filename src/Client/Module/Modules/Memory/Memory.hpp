#pragma once

#include "../HUDModule.hpp"

class MEM : public HUDModule {
public:
    MEM() : HUDModule(4, "Memory", "Shows your current system RAM usage.",
        IDR_MEMORY_PNG, "", {"stats"}) {}

protected:
    std::string getDisplayValue() override;
};
