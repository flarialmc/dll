#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

#include <unordered_map>
#include <string>
#include <regex>
#include <atomic>
#include <mutex>

// Custom color system that extends Minecraft's §X format codes
// Standard MC codes: 0-9, a-f (basic colors), g (minecoin gold)
// Material codes: h-u (quartz, iron, netherite, redstone, copper, gold, emerald, diamond, lapis, amethyst)
// New material code: v (resin) added in 1.21.50+
// Style codes: k (obfuscated), l (bold), m (strikethrough), n (underline), o (italic), r (reset)
//
// New custom format: §x#RRGGBB for arbitrary RGB colors
// Example: "§x#FF5500Hello" displays "Hello" in orange
//
// Note: In recent MC versions (1.21.130+), ColorFromChar is inlined.
// We hook ColorFromColorCode instead, which takes a std::string (e.g., "§0", "§a").

class ColorFormatHook : public Hook {
private:
    // Map of custom color codes (full strings like "§w") to their RGB values
    // We use characters after 'v' (which is the last material color code)
    // Slots: w, x, y, z, and more can be added
    static inline std::unordered_map<std::string, MCCColor> customColorCodes;
    static inline std::mutex colorMutex;
    static inline std::atomic<int> nextSlotIndex{0};

    // Maps hex color strings to their assigned slot characters
    static inline std::unordered_map<std::string, char> hexToSlotMap;

    // Static storage for the returned color (since we need to return a pointer that persists)
    static inline MCCColor returnedColor;

    // ColorFromColorCode callback
    // Signature (1.21.130+): mce::Color* __fastcall ColorFromColorCode(std::string* colorCode)
    // Returns pointer to color directly (or nullptr if not found), no longer uses output parameter
    static MCCColor* ColorFromColorCodeCallback(std::string* colorCode);

public:
    // ColorFromColorCode takes (colorCode string*) and returns mce::Color* directly
    // Note: In older versions this was (outColor*, colorCode*) but signature changed
    typedef MCCColor* (__fastcall* ColorFromColorCodeOriginal)(std::string* colorCode);

    static inline ColorFromColorCodeOriginal funcOriginal = nullptr;

    ColorFormatHook();

    void enableHook() override;

    // Utility functions for custom color support

    // Convert hex string to MCCColor (e.g., "#FF5500" -> MCCColor)
    static MCCColor hexToColor(const std::string& hex);

    // Register a custom color and return its slot character
    // Returns the character to use after § to display this color
    static char registerCustomColor(const MCCColor& color);

    // Register a hex color and return its slot character
    static char registerHexColor(const std::string& hex);

    // Pre-process a string to convert §x#RRGGBB patterns to §[slot] format
    // This should be called before text is rendered
    static std::string preprocessColorCodes(const std::string& text);

    // Check if a character is a custom color slot
    static bool isCustomSlot(char c);

    // Get color for a custom slot (returns nullptr if not found)
    static const MCCColor* getCustomColor(char slot);

    // Clear all custom colors (useful for cleanup)
    static void clearCustomColors();

    // Get all available custom slot characters
    static constexpr const char* getCustomSlotChars() {
        return "wxyz"; // Can extend this as needed
    }

    // UTF-8 section sign (§)
    static constexpr const char* ESCAPE_SEQUENCE = "\xC2\xA7";
};
