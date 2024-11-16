#pragma once

class NinesliceData {
public:
    float x;
    float y;
    float width;
    float height;
    float u;
    float v;
    float uWidth;
    float vHeight;
};

class NinesliceInfo {
public:
    NinesliceData topLeft;
    NinesliceData topRight;
    NinesliceData bottomLeft;
    NinesliceData bottomRight;
    float width;
    float height;
};