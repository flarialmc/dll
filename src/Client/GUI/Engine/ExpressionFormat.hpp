#pragma once

/**
 * @file ExpressionFormat.hpp
 * @brief Dynamic expression evaluation system for HUD label formatting
 *
 * This namespace provides a rift-powered expression language for creating dynamic,
 * color-coded HUD displays. It evaluates expressions embedded in format strings,
 * enabling value-dependent coloring and computation.
 *
 * ## Overview
 *
 * The expression system allows HUD modules to define format strings that
 * respond dynamically to numeric values. For example, an FPS counter can
 * automatically show green when FPS is high, yellow when moderate, and
 * red when low.
 *
 * ## Syntax
 *
 * Expressions are enclosed in curly braces: `{expression}`
 * Literal text outside braces is passed through as-is.
 *
 * ### Available Functions
 *
 * | Function | Description | Example |
 * |----------|-------------|---------|
 * | `colorRange(val, green, yellow)` | Returns green/yellow/red tag (lower is better) | `{colorRange(value, 30, 60)}` |
 * | `colorRangeInverse(val, red, yellow)` | Returns red/yellow/green tag (higher is better) | `{colorRangeInverse(value, 30, 60)}` |
 * | `colorGradient(val, min, max)` | Smooth green-to-red gradient | `{colorGradient(value, 0, 100)}` |
 * | `colorGradientInverse(val, min, max)` | Smooth red-to-green gradient | `{colorGradientInverse(value, 0, 100)}` |
 * | `if(cond, trueResult, falseResult)` | Conditional output | `{if(value >= 50, green + "GOOD", red + "LOW")}` |
 * | `percent(val, max)` | Calculate percentage | `{percent(value, 20)}` -> "75" |
 * | `round(val, decimals)` | Round to decimal places | `{round(value, 2)}` -> "3.14" |
 * | `clamp(val, min, max)` | Clamp value to range | `{clamp(value, 0, 100)}` |
 * | `rgb(r, g, b)` | Create hex color from RGB (0-255) | `{rgb(255, 128, 0)}` -> "{#ff8000}" |
 * | `hex(str)` | Create hex color from string | `{hex("ff8800")}` -> "{#ff8800}" |
 *
 * ### Variables
 *
 * - `value` or `v` - References the numeric value passed to format()
 * - Named colors: `red`, `green`, `blue`, `yellow`, `orange`, `purple`,
 *   `pink`, `cyan`, `white`, `black`, `gray`, `reset` - Evaluate to their
 *   color tag strings for TextFormat (e.g., `red` -> "{red}")
 *
 * ## Usage Examples
 *
 * ```cpp
 * // Basic FPS display with color coding
 * std::string result = ExpressionFormat::format("{colorRangeInverse(value, 30, 60)}{value} FPS", 45.0);
 * // Result: "{yellow}45 FPS"
 *
 * // Health bar with gradient
 * std::string result = ExpressionFormat::format("HP: {colorGradientInverse(value, 0, 100)}{value}%", 75.0);
 * // Result: "HP: {#40bf00}75%"
 *
 * // Conditional with color (rift syntax)
 * std::string result = ExpressionFormat::format("Status: {if(value >= 50, green + \"GOOD\", red + \"LOW\")}", 60.0);
 * // Result: "Status: {green}GOOD"
 *
 * // Percentage calculation
 * std::string result = ExpressionFormat::format("Progress: {percent(value, 200)}%", 150.0);
 * // Result: "Progress: 75%"
 * ```
 *
 * ## Integration with TextFormat
 *
 * ExpressionFormat outputs color tags (like `{green}` or `{#ff0000}`) that are
 * then processed by TextFormat for rendering. The pipeline is:
 *
 * 1. ExpressionFormat::format() - Evaluates expressions via rift, outputs color tags
 * 2. TextFormat::parseFormattedText() - Parses color tags into segments
 * 3. FlarialGUI::FlarialTextWithFontFormatted() - Renders colored segments
 *
 * @see TextFormat.hpp for color tag parsing
 * @see Module::normalRender() for the complete formatting pipeline
 */

#include <string>
#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <rift.hpp>

namespace ExpressionFormat {

    namespace detail {

        // ===== Custom rift functions =====

        inline std::string colorRange(double val, double greenMax, double yellowMax) {
            if (val <= greenMax) return "{green}";
            if (val <= yellowMax) return "{yellow}";
            return "{red}";
        }

        inline std::string colorRangeInverse(double val, double redMax, double yellowMax) {
            if (val <= redMax) return "{red}";
            if (val <= yellowMax) return "{yellow}";
            return "{green}";
        }

        inline std::string colorGradient(double val, double minVal, double maxVal) {
            if (maxVal <= minVal) maxVal = minVal + 1.0;
            double t = std::clamp((val - minVal) / (maxVal - minVal), 0.0, 1.0);
            int r = static_cast<int>(t * 255.0);
            int g = static_cast<int>((1.0 - t) * 255.0);
            std::ostringstream oss;
            oss << "{#" << std::hex << std::setfill('0')
                << std::setw(2) << r << std::setw(2) << g << std::setw(2) << 0 << "}";
            return oss.str();
        }

        inline std::string colorGradientInverse(double val, double minVal, double maxVal) {
            if (maxVal <= minVal) maxVal = minVal + 1.0;
            double t = std::clamp((val - minVal) / (maxVal - minVal), 0.0, 1.0);
            int r = static_cast<int>((1.0 - t) * 255.0);
            int g = static_cast<int>(t * 255.0);
            std::ostringstream oss;
            oss << "{#" << std::hex << std::setfill('0')
                << std::setw(2) << r << std::setw(2) << g << std::setw(2) << 0 << "}";
            return oss.str();
        }

        inline std::string rgbFunc(double r, double g, double b) {
            int ri = std::clamp(static_cast<int>(r), 0, 255);
            int gi = std::clamp(static_cast<int>(g), 0, 255);
            int bi = static_cast<int>(std::clamp(b, 0.0, 255.0));
            std::ostringstream oss;
            oss << "{#" << std::hex << std::setfill('0')
                << std::setw(2) << ri << std::setw(2) << gi << std::setw(2) << bi << "}";
            return oss.str();
        }

        inline std::string percentFunc(double val, double max) {
            if (max == 0.0) return "0";
            int pct = static_cast<int>(std::round((val / max) * 100.0));
            return std::to_string(std::clamp(pct, 0, 100));
        }

        inline std::string roundFunc(double val, double decimals) {
            int dec = std::clamp(static_cast<int>(decimals), 0, 6);
            if (dec == 0) return std::to_string(static_cast<int>(std::round(val)));
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(dec) << val;
            return oss.str();
        }

        inline std::string clampFunc(double val, double minVal, double maxVal) {
            double result = std::clamp(val, minVal, maxVal);
            if (result == static_cast<int>(result)) {
                return std::to_string(static_cast<int>(result));
            }
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << result;
            std::string str = oss.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') str.pop_back();
            return str;
        }

        inline bool initialized = false;

    } // namespace detail

    /**
     * @brief Initialize the expression system — registers custom functions with rift
     *
     * Must be called once at startup before any format() calls.
     */
    inline void initialize() {
        if (detail::initialized) return;

        auto& config = rift::Config::get();

        // Register color functions — these return color tag strings for TextFormat
        config.registerFunction("colorRange", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("colorRange requires 3 arguments");
            return geode::Ok(rift::Value(detail::colorRange(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        config.registerFunction("colorRangeInverse", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("colorRangeInverse requires 3 arguments");
            return geode::Ok(rift::Value(detail::colorRangeInverse(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        config.registerFunction("colorGradient", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("colorGradient requires 3 arguments");
            return geode::Ok(rift::Value(detail::colorGradient(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        config.registerFunction("colorGradientInverse", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("colorGradientInverse requires 3 arguments");
            return geode::Ok(rift::Value(detail::colorGradientInverse(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        // Utility functions
        config.registerFunction("rgb", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("rgb requires 3 arguments");
            return geode::Ok(rift::Value(detail::rgbFunc(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        config.registerFunction("hex", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 1) return geode::Err("hex requires 1 argument");
            std::string hexStr = args[0].toString();
            // Strip # prefix if provided
            if (!hexStr.empty() && hexStr[0] == '#') hexStr = hexStr.substr(1);
            if (hexStr.length() != 6) return geode::Err("hex requires a 6-character hex string");
            return geode::Ok(rift::Value("{#" + hexStr + "}"));
        });

        config.registerFunction("percent", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 2) return geode::Err("percent requires 2 arguments");
            return geode::Ok(rift::Value(detail::percentFunc(args[0].toFloat(), args[1].toFloat())));
        });

        config.registerFunction("round", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 2) return geode::Err("round requires 2 arguments");
            return geode::Ok(rift::Value(detail::roundFunc(args[0].toFloat(), args[1].toFloat())));
        });

        config.registerFunction("clamp", [](std::span<rift::Value> args) -> rift::RuntimeFuncResult {
            if (args.size() < 3) return geode::Err("clamp requires 3 arguments");
            return geode::Ok(rift::Value(detail::clampFunc(args[0].toFloat(), args[1].toFloat(), args[2].toFloat())));
        });

        detail::initialized = true;
    }

    /**
     * @brief Build a rift::Object with all standard variables for a given numeric value
     *
     * Includes the value aliases (value, v, val) and all named color tag variables.
     * When the value is a whole number, it's stored as int64_t so rift's toString()
     * produces "45" instead of "45.0".
     *
     * @param value The numeric value for expression math
     * @param stringValue Optional original string representation. When provided,
     *        the value aliases resolve to this string (for non-numeric display values
     *        like IP addresses, block names, etc). Math functions still receive the
     *        numeric value parameter.
     */
    inline rift::Object buildVariables(double value, const std::string& stringValue = "") {
        rift::Object vars;

        // If a string value is provided, use it directly so {value} shows the
        // original text (e.g., "play.hive.com" instead of "0")
        rift::Value riftValue;
        if (!stringValue.empty()) {
            riftValue = stringValue;
        } else if (value == std::floor(value) && std::isfinite(value) &&
            value >= static_cast<double>(std::numeric_limits<int64_t>::min()) &&
            value <= static_cast<double>(std::numeric_limits<int64_t>::max())) {
            // Store as integer when possible so toString() gives "45" not "45.0"
            riftValue = static_cast<int64_t>(value);
        } else {
            riftValue = value;
        }

        // Value aliases (case variations for backward compatibility)
        vars["value"] = riftValue;
        vars["v"] = riftValue;
        vars["val"] = riftValue;
        vars["VALUE"] = riftValue;
        vars["Value"] = riftValue;

        // Named color variables — evaluate to their own tag strings
        // so {red} in a rift template produces literal "{red}" for TextFormat
        vars["red"] = std::string("{red}");
        vars["green"] = std::string("{green}");
        vars["blue"] = std::string("{blue}");
        vars["yellow"] = std::string("{yellow}");
        vars["orange"] = std::string("{orange}");
        vars["purple"] = std::string("{purple}");
        vars["pink"] = std::string("{pink}");
        vars["cyan"] = std::string("{cyan}");
        vars["white"] = std::string("{white}");
        vars["black"] = std::string("{black}");
        vars["gray"] = std::string("{gray}");
        vars["reset"] = std::string("{reset}");

        return vars;
    }

    /**
     * @brief Format a string by evaluating all embedded rift expressions
     *
     * @param formatString The format string containing {expression} blocks
     * @param value The numeric value available as `value`, `v`, or `val`
     * @param stringValue Optional original string representation for non-numeric values.
     *        When provided, {value} resolves to this string instead of the number.
     * @return String with expressions replaced by their results.
     *         On error, returns the original format string unchanged.
     */
    inline std::string format(const std::string& formatString, double value, const std::string& stringValue = "") {
        if (formatString.find('{') == std::string::npos) return formatString;

        auto vars = buildVariables(value, stringValue);
        auto result = rift::format(formatString, vars);

        if (result.isOk()) {
            return result.unwrap();
        }

        // On rift error, return the original string unchanged
        return formatString;
    }

    /**
     * @brief Check if a format string contains any expressions
     *
     * Quick check — looks for `{` which indicates potential rift expressions.
     *
     * @param text The format string to check
     * @return true if the string might contain expressions
     */
    inline bool hasExpressions(const std::string& text) {
        return text.find('{') != std::string::npos;
    }

    /**
     * @brief Get documentation for all available functions
     */
    inline std::string getDocumentation() {
        return
            "Expression Functions (powered by rift):\n"
            "\n"
            "COLOR FUNCTIONS:\n"
            "  {colorRange(value, green, yellow)} - Lower is better (ping-like)\n"
            "  {colorRangeInverse(value, red, yellow)} - Higher is better (FPS-like)\n"
            "  {colorGradient(value, min, max)} - Smooth green-to-red gradient\n"
            "  {colorGradientInverse(value, min, max)} - Smooth red-to-green gradient\n"
            "  {rgb(r, g, b)} - Create color from RGB (0-255)\n"
            "  {hex(\"ff8800\")} - Create color from hex string\n"
            "\n"
            "UTILITY FUNCTIONS:\n"
            "  {percent(value, max)} - Calculate percentage\n"
            "  {round(value, decimals)} - Round to decimal places\n"
            "  {clamp(value, min, max)} - Clamp value to range\n"
            "\n"
            "CONDITIONALS (rift syntax):\n"
            "  {if(value >= 50, green + \"GOOD\", red + \"LOW\")}\n"
            "  {value > 100 ? \"HIGH\" : \"normal\"}\n"
            "\n"
            "VARIABLES:\n"
            "  value, v, val - The current numeric value\n"
            "\n"
            "COLOR VARIABLES:\n"
            "  red, green, blue, yellow, orange, purple,\n"
            "  pink, cyan, white, black, gray, reset\n";
    }

} // namespace ExpressionFormat
