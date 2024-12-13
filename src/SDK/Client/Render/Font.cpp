#include "Font.hpp"

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

float Font::getLineLength(const std::string& text, float fontSize, bool showColorSymbol) {
    static int off = GET_OFFSET("Font::getLineLength");
    return Memory::CallVFuncI<float, const std::string&, float, bool>(off, this, text, fontSize, showColorSymbol);
}

float Font::getLineHeight() {
    static int off = GET_OFFSET("Font::getLineHeight");
    return Memory::CallVFuncI<float>(off, this);
}
