#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Client.hpp"
#include "../../Utils/VersionUtils.hpp"
#include <windows.h>
#include <chrono>


class JavaDebugMenu : public Module {

private:
    std::string lookingAt = "minecraft:empty";
    Vec3<float> PrevPos{};
    std::string speed = "0";
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

public:

    JavaDebugMenu() : Module("Java Debug Menu", "Displays Java-style debug information.\nSimilar to F3 menu in Minecraft Java Edition.",
        IDR_F3_PNG, "F3") {
        Module::setup();
    }

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
        Listen(this, RenderEvent, &JavaDebugMenu::onRender)
        Listen(this, TickEvent, &JavaDebugMenu::onTick)
        Listen(this, KeyEvent, &JavaDebugMenu::onKey)
            Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
        Deafen(this, RenderEvent, &JavaDebugMenu::onRender)
        Deafen(this, TickEvent, &JavaDebugMenu::onTick)
        Deafen(this, KeyEvent, &JavaDebugMenu::onKey)
            Module::onDisable();
    }

    void defaultConfig() override {
        getKeybind();
        Module::defaultConfig("core");
        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 0.65f);
        if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 0.0f);
        if (settings.getSettingByName<bool>("showBg") == nullptr) settings.addSetting("showBg", true);
        if (settings.getSettingByName<std::string>("textColor") == nullptr) settings.addSetting("textColor", (std::string)"ffffff");
        if (settings.getSettingByName<float>("textOpacity") == nullptr) settings.addSetting("textOpacity", 1.0f);
        if (settings.getSettingByName<bool>("textRGB") == nullptr) settings.addSetting("textRGB", false);
        if (settings.getSettingByName<std::string>("bgColor") == nullptr) settings.addSetting("bgColor", (std::string)"000000");
        if (settings.getSettingByName<float>("bgOpacity") == nullptr) settings.addSetting("bgOpacity", 0.5f);
        if (settings.getSettingByName<bool>("bgRGB") == nullptr) settings.addSetting("bgRGB", false);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Main");
        addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
        addSlider("UI Scale", "", settings.getSettingByName<float>("uiscale")->value, 2.0f);
        addSlider("Rounding", "Rounding of the rectangle", settings.getSettingByName<float>("rounding")->value, 100, 0, false);
        addToggle("Background", "", settings.getSettingByName<bool>("showBg")->value);
        this->extraPadding();

        this->addHeader("Colors");
        addColorPicker("Text Color", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);
        addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
        this->extraPadding();

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    std::string getFacingDirection(LocalPlayer* player) {
        auto rotationComponent = player->getActorRotationComponent();
        yaw = rotationComponent->rot.y;
        pitch = rotationComponent->rot.x;
        std::string direction;
        if (yaw >= -45 && yaw < 45) direction = "South";
        else if (yaw >= 45 && yaw < 135) direction = "West";
        else if (yaw >= 135 || yaw < -135) direction = "North";
        else direction = "East";
        return std::format("Facing: {} ({:.1f} / {:.1f})", direction, yaw, pitch);
    }

    std::string getCPU() { // AI Slop
        int cpuInfo[4] = { -1 };
        char cpuBrand[0x40] = { 0 };
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuBrand, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuBrand + 16, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuBrand + 32, cpuInfo, sizeof(cpuInfo));
        return std::string(cpuBrand);
    }



    std::string getDimensionName() {
        BlockSource* blocksrc = SDK::clientInstance->getBlockSource();
        if (!blocksrc) {
            return "Unknown dimension";
        }
        std::string dim = blocksrc->getDimension()->getName();
        if (dim == "Overworld") {
            return "minecraft:overworld";
        }
        else if (dim == "Nether") {
            return "minecraft:nether";
        }
        else if (dim == "TheEnd") {
            return "minecraft:the_end";
        }
        else {
            return dim;
        }
    }

    std::string getTime() {
        const auto now = std::time(nullptr);
        const std::tm calendarTime = localtime_xp(now);

        std::string meridiem;
        std::string seperator;

        int hour = calendarTime.tm_hour;
        int minute = calendarTime.tm_min;

        if (hour - 12 < 0) {
            meridiem = "AM";
        }
        else if (hour == 0) {
            hour = 12;
            meridiem = "AM";
        }
        else if (hour == 12) {
            hour = 12;
            meridiem = "PM";
        }
        else {
            meridiem = "PM";
            hour -= 12;
        }

        seperator = minute < 10 ? ":0" : ":";

        if (hour == 24) hour = 0;

        return FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;
    }

    std::string getFormattedTime(long long seconds) {
        std::string text;
        int days = seconds / 86400;
        int hours = (seconds % 86400) / 3600;
        int mins = (seconds % 3600) / 60;
        int secs = seconds % 60;
        if (days >= 1) {
            text += std::format("{} days, ", days);
        }
        if (hours >= 1) {
            text += std::format("{} hours, ", hours);
        }
        if (mins >= 1) {
            text += std::format("{} mins, ", mins);
        }
        text += std::format("{} secs", secs);
        return text;
    }

    void onTick(TickEvent& event) {
        if (!SDK::clientInstance->getLocalPlayer())
            return;
        auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
        if (stateVectorComponent != nullptr) {
            speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
            PrevPos = stateVectorComponent->Pos;
        }
    }

    void onSetupAndRender(SetupAndRenderEvent& event) { // WAILA code
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
        if (!SDK::clientInstance->getBlockSource()) return;
        HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

        BlockPos pos = { result.blockPos.x,
                         result.blockPos.y ,
                         result.blockPos.z };
        BlockSource* blockSource = SDK::clientInstance->getBlockSource();
        try {
            BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
            if (!block) return;
            try {
                lookingAt = block->getNamespace() + ":" + block->getName();
            }
            catch (const std::exception& e) { Logger::error("Failed to get block name: {}", e.what()); }
        }
        catch (const std::exception& e) {
            Logger::error("Failed to get block: {}", e.what());
        }
    }

    void onRender(RenderEvent& event) {
        if (this->active && SDK::clientInstance->getScreenName() == "hud_screen") {

            float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);
            float textSize = Constraints::SpacingConstraint(2.0f, textHeight);
            float yPadding = Constraints::SpacingConstraint(0.025f, textHeight);
            float rounding = settings.getSettingByName<float>("rounding")->value;

            D2D1_COLOR_F textColor = settings.getSettingByName<bool>("textRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
            textColor.a = settings.getSettingByName<float>("textOpacity")->value;
            D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
            bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;

            LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
            BlockPos targetPos = { 0, 0, 0 };

            std::vector<std::string> left;
            std::vector<std::string> right;

            left.emplace_back(std::format("Flarial V2 Open Beta, Minecraft {}", VersionUtils::getFormattedVersion()));
            left.emplace_back(std::format("{} FPS", MC::fps));

            left.emplace_back("");

            if (player) {
                left.emplace_back(std::format("E: {}", player->getLevel()->getRuntimeActorList().size()));
                left.emplace_back(getDimensionName());
            }
            else {
                left.emplace_back("E: -1");
                left.emplace_back("Dimension: Unknown dimension");
            }

            left.emplace_back("");

            if (player) {
                Vec3<float> pos = *player->getPosition();
                left.emplace_back(std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", pos.x, pos.y, pos.z));
                Vec3<int> blockPos(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z));
                left.emplace_back(std::format("Block: {} {} {}", blockPos.x, blockPos.y, blockPos.z));
                left.emplace_back(std::format("Chunk: {} {} {}", static_cast<int>(pos.x / 16), static_cast<int>(pos.y / 16), static_cast<int>(pos.z / 16)));
                left.emplace_back(std::format("Chunk Coordinate: {} {}", static_cast<int>(pos.x) % 16, static_cast<int>(pos.y) % 16));
                left.emplace_back(getFacingDirection(player));
                left.emplace_back("");
                HitResult target = player->getLevel()->getHitResult();
                targetPos = { target.blockPos.x, target.blockPos.y, target.blockPos.z };
                left.emplace_back(std::format("Looking at block: {} {} {}", targetPos.x, targetPos.y, targetPos.z));
            }
            else {
                left.emplace_back("XYZ: 0 / 0 / 0");
                left.emplace_back("Block: 0 0 0");
                left.emplace_back("Chunk: 0 0 0");
                left.emplace_back("Chunk Coordinate: 0 0");
                left.emplace_back("Facing: nil (0 / 0)");
                left.emplace_back("");
                left.emplace_back("Looking at block: 0 0 0");
            }


            right.emplace_back("64bit");
            MEMORYSTATUSEX memory_status;
            memory_status.dwLength = sizeof(memory_status);
            GlobalMemoryStatusEx(&memory_status);
            int total_memory = static_cast<int>(memory_status.ullTotalPhys / 1000000);
            int free_memory = static_cast<int>(memory_status.ullAvailPhys / 1000000);
            int used_memory = total_memory - free_memory;
            right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / total_memory), used_memory, total_memory));

            right.emplace_back("");

            std::string cpuName = getCPU();
            if (!cpuName.empty()) {
                right.emplace_back(std::format("CPU: {}", getCPU()));
            }
            else {
                right.emplace_back("CPU: Unknown");
            }

            right.emplace_back("");

            right.emplace_back(std::format("Display: {}x{}", MC::windowSize.x, MC::windowSize.y));
            if (!MC::GPU.empty()) {
                right.emplace_back(MC::GPU);
            }
            else {
                right.emplace_back("Unknown GPU");
            }

            right.emplace_back("");

            right.emplace_back("Targetted Block");
            right.emplace_back(lookingAt);

            right.emplace_back("");

            right.emplace_back(std::format("Local Time: {}", getTime()));
            right.emplace_back(std::format("CPU Uptime: {}", getFormattedTime(static_cast<long long>(GetTickCount64() / 1000))));
            right.emplace_back(std::format("Minecraft Uptime: {}", getFormattedTime(
                static_cast<long long>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()
                ) / 1000
            )));

            right.emplace_back("");

            right.emplace_back(std::format("Speed: {} blocks/s", speed));


            int leftYoffset = 0.0f;
            for (size_t i = 0; i < left.size(); ++i) {
                if (settings.getSettingByName<bool>("showBg")->value && !left[i].empty()) {
                    float lineWidth = FlarialGUI::getFlarialTextSize(
                        String::StrToWStr(left[i]).c_str(),
                        30.0f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_LEADING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        true
                    ).x;
                    FlarialGUI::RoundedRect(
                        0.0f, leftYoffset - yPadding,
                        bgColor,
                        lineWidth, textHeight / 3.0f + yPadding * 2,
                        rounding, rounding
                    );
                }
                FlarialGUI::FlarialTextWithFont(
                    0.0f,  leftYoffset,
                    String::StrToWStr(left[i]).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
                leftYoffset += textHeight / 3.0f + yPadding * 2;
            }

            int rightYoffset = 0.0f;
            for (size_t i = 0; i < right.size(); ++i) {
                if (settings.getSettingByName<bool>("showBg")->value && !right[i].empty()) {
                    float lineWidth = FlarialGUI::getFlarialTextSize(
                        String::StrToWStr(right[i]).c_str(),
                        30.0f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_TRAILING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        true
                    ).x;
                    FlarialGUI::RoundedRect(
                        MC::windowSize.x - lineWidth, rightYoffset - yPadding,
                        bgColor,
                        lineWidth, textHeight / 3.0f + yPadding * 2,
                        rounding, rounding
                    );
                }
                FlarialGUI::FlarialTextWithFont(
                    MC::windowSize.x - 30.0f,  rightYoffset,
                    String::StrToWStr(right[i]).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
                rightYoffset += textHeight / 3.0f + yPadding * 2;
            }

        }
    }

    void onKey(KeyEvent& event) {
        if (this->isKeyPartOfKeybind(event.key)) {
            if (this->isKeybind(event.keys)) {
                keybindActions[0]({});
            }
        }
    };

};