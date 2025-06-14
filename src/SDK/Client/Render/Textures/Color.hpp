#pragma once

namespace mce {
    class Color { // MCCColor
    public:
        float r;
        float g;
        float b;
        float a;
        Color() {
            this->r = 1;
            this->g = 1;
            this->b = 1;
            this->a = 1;
        }

        Color(float r, float g, float b, float a) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }

        Color(float r, float g, float b) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = 1.0f; // Default alpha to opaque
        }
    };
}
