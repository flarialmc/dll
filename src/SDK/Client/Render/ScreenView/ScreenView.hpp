#pragma once

#include "VisualTree.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"


class ScreenView {
public:
    BUILD_ACCESS(this, class VisualTree*, VisualTree, GET_OFFSET("ScreenView::VisualTree"));
};

