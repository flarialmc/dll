#pragma once

#include <string>

class Font {
public:
    float getLineLength(const std::string& text, float fontSize, bool showColorSymbol);
    float getLineHeight();
};
