#pragma once

namespace mce {
    class Color { // MCCColor
        float r;
        float g;
        float b;
        float a;

    public:
        Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    };
}
