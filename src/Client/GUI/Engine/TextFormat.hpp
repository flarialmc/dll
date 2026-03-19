#pragma once

/**
 * @file TextFormat.hpp
 * @brief Color tag parsing system for formatted text rendering
 *
 * This namespace provides parsing and handling of color format tags embedded
 * in text strings. It works in conjunction with ExpressionFormat to enable
 * rich, colorful HUD text displays.
 *
 * ## Overview
 *
 * TextFormat parses text containing color tags like `{red}Hello {green}World`
 * into segments, each with its own color. The rendering engine then draws
 * each segment in sequence with the appropriate color.
 *
 * ## Tag Syntax
 *
 * Color tags are enclosed in curly braces: `{tagName}`
 *
 * ### Named Color Tags
 *
 * | Tag | Description |
 * |-----|-------------|
 * | `{red}` | Red color (customizable in ClickGUI) |
 * | `{green}` | Green color |
 * | `{blue}` | Blue color |
 * | `{yellow}` | Yellow color |
 * | `{orange}` | Orange color |
 * | `{purple}` | Purple color |
 * | `{pink}` | Pink color |
 * | `{cyan}` | Cyan color |
 * | `{white}` | White color |
 * | `{black}` | Black color |
 * | `{gray}` | Gray color |
 * | `{reset}` | Reset to default/module text color |
 *
 * ### Hex Color Tags
 *
 * Use `{#RRGGBB}` for custom colors:
 * - `{#ff0000}` - Red
 * - `{#00ff00}` - Green
 * - `{#ff8800}` - Orange
 *
 * ## Usage Examples
 *
 * ```cpp
 * // Simple colored text
 * std::string text = "{red}Error: {reset}Something went wrong";
 *
 * // Multiple colors
 * std::string text = "{green}HP: {yellow}50 {gray}/ {white}100";
 *
 * // Hex colors
 * std::string text = "{#ff8800}Warning: {#ffffff}Check settings";
 *
 * // Parse into segments
 * auto segments = TextFormat::parseFormattedText(text, getColorFunc);
 * for (const auto& seg : segments) {
 *     // seg.text = the text content
 *     // seg.color = the color (optional, nullopt = use default)
 *     // seg.useReset = true if {reset} was used
 * }
 * ```
 *
 * ## Integration Pipeline
 *
 * The complete text formatting pipeline is:
 *
 * 1. **Module** defines format string (e.g., "FPS: {value}")
 * 2. **ExpressionFormat** evaluates expressions → "FPS: {green}120"
 * 3. **Module** replaces {VALUE} placeholder → "FPS: {green}120"
 * 4. **TextFormat** parses color tags → [{"FPS: ", default}, {"120", green}]
 * 5. **FlarialGUI** renders each segment with its color
 *
 * ## Configuration
 *
 * Named colors are customizable through ClickGUI settings:
 * - Setting name: `formatColor_[colorname]Col` (hex value)
 * - Setting name: `formatColor_[colorname]RGB` (use RGB mode)
 * - Setting name: `formatColor_[colorname]Opacity` (alpha value)
 *
 * @see ExpressionFormat.hpp for dynamic expression evaluation
 * @see ClickGUI::getFormatColor() for color setting retrieval
 * @see FlarialGUI::FlarialTextWithFontFormatted() for rendering
 */

#include <string>
#include <vector>
#include <optional>
#include <regex>
#include <functional>
#include <algorithm>
#include <d2d1.h>

namespace TextFormat {

    /**
     * @brief Represents a segment of text with associated color information
     *
     * Each segment contains:
     * - The text content (without color tags)
     * - An optional color override
     * - A flag indicating if {reset} was used (use module's default color)
     */
    struct TextSegment {
        std::string text;                      ///< The text content of this segment
        std::optional<D2D1_COLOR_F> color;     ///< Color override, nullopt = use default
        bool useReset = false;                 ///< True if this segment should use reset/default color
    };

    /**
     * @brief List of all supported named color tags
     *
     * These colors can be customized in ClickGUI settings.
     * The "reset" tag is special - it returns to the module's default text color.
     */
    inline const std::vector<std::string> namedColors = {
        "red", "green", "blue", "yellow", "orange", "purple",
        "pink", "cyan", "white", "black", "gray", "reset"
    };

    /**
     * @brief Get the settings key name for a format color
     *
     * Format colors are stored in ClickGUI settings with the prefix "formatColor_".
     *
     * @param colorName The color name (e.g., "red")
     * @return Settings key (e.g., "formatColor_red")
     */
    inline std::string getFormatColorSettingName(const std::string& colorName) {
        return "formatColor_" + colorName;
    }

    /**
     * @brief Parse a hex color string to D2D1_COLOR_F
     *
     * Supports formats:
     * - "ff0000" (6 hex digits)
     * - "#ff0000" (# prefix with 6 hex digits)
     *
     * @param hex The hex color string
     * @return Parsed color, or nullopt if invalid
     *
     * @example
     * ```cpp
     * auto color = parseHexColor("#ff8800");
     * if (color) {
     *     // Use color.value()
     * }
     * ```
     */
    inline std::optional<D2D1_COLOR_F> parseHexColor(const std::string& hex) {
        std::string cleanHex = hex;

        // Remove # prefix if present
        if (!cleanHex.empty() && cleanHex[0] == '#') {
            cleanHex = cleanHex.substr(1);
        }

        // Must be exactly 6 characters for RRGGBB
        if (cleanHex.length() != 6) {
            return std::nullopt;
        }

        // Validate all characters are hex digits
        for (char c : cleanHex) {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                return std::nullopt;
            }
        }

        try {
            unsigned int r = std::stoul(cleanHex.substr(0, 2), nullptr, 16);
            unsigned int g = std::stoul(cleanHex.substr(2, 2), nullptr, 16);
            unsigned int b = std::stoul(cleanHex.substr(4, 2), nullptr, 16);

            return D2D1::ColorF(
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                1.0f  // Full opacity
            );
        } catch (...) {
            return std::nullopt;
        }
    }

    /**
     * @brief Check if a tag name is a valid named color
     *
     * Comparison is case-insensitive.
     *
     * @param name The tag name to check
     * @return true if it's a valid named color
     */
    inline bool isNamedColor(const std::string& name) {
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                      [](unsigned char c) { return std::tolower(c); });

        for (const auto& color : namedColors) {
            if (color == lowerName) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Check if a tag is a hex color (starts with #)
     *
     * @param name The tag content (without braces)
     * @return true if it matches hex color format (#RRGGBB)
     */
    inline bool isHexColorTag(const std::string& name) {
        return !name.empty() && name[0] == '#' && name.length() == 7;
    }

    /**
     * @brief Check if a tag is any valid color tag
     *
     * @param tagContent The tag content (without braces)
     * @return true if it's a named color, hex color, or reset tag
     */
    inline bool isValidColorTag(const std::string& tagContent) {
        std::string lower = tagContent;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                      [](unsigned char c) { return std::tolower(c); });

        return lower == "reset" || isNamedColor(lower) || isHexColorTag(tagContent);
    }

    /**
     * @brief Parse formatted text into colored segments
     *
     * Parses a string containing color tags into a vector of segments,
     * each with its text content and color information.
     *
     * @param text The formatted text string
     * @param getNamedColor Callback function to get D2D1_COLOR_F for a named color
     * @return Vector of TextSegment objects
     *
     * @example
     * ```cpp
     * auto segments = TextFormat::parseFormattedText(
     *     "{red}Hello {green}World",
     *     [](const std::string& name) {
     *         // Return color for name from settings
     *         return ClickGUI::getFormatColor(name);
     *     }
     * );
     * // segments[0] = {"Hello ", red_color, false}
     * // segments[1] = {"World", green_color, false}
     * ```
     */
    inline std::vector<TextSegment> parseFormattedText(
        const std::string& text,
        std::function<D2D1_COLOR_F(const std::string&)> getNamedColor
    ) {
        std::vector<TextSegment> segments;

        if (text.empty()) {
            return segments;
        }

        // Current color state
        std::optional<D2D1_COLOR_F> currentColor = std::nullopt;
        bool currentUseReset = true;  // Start with default color
        size_t pos = 0;
        std::string currentText;

        while (pos < text.length()) {
            // Look for opening brace
            size_t braceStart = text.find('{', pos);

            if (braceStart == std::string::npos) {
                // No more tags, add remaining text
                currentText += text.substr(pos);
                break;
            }

            // Add text before the brace
            if (braceStart > pos) {
                currentText += text.substr(pos, braceStart - pos);
            }

            // Look for closing brace
            size_t braceEnd = text.find('}', braceStart);

            if (braceEnd == std::string::npos) {
                // No closing brace, treat rest as literal text
                currentText += text.substr(braceStart);
                break;
            }

            // Extract tag content
            std::string tagContent = text.substr(braceStart + 1, braceEnd - braceStart - 1);

            // Check if this is a valid color tag
            bool isValidTag = false;
            std::optional<D2D1_COLOR_F> newColor = std::nullopt;
            bool newUseReset = false;

            // Normalize tag to lowercase for comparison
            std::string lowerTag = tagContent;
            std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(),
                          [](unsigned char c) { return std::tolower(c); });

            if (lowerTag == "reset") {
                // Reset tag - use default color
                isValidTag = true;
                newUseReset = true;
            } else if (isNamedColor(lowerTag)) {
                // Named color - get from callback
                isValidTag = true;
                newColor = getNamedColor(lowerTag);
            } else if (isHexColorTag(tagContent)) {
                // Hex color - parse directly
                auto hexColor = parseHexColor(tagContent);
                if (hexColor.has_value()) {
                    isValidTag = true;
                    newColor = hexColor;
                }
            }

            if (isValidTag) {
                // Valid color tag - save current segment and switch colors
                if (!currentText.empty()) {
                    TextSegment segment;
                    segment.text = currentText;
                    segment.color = currentColor;
                    segment.useReset = currentUseReset;
                    segments.push_back(segment);
                    currentText.clear();
                }

                // Update color state
                currentColor = newColor;
                currentUseReset = newUseReset;
            } else {
                // Not a valid color tag - keep as literal text (including braces)
                currentText += text.substr(braceStart, braceEnd - braceStart + 1);
            }

            pos = braceEnd + 1;
        }

        // Add final segment if there's remaining text
        if (!currentText.empty()) {
            TextSegment segment;
            segment.text = currentText;
            segment.color = currentColor;
            segment.useReset = currentUseReset;
            segments.push_back(segment);
        }

        return segments;
    }

    /**
     * @brief Strip all color tags from text
     *
     * Removes all valid color tags, leaving only the plain text content.
     * Useful for calculating text width/size without color formatting.
     *
     * Invalid tags (like `{notacolor}`) are kept as literal text.
     *
     * @param text The formatted text string
     * @return Plain text without color tags
     *
     * @example
     * ```cpp
     * std::string plain = stripFormatTags("{red}Hello {green}World");
     * // plain = "Hello World"
     *
     * std::string plain2 = stripFormatTags("{notacolor}Test");
     * // plain2 = "{notacolor}Test" (invalid tag preserved)
     * ```
     */
    inline std::string stripFormatTags(const std::string& text) {
        std::string result;
        result.reserve(text.length());  // Optimization: pre-allocate
        size_t pos = 0;

        while (pos < text.length()) {
            size_t braceStart = text.find('{', pos);

            if (braceStart == std::string::npos) {
                // No more braces, append rest
                result += text.substr(pos);
                break;
            }

            // Append text before brace
            result += text.substr(pos, braceStart - pos);

            size_t braceEnd = text.find('}', braceStart);

            if (braceEnd == std::string::npos) {
                // No closing brace, append rest including opening brace
                result += text.substr(braceStart);
                break;
            }

            // Extract and check tag
            std::string tagContent = text.substr(braceStart + 1, braceEnd - braceStart - 1);

            if (!isValidColorTag(tagContent)) {
                // Invalid tag - keep as literal text
                result += text.substr(braceStart, braceEnd - braceStart + 1);
            }
            // Valid tag - skip it (don't add to result)

            pos = braceEnd + 1;
        }

        return result;
    }

    /**
     * @brief Get list of all available color tag names
     *
     * Returns a copy of the named colors list for UI display or documentation.
     *
     * @return Vector of color names (lowercase)
     */
    inline std::vector<std::string> getAvailableColors() {
        return namedColors;
    }

    /**
     * @brief Get documentation string for color tags
     *
     * Returns a human-readable description of all available color tags.
     * Useful for displaying in UI tooltips or help text.
     *
     * @return Multi-line documentation string
     */
    inline std::string getDocumentation() {
        return
            "Color Tags:\n"
            "\n"
            "NAMED COLORS:\n"
            "  {red}     {green}    {blue}\n"
            "  {yellow}  {orange}   {purple}\n"
            "  {pink}    {cyan}     {gray}\n"
            "  {white}   {black}    {reset}\n"
            "\n"
            "HEX COLORS:\n"
            "  {#RRGGBB} - Custom RGB color\n"
            "  Example: {#ff8800} for orange\n"
            "\n"
            "SPECIAL:\n"
            "  {reset} - Return to module's default color\n"
            "\n"
            "Named colors can be customized in ClickGUI settings.\n";
    }

    /**
     * @brief Count the number of color tags in a string
     *
     * Useful for UI validation or optimization decisions.
     *
     * @param text The formatted text string
     * @return Number of valid color tags found
     */
    inline size_t countColorTags(const std::string& text) {
        size_t count = 0;
        size_t pos = 0;

        while (pos < text.length()) {
            size_t braceStart = text.find('{', pos);
            if (braceStart == std::string::npos) break;

            size_t braceEnd = text.find('}', braceStart);
            if (braceEnd == std::string::npos) break;

            std::string tagContent = text.substr(braceStart + 1, braceEnd - braceStart - 1);
            if (isValidColorTag(tagContent)) {
                count++;
            }

            pos = braceEnd + 1;
        }

        return count;
    }

    /**
     * @brief Check if text contains any color formatting
     *
     * Quick check to determine if text needs formatted rendering.
     *
     * @param text The text to check
     * @return true if text contains valid color tags
     */
    inline bool hasColorFormatting(const std::string& text) {
        return countColorTags(text) > 0;
    }

} // namespace TextFormat
