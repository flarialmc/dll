#pragma once
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"

class FilePath {
private:
    char pad[0x18];
public:
    const char* path;
};

class TexturePath {
private:
    char pad[0x18];
public:
    FilePath* filePath;
};

class TextMeasureData {
public:
    float scale;
public:
    TextMeasureData(float scale = 1.f) {
        this->scale = scale;
    };
};

class CaretMeasureData {
public:
    int a;
    int b;
public:
    CaretMeasureData(int paramA = 0xFFFFFFFF, bool paramB = false) {
        this->a = paramA;
        this->b = paramB;
    };
};

class Color {
public:
    float r, g, b, a;
    float convert[4];
public:
    Color(float* convert) {
        this->convert[0] = convert[0];
        this->convert[1] = convert[1];
        this->convert[2] = convert[2];
        this->convert[3] = convert[3];
    };
public:
    Color(float r = 255.f, float g = 255.f, float b = 255.f, float a = 1.f) {
        this->r = r / 255.f;
        this->g = g / 255.f;
        this->b = b / 255.f;
        this->a = a;
    };
public:
    auto get(void) -> float* {
        float v[] = { this->r, this->g, this->b, this->a };
        return v;
    };
};

class Rect : public Vec4<float> {
public:
    Rect(float top = 0.f, float left = 0.f, float right = 0.f, float bottom = 0.f) : Vec4<float>(top, left, right, bottom) {};
public:
    Rect(Vec4<float> v) {
        *this = Rect(v.x, v.y, v.z, v.w);
    };
public:
    auto get(void) -> float* {
        float v[] = { this->x, this->y, this->z, this->w };
        return v;
    };
public:
    template<typename T>
    auto intersects(Vec2<T> pos) -> bool {
        return (pos.x >= this->x && pos.y >= this->y) && (pos.x <= this->z && pos.y <= this->w);
    };
};

class MinecraftUIRenderContext {
public:
    BUILD_ACCESS(this, class ClientInstance*, instance, 0x8);
private:
    virtual auto Function0(void) -> void; /* Constructor - MinecraftUIRenderContext::MinecraftUIRenderContext */
public:
    virtual auto getTextLength(class Font*, std::string*, float, bool) -> float;
private:
    virtual auto Function2(void) -> void;
    virtual auto Function3(void) -> void;
    virtual auto Function4(void) -> void;
public:
    virtual auto drawText(class Font*, const float*, std::string*, const float*, float, unsigned int, const TextMeasureData*, const CaretMeasureData*) -> void;
    virtual auto flushText(float) -> void;
private:
    virtual auto Function7(void) -> void;
    virtual auto Function8(void) -> void;
    virtual auto Function9(void) -> void;
    virtual auto Function10(void) -> void;
    virtual auto Function11(void) -> void;
public:
    virtual auto drawRectangle(const float*, const float*, float, int) -> void;
    virtual auto fillRectangle(const float*, const float*, float) -> void;
public:
    auto drawText(class Font* font, std::string text, Vec2<float> textPos, Color color, float fontSize) -> void {

        TextMeasureData textMeasureData = TextMeasureData(fontSize);
        CaretMeasureData caretMeasureData = CaretMeasureData();

        auto textRect = Rect(textPos.x, textPos.x + (textPos.x * fontSize), textPos.y, textPos.y + (textPos.y * fontSize / 2));
        this->drawText(font, textRect.get(), &text, color.get(), color.a, 0, &textMeasureData, &caretMeasureData);

    };
public:
    auto fillRectangle(Rect rect, Color color) -> void {

        rect = Rect(rect.x, rect.z, rect.y, rect.w);
        this->fillRectangle(rect.get(), color.get(), color.a);

    };
public:
    auto drawRectangle(Rect rect, Color color, int lineWidth) -> void {

        this->drawRectangle(rect.get(), color.get(), color.a, lineWidth);

    };
public:
    auto gameToScreenPos(Vec3<float> gamePos) -> Vec2<float>;
};