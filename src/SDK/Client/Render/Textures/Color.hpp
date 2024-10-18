#pragma once

namespace mce {
    class Color { // MCCColor
        float r;
        float g;
        float b;
        float a;

    public:
        Color() {
            this->r = 1;
            this->g = 1;
            this->b = 1;
            this->a = 1;
        }
    };
}
