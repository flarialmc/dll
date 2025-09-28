#include "TestModule.hpp"

#include "SDK/Client/Block/BlockLegacy.hpp"

void TestModule::onEnable() {
    Listen(this, RenderEvent, &TestModule::onRender)
    Listen(this, SetupAndRenderEvent, &TestModule::onSetupAndRender);
    Module::onEnable();
}

void TestModule::onDisable() {
    Deafen(this, RenderEvent, &TestModule::onRender)
    Deafen(this, SetupAndRenderEvent, &TestModule::onSetupAndRender);
    Module::onDisable();
}

void TestModule::defaultConfig() {
    setDef("text", (std::string)"Test: {value}");
    Module::defaultConfig("all");
}

void TestModule::settingsRender(float settingsOffset) {
    initSettingsPage();

    addElementText("This is a test module for debugging purposes.");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}


void TestModule::onSetupAndRender(SetupAndRenderEvent& ev)
{
    if (SDK::clientInstance) if (SDK::clientInstance->getBlockSource())
    {
        BlockSource* sorc = SDK::clientInstance->getBlockSource();
        Block* vlock = sorc->getBlock({ 0,0,0 });
        Logger::debug("Block Name: {}", vlock->getBlockLegacy()->getName());
        if (vlock) {
            mce::Color trolly = vlock->getMapColor(*sorc, { 0,0,0 });
        }
    }
}
void TestModule::onRender(RenderEvent &event) {

    if (this->isEnabled()) {
        std::string testStr = "Debug Test";
        this->normalRender(0, testStr);
    }
}