#pragma once

#include "Tessellator/Tessellator.hpp"
#include "libhat/Access.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ScreenContext {
public:
    MCCColor *getColorHolder() {
        return hat::member_at<MCCColor *>(this, GET_OFFSET("ScreenContext::colorHolder"));
    }

    Tessellator *getTessellator() {
        return hat::member_at<Tessellator *>(this, GET_OFFSET("ScreenContext::tessellator"));
    }
};