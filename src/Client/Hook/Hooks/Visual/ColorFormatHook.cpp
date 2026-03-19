#include "ColorFormatHook.hpp"

#include <algorithm>
#include <cctype>

MCCColor ColorFormatHook::hexToColor(const std::string& hex) {
    std::string cleanHex = hex;

    // Remove # prefix if present
    if (!cleanHex.empty() && cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1);
    }

    // Ensure we have 6 characters
    if (cleanHex.length() != 6) {
        return MCCColor(1.0f, 1.0f, 1.0f, 1.0f); // Default to white
    }

    // Parse RGB values
    try {
        int r = std::stoi(cleanHex.substr(0, 2), nullptr, 16);
        int g = std::stoi(cleanHex.substr(2, 2), nullptr, 16);
        int b = std::stoi(cleanHex.substr(4, 2), nullptr, 16);

        return MCCColor(r, g, b, 255);
    } catch (...) {
        return MCCColor(1.0f, 1.0f, 1.0f, 1.0f); // Default to white on error
    }
}

char ColorFormatHook::registerCustomColor(const MCCColor& color) {
    std::lock_guard<std::mutex> lock(colorMutex);

    const char* slots = getCustomSlotChars();
    int slotCount = static_cast<int>(strlen(slots));

    // Get the next available slot (cycling through available slots)
    int index = nextSlotIndex.fetch_add(1) % slotCount;
    char slot = slots[index];

    // Build the full color code string (e.g., "§w")
    std::string colorCode = std::string(ESCAPE_SEQUENCE) + slot;
    customColorCodes[colorCode] = color;

    return slot;
}

char ColorFormatHook::registerHexColor(const std::string& hex) {
    std::lock_guard<std::mutex> lock(colorMutex);

    // Normalize hex string (uppercase, without #)
    std::string normalizedHex = hex;
    if (!normalizedHex.empty() && normalizedHex[0] == '#') {
        normalizedHex = normalizedHex.substr(1);
    }
    std::transform(normalizedHex.begin(), normalizedHex.end(), normalizedHex.begin(), ::toupper);

    // Check if this hex color already has a slot
    auto it = hexToSlotMap.find(normalizedHex);
    if (it != hexToSlotMap.end()) {
        return it->second;
    }

    // Register new color
    MCCColor color = hexToColor(hex);

    const char* slots = getCustomSlotChars();
    int slotCount = static_cast<int>(strlen(slots));

    int index = nextSlotIndex.fetch_add(1) % slotCount;
    char slot = slots[index];

    // Build the full color code string (e.g., "§w")
    std::string colorCode = std::string(ESCAPE_SEQUENCE) + slot;
    customColorCodes[colorCode] = color;
    hexToSlotMap[normalizedHex] = slot;

    return slot;
}

std::string ColorFormatHook::preprocessColorCodes(const std::string& text) {
    std::string result;
    result.reserve(text.size());

    size_t i = 0;
    while (i < text.size()) {
        // Check for section sign (§) - UTF-8 is \xC2\xA7
        if (i + 1 < text.size() && text[i] == '\xC2' && text[i + 1] == '\xA7') {
            // Check for our custom format: §x#RRGGBB
            if (i + 2 < text.size() && text[i + 2] == 'x') {
                // Look for hex color following §x
                if (i + 3 < text.size() && text[i + 3] == '#') {
                    // Try to extract the 6 hex digits
                    if (i + 9 <= text.size()) {
                        std::string hexPart = text.substr(i + 3, 7); // #RRGGBB

                        // Validate hex format
                        bool validHex = hexPart.size() == 7 && hexPart[0] == '#';
                        if (validHex) {
                            for (size_t j = 1; j < 7 && validHex; j++) {
                                char c = hexPart[j];
                                validHex = std::isxdigit(static_cast<unsigned char>(c));
                            }
                        }

                        if (validHex) {
                            // Register the color and get its slot
                            char slot = registerHexColor(hexPart);

                            // Replace §x#RRGGBB with §[slot]
                            result += ESCAPE_SEQUENCE;
                            result += slot;

                            i += 10; // Skip §x#RRGGBB (2 + 1 + 7 bytes)
                            continue;
                        }
                    }
                }
            }

            // Not our custom format, pass through as-is
            result += text[i];
            result += text[i + 1];
            i += 2;
        } else {
            result += text[i];
            i++;
        }
    }

    return result;
}

bool ColorFormatHook::isCustomSlot(char c) {
    const char* slots = getCustomSlotChars();
    return strchr(slots, c) != nullptr;
}

const MCCColor* ColorFormatHook::getCustomColor(char slot) {
    std::lock_guard<std::mutex> lock(colorMutex);

    // Build the full color code string
    std::string colorCode = std::string(ESCAPE_SEQUENCE) + slot;

    auto it = customColorCodes.find(colorCode);
    if (it != customColorCodes.end()) {
        return &it->second;
    }
    return nullptr;
}

void ColorFormatHook::clearCustomColors() {
    std::lock_guard<std::mutex> lock(colorMutex);

    customColorCodes.clear();
    hexToSlotMap.clear();
    nextSlotIndex = 0;
}

MCCColor* ColorFormatHook::ColorFromColorCodeCallback(std::string* colorCode) {
    return funcOriginal(colorCode);
    // Logger::debug("[ColorFormatHook] ColorFromColorCode called with colorCode: {}", colorCode ? *colorCode : "nullptr");
    if (colorCode && colorCode->size() >= 3) {
        // Check if this is one of our custom color codes
        // The colorCode string looks like "§w", "§x", etc.
        std::lock_guard<std::mutex> lock(colorMutex);

        auto it = customColorCodes.find(*colorCode);
        if (it != customColorCodes.end()) {
            // Found a custom color - return pointer to our static storage
            // Note: This is safe because customColorCodes persists for the lifetime of the hook
            returnedColor = it->second;
            return &returnedColor;
        }
    }

    // Fall back to original Minecraft color handling
    // New signature: takes only colorCode, returns pointer to color (or nullptr if not found)
    return funcOriginal(colorCode);
}

ColorFormatHook::ColorFormatHook(): Hook("ColorFormat::ColorFromColorCode Hook", GET_SIG_ADDRESS("ColorFormat::ColorFromColorCode"))
{}

void ColorFormatHook::enableHook() {
    this->autoHook((void*)ColorFromColorCodeCallback, (void**)&funcOriginal);
}
