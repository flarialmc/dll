#pragma once

#include "Client/Core/ClientInstance.hpp"
#include "Client/GUI/RectangleArea.hpp"
#include "Client/Render/ScreenView/ScreenView.hpp"
#include "Client/Network/Packet/Packet.hpp"
// #include "Client/Render/MinecraftUIRenderContext.hpp"
//

namespace ui {
    enum class TextAlignment {
        LEFT,
        RIGHT,
        CENTER
    };
}

struct TextMeasureData {
    float textSize = 10.f;
    float linePadding = 0.f;
    bool displayShadow = false;
    bool showColorSymbols = false;
    bool hideHyphen = false;

    constexpr TextMeasureData(float textSize, bool displayShadow, bool showColorSymbols)
        : textSize(textSize), displayShadow(displayShadow), showColorSymbols(showColorSymbols) {
    }
};

struct DrawTextQueueEntry {
    std::string id;
    std::string text;
    RectangleArea rect;
    MCCColor color;
    ui::TextAlignment alignment;
    TextMeasureData textMeasureData;
    CaretMeasureData caretMeasureData;

    DrawTextQueueEntry(std::string id, std::string text, RectangleArea rect, MCCColor color, ui::TextAlignment alignment, TextMeasureData textMeasureData, CaretMeasureData caretMeasureData)
        : id(id), text(text), rect(rect), color(color), alignment(alignment), textMeasureData(textMeasureData), caretMeasureData(caretMeasureData) {
    }

    friend bool operator==(const DrawTextQueueEntry &lhs, const DrawTextQueueEntry &rhs) {
        return lhs.id == rhs.id &&
               lhs.text == rhs.text &&
               lhs.rect.top == rhs.rect.top &&
               lhs.rect.bottom == rhs.rect.bottom &&
               lhs.rect.left == rhs.rect.left &&
               lhs.rect.right == rhs.rect.right &&
               lhs.textMeasureData.textSize == rhs.textMeasureData.textSize &&
               lhs.textMeasureData.linePadding == rhs.textMeasureData.linePadding &&
               lhs.textMeasureData.displayShadow == rhs.textMeasureData.displayShadow &&
               lhs.textMeasureData.showColorSymbols == rhs.textMeasureData.showColorSymbols &&
               lhs.textMeasureData.hideHyphen == rhs.textMeasureData.hideHyphen &&
               lhs.alignment == rhs.alignment &&
               lhs.color == rhs.color &&
               lhs.caretMeasureData.Position == rhs.caretMeasureData.Position &&
               lhs.caretMeasureData.isSingleline == rhs.caretMeasureData.isSingleline;
    }
};

namespace SDK {
    extern std::vector<DrawTextQueueEntry> drawTextQueue;

    extern void pushDrawTextQueueEntry(DrawTextQueueEntry entry);

    extern ClientInstance *clientInstance;
    extern ScreenView *screenView;
    extern bool hasInstanced;
    extern std::string currentScreen;
    extern uint64_t serverPing;

    bool containsIgnoreCase(const std::string &mainString, const std::string &searchString);

    extern int getServerPing();

    extern void setCurrentScreen(const std::string &layer);

    extern std::string getCurrentScreen();

    extern std::string getServerIP();

    extern std::string getServerPort();

    extern std::shared_ptr<Packet> createPacket(int id);

    extern bool isHovered(Vec4<float> box, Vec2<float> mouse);
}
