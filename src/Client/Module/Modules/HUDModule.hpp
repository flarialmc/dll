

#pragma once

#include "Module.hpp"
#include "Events/Render/RenderEvent.hpp"

/**
 * Base class for simple HUD modules that display a single value on screen.
 *
 * HUDModule abstracts away the common boilerplate for modules that:
 * - Display a single text value (FPS, ping, coordinates, etc.)
 * - Use standard settings (position, scale, colors, alignment)
 * - Follow the normalRender pattern
 *
 * To create a HUD module, inherit from this class and implement:
 * - getDisplayValue(): Return the string to display
 * - customConfig(): Add any custom settings beyond the defaults
 * - customSettings(): Add any custom UI beyond the standard layout
 *
 * Example:
 * ```cpp
 * class FPS : public HUDModule {
 * public:
 *     FPS() : HUDModule(0, "FPS", "Shows FPS", IDR_FPS_PNG, "") {}
 *
 * protected:
 *     std::string getDisplayValue() override {
 *         return std::to_string((int)MC::fps);
 *     }
 * };
 * ```
 */
class HUDModule : public Module {

protected:
    int renderIndex;  // Index for module positioning/rendering

public:
    /**
     * Constructor for HUD modules
     * @param renderIndex The rendering index for this HUD element
     * @param name Display name of the module
     * @param description Module description/tooltip
     * @param icon Resource ID for the module icon
     * @param defaultKeybind Default keybind (empty string for none)
     * @param aliases Optional list of command aliases
     */
    HUDModule(
        int renderIndex,
        const std::string& name,
        const std::string& description,
        int icon,
        const std::string& defaultKeybind = "",
        const std::vector<std::string>& aliases = {}
    );

    /**
     * Called when the module is enabled.
     * Automatically registers RenderEvent listener.
     * Override customInit() for additional initialization.
     */
    void onEnable() override;

    /**
     * Called when the module is disabled.
     * Automatically deregisters RenderEvent listener.
     * Override customCleanup() for additional cleanup.
     */
    void onDisable() override;

    /**
     * Configures default settings for HUD modules.
     * Calls Module::defaultConfig("all") and adds text setting.
     * Override customConfig() to add module-specific settings.
     */
    void defaultConfig() override;

    /**
     * Renders the standard settings UI layout for HUD modules.
     * Includes Main, Text, Colors, and Misc sections.
     * Override customSettings() to add module-specific UI elements.
     */
    void settingsRender(float settingsOffset) override;

    /**
     * Main render event handler.
     * Gets the value from getDisplayValue() and renders it using normalRender().
     */
    void onRender(RenderEvent& event);

protected:
    /**
     * Override this to provide the value to display.
     * This is called every frame when the module is enabled.
     *
     * @return The string to display on screen
     *
     * Example:
     * ```cpp
     * std::string getDisplayValue() override {
     *     return "FPS: " + std::to_string((int)MC::fps);
     * }
     * ```
     */
    virtual std::string getDisplayValue() = 0;

    /**
     * Override this to add custom configuration settings.
     * Called after standard HUD settings are configured.
     *
     * Example:
     * ```cpp
     * void customConfig() override {
     *     setDef("showDecimals", false);
     *     setDef("updateInterval", 1.0f);
     * }
     * ```
     */
    virtual void customConfig() {}

    /**
     * Override this to add custom settings UI elements.
     * Called before the standard sections (Main, Text, Colors, Misc).
     *
     * Example:
     * ```cpp
     * void customSettings() override {
     *     addToggle("Show Decimals", "Display decimal places", "showDecimals");
     *     addSlider("Update Interval", "Refresh rate in seconds", "updateInterval", 5.0f, 0.1f);
     * }
     * ```
     */
    virtual void customSettings() {}

    /**
     * Override this for custom initialization when the module is enabled.
     * Called after RenderEvent listener is registered.
     */
    virtual void customInit() {}

    /**
     * Override this for custom cleanup when the module is disabled.
     * Called after RenderEvent listener is deregistered.
     */
    virtual void customCleanup() {}
};
