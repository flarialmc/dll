#pragma once

#include "UIControl.hpp"

class VisualTree {
public:
    BUILD_ACCESS(this, class UIControl*, root, GET_OFFSET("VisualTree::root"));
};