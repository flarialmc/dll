#pragma once

#include <array>
#include "../Hook.hpp"

class BgfxFrameExtractorInsertHook : public Hook {
public:
    BgfxFrameExtractorInsertHook();
    void enableHook() override;
};