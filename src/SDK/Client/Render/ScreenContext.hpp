#pragma once

#include "Tessellator/Tessellator.h"
#include "libhat/Access.hpp"

class ScreenContext {
public:
    inline MCCColor *getColorHolder() {
        return hat::member_at<MCCColor *>(this, GET_OFFSET("ScreenContext::colorHolder"));
    }

    inline Tessellator *getTessellator() {
        return hat::member_at<Tessellator *>(this, GET_OFFSET("ScreenContext::tessellator"));
    }
};