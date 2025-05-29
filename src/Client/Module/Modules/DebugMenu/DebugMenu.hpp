#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../Manager.hpp"
#include "../../../Client.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <intrin.h>
#include <dxgi.h>
#include "../../Utils/VersionUtils.hpp"

class JavaDebugMenu : public Module {
private:
    float lastFpsUpdate = 0.0f;
    int frameCount = 0;
    int fps = 0;
    Vec3<float> PrevPos{};
    std::string speed;


    ULONGLONG prevKernelTime = 0;
    ULONGLONG prevUserTime = 0;
    ULONGLONG prevIdleTime = 0;
    float lastCpuUpdate = 0.0f;
    float cpuUsage = 0.0f;


    std::vector<float> ticks;
    float lastTpsUpdate = 0.0f;
    float tps = 0.0f;


    time_t startTime;
    float lastUptimeUpdate = 0.0f;
    std::string uptime;


    static std::string getBiome() {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->getBlockSource()) {
            return "Unknown Biome";
        }


        Vec3<float>* pos = SDK::clientInstance->getLocalPlayer()->getPosition();
        Vec3<int> blockPos(static_cast<int>(pos->x), static_cast<int>(pos->y), static_cast<int>(pos->z));


        Biome* biome = SDK::clientInstance->getBlockSource()->getBiome(blockPos);
        if (!biome) {
            return "Unknown Biome";
        }

        // Try accessing the name field
        try {
            return "Unknown";
        } catch (const std::exception& e) {
            Logger::error("Failed to get biome name: {}", e.what());
            return "Unknown Biome";
        }
    }


    static std::pair<int, int> getLightLevel() {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return {0, 0};
        return {15, 0}; // Placeholder
    }


    static std::string getFacingDirection(float yaw, float pitch) {
        std::string direction;
        if (yaw >= -45 && yaw < 45) direction = "South";
        else if (yaw >= 45 && yaw < 135) direction = "West";
        else if (yaw >= 135 || yaw < -135) direction = "North";
        else direction = "East";
        return direction + " (Yaw: " + std::to_string(static_cast<int>(yaw)) + ", Pitch: " + std::to_string(static_cast<int>(pitch)) + ")";
    }


    static std::string getCoordinates() {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return "XYZ: 0.0 / 0.0 / 0.0";
        Vec3<float> *position = SDK::clientInstance->getLocalPlayer()->getPosition();
        std::string ln1 = std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", position->x, position->y, position->z);
        std::string ln2;
        BlockSource* blocksrc = SDK::clientInstance->getBlockSource();
        std::string dimName = std::string(blocksrc->getDimension()->getName().data());
        if (dimName == "Nether") {
            ln2 = std::format("Overworld: {:.1f} / {:.1f} / {:.1f}", position->x * 8.f, position->y, position->z * 8.f);
        }
        return ln1 + (ln2.empty() ? "" : "\n" + ln2);
    }


    void updateSpeed() {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) {
            speed = "Speed: 0.00 m/s";
            return;
        }
        auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
        if (stateVectorComponent != nullptr) {
            speed = std::format("Speed: {:.2f} m/s", stateVectorComponent->Pos.dist(PrevPos) * 20);
            PrevPos = stateVectorComponent->Pos;
        }
    }


    static std::string getLookingAt(bool advanced) {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return "";
        if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return "";
        if (!SDK::clientInstance->getBlockSource()) return "";

        HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();
        BlockPos pos = { result.blockPos.x, result.blockPos.y, result.blockPos.z };
        BlockSource* blockSource = SDK::clientInstance->getBlockSource();

        try {
            BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
            if (!block) return "";
            try {
                if (!advanced) return "Looking at: " + block->getName();
                else return "Looking at: " + block->getNamespace();
            } catch (const std::exception& e) {
                Logger::error("Failed to get block name: {}", e.what());
                return "";
            }
        } catch (const std::exception& e) {
            Logger::error("Failed to get block: {}", e.what());
            return "";
        }
    }


    static std::string getWeatherState() {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->getBlockSource()) {
            return "Weather: Unknown";
        }

        auto dimension = SDK::clientInstance->getBlockSource()->getDimension();
        if (!dimension || !dimension->weather) return "Weather: Unknown";


        Vec3<float>* pos = SDK::clientInstance->getLocalPlayer()->getPosition();
        Vec3<int> blockPos(static_cast<int>(pos->x), static_cast<int>(pos->y), static_cast<int>(pos->z));
        Biome* biome = SDK::clientInstance->getBlockSource()->getBiome(blockPos);
        if (!biome) return "Weather: Unknown";


        float rainLevel = dimension->weather->rainLevel;
        float lightingLevel = dimension->weather->lightingLevel;
        float temperature = biome->temparature;


        std::string weatherStr = "Weather: ";

        // Handle rain
        if (rainLevel > 0.0f) {
            std::string intensity = (rainLevel >= 0.8f) ? " (Heavy)" : (rainLevel <= 0.3f) ? " (Light)" : "";
            weatherStr += "Rain" + intensity;
        } else {
            weatherStr += "Clear";
        }

        // Handle lighting (ambient darkness)
        if (lightingLevel > 0.0f) {
            std::string lightIntensity = (lightingLevel >= 0.8f) ? " (Dark)" : (lightingLevel <= 0.3f) ? " (Dim)" : "";
            weatherStr += ", Lighting" + lightIntensity;
        }

        // Handle snow based on biome temperature
        if (temperature <= 0.0f) {
            // Map temperature (0.7 to 0.0) back to snow intensity (0.0 to 1.0)
            float snowLevel = 1.0f - (temperature / 0.7f); // Inverse of WeatherChanger's mapping
            snowLevel = std::clamp(snowLevel, 0.0f, 1.0f);
            std::string snowIntensity = (snowLevel >= 0.8f) ? " (Heavy)" : (snowLevel <= 0.3f) ? " (Light)" : "";
            weatherStr += ", Snow" + snowIntensity;
        }

        return weatherStr;
    }

    // Helper function to get local system time in HH:MM:SS AM/PM format
    std::string getLocalTime() {
        time_t now = std::time(nullptr);
        struct tm* localTime = std::localtime(&now);
        if (!localTime) return "Local Time: Unknown";

        int hours = localTime->tm_hour;
        int minutes = localTime->tm_min;
        int seconds = localTime->tm_sec;
        std::string period = hours >= 12 ? "PM" : "AM";
        hours = hours % 12;
        if (hours == 0) hours = 12; // Convert 0 to 12 for 12-hour clock
        return std::format("Local Time: {:02d}:{:02d}:{:02d} {}", hours, minutes, seconds, period);
    }

    // Helper function to update and get TPS
    void updateTPS() {
        float currentTime = ImGui::GetTime();
        ticks.push_back(currentTime);

        // Remove ticks older than 0.99 seconds
        while (!ticks.empty() && ticks.front() < currentTime - 0.99f) {
            ticks.erase(ticks.begin());
        }

        // Update TPS every second
        if (currentTime - lastTpsUpdate >= 1.0f) {
            tps = static_cast<float>(ticks.size());
            lastTpsUpdate = currentTime;
        }
    }

    std::string getTPS() {
        return std::format("TPS: {:.1f}", tps);
    }

    // Helper function to update and get uptime
    std::string getUptime() {
        time_t now = std::time(nullptr);
        long long elapsed = static_cast<long long>(now - startTime);
        int hours = elapsed / 3600;
        int minutes = (elapsed % 3600) / 60;
        int seconds = elapsed % 60;
        return std::format("Uptime: {}h {}m {}s", hours, minutes, seconds);
    }

    // Helper function to get memory usage (split into available and used)
    static std::pair<std::string, std::string> getMemoryInfo() {
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        GlobalMemoryStatusEx(&memStatus);
        double usedMemoryGB = static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024 * 1024);
        double availableMemoryGB = static_cast<double>(memStatus.ullAvailPhys) / (1024 * 1024 * 1024);
        return {
            std::format("RAM Used: {:.2f} GB", usedMemoryGB),
            std::format("RAM Available: {:.2f} GB", availableMemoryGB)
        };
    }

    // Helper function to get CPU name
    static std::string getCpuName() {
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

    // Helper function to get CPU architecture
    static std::string getCpuArchitecture() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
        case PROCESSOR_ARCHITECTURE_ARM: return "ARM";
        case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
        case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
        default: return "Unknown";
        }
    }

    // Helper function to get CPU usage using Win32 API
    void updateCpuUsage() {
        FILETIME idleTime, kernelTime, userTime;
        if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            Logger::error("Failed to get system times: {}", GetLastError());
            cpuUsage = 0.0f;
            return;
        }

        ULONGLONG currentIdle = (static_cast<ULONGLONG>(idleTime.dwHighDateTime) << 32) | idleTime.dwLowDateTime;
        ULONGLONG currentKernel = (static_cast<ULONGLONG>(kernelTime.dwHighDateTime) << 32) | kernelTime.dwLowDateTime;
        ULONGLONG currentUser = (static_cast<ULONGLONG>(userTime.dwHighDateTime) << 32) | userTime.dwLowDateTime;

        if (prevKernelTime != 0 && prevUserTime != 0 && prevIdleTime != 0) {
            ULONGLONG kernelDiff = currentKernel - prevKernelTime;
            ULONGLONG userDiff = currentUser - prevUserTime;
            ULONGLONG idleDiff = currentIdle - prevIdleTime;
            ULONGLONG totalDiff = kernelDiff + userDiff + idleDiff;

            if (totalDiff > 0) {
                cpuUsage = static_cast<float>(kernelDiff + userDiff - idleDiff) / totalDiff * 100.0f;
                cpuUsage = std::clamp(cpuUsage, 0.0f, 100.0f);
            } else {
                cpuUsage = 0.0f;
            }
        }

        prevIdleTime = currentIdle;
        prevKernelTime = currentKernel;
        prevUserTime = currentUser;
    }

    std::string getCpuUsage() {
        return std::format("CPU Usage: {:.1f}%", cpuUsage);
    }

    // Helper function to get GPU name using DXGI
    static std::string getGpuName() {
        HRESULT hr = S_OK;
        IDXGIFactory* factory = nullptr;
        IDXGIAdapter* adapter = nullptr;
        std::wstring gpuName;

        hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
        if (FAILED(hr)) {
            Logger::error("Failed to create DXGI factory");
            return "GPU: Unknown";
        }

        hr = factory->EnumAdapters(0, &adapter);
        if (FAILED(hr)) {
            Logger::error("Failed to enumerate DXGI adapters");
            factory->Release();
            return "GPU: Unknown";
        }

        DXGI_ADAPTER_DESC desc;
        hr = adapter->GetDesc(&desc);
        if (FAILED(hr)) {
            Logger::error("Failed to get DXGI adapter description");
            adapter->Release();
            factory->Release();
            return "GPU: Unknown";
        }

        gpuName = desc.Description;
        adapter->Release();
        factory->Release();

        return "GPU: " + std::string(gpuName.begin(), gpuName.end());
    }

public:
    JavaDebugMenu() : Module("Java Debug Menu", "Displays Java-style debug information.\nSimilar to F3 menu in Minecraft Java Edition.",
        IDR_F3_PNG, "F3") {
        Module::setup();
        defaultConfig(); // Apply default config immediately
        // Initialize CPU usage tracking
        lastCpuUpdate = ImGui::GetTime();
        updateCpuUsage(); // Get initial times
        // Initialize uptime
        startTime = std::time(nullptr);
        lastUptimeUpdate = ImGui::GetTime();
        // Initialize TPS
        lastTpsUpdate = ImGui::GetTime();
    }

    void onEnable() override {
        Listen(this, RenderEvent, &JavaDebugMenu::onRender);
        Listen(this, KeyEvent, &JavaDebugMenu::onKey);
        Listen(this, TickEvent, &JavaDebugMenu::onTick);
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &JavaDebugMenu::onRender);
        Deafen(this, KeyEvent, &JavaDebugMenu::onKey);
        Deafen(this, TickEvent, &JavaDebugMenu::onTick);
        Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig();
        settings.addSetting<float>("textSize", 0.3f);
        settings.addSetting<bool>("showVersionInfo", true);
        settings.addSetting<bool>("showFPS", true);
        settings.addSetting<bool>("showDimension", true);
        settings.addSetting<bool>("showCoords", true);
        settings.addSetting<bool>("showSpeed", true);
        settings.addSetting<bool>("showFacing", true);
        settings.addSetting<bool>("showLookingAt", true);
        settings.addSetting<bool>("showAdvancedLookingAt", false);
        settings.addSetting<bool>("showBiome", true);
        settings.addSetting<bool>("showLight", true);
        settings.addSetting<bool>("showWeather", true);
        settings.addSetting<bool>("showTPS", true);
        settings.addSetting<bool>("showUptime", true);

        Logger::debug("JavaDebugMenu: Default config applied - showFPS: {}, percentageX: {}, percentageY: {}",
            settings.getSettingByName<bool>("showFPS")->value,
            settings.getSettingByName<float>("percentageX")->value,
            settings.getSettingByName<float>("percentageY")->value);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Debug Menu");
        this->addSlider("Text Size", "", this->settings.getSettingByName<float>("textSize")->value, 5.0f, 0.1f, false);
        this->addToggle("Show Version Info", "", this->settings.getSettingByName<bool>("showVersionInfo")->value);
        this->addToggle("Show FPS", "", this->settings.getSettingByName<bool>("showFPS")->value);
        this->addToggle("Show Dimension", "", this->settings.getSettingByName<bool>("showDimension")->value);
        this->addToggle("Show Coordinates", "", this->settings.getSettingByName<bool>("showCoords")->value);
        this->addToggle("Show Speed", "", this->settings.getSettingByName<bool>("showSpeed")->value);
        this->addToggle("Show Facing", "", this->settings.getSettingByName<bool>("showFacing")->value);
        this->addToggle("Show Looking At", "", this->settings.getSettingByName<bool>("showLookingAt")->value);
        this->addToggle("Show Advanced Looking At", "Show block namespace", this->settings.getSettingByName<bool>("showAdvancedLookingAt")->value);
        this->addToggle("Show Biome", "", this->settings.getSettingByName<bool>("showBiome")->value);
        this->addToggle("Show Light Level", "", this->settings.getSettingByName<bool>("showLight")->value);
        this->addToggle("Show Weather", "", this->settings.getSettingByName<bool>("showWeather")->value);
        this->addToggle("Show Local Time", "", this->settings.getSettingByName<bool>("showLocalTime")->value);
        this->addToggle("Show TPS", "", this->settings.getSettingByName<bool>("showTPS")->value);
        this->addToggle("Show Uptime", "", this->settings.getSettingByName<bool>("showUptime")->value);
        this->extraPadding();

        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
        this->extraPadding();

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void normalRender(int index, std::string& value) override {
        if (!SDK::hasInstanced) {
            Logger::debug("JavaDebugMenu: SDK not instanced");
            return;
        }
        if (!active && !ClickGUI::editmenu) {
            Logger::debug("JavaDebugMenu: Module not active and not in edit menu");
            return;
        }
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) {
            Logger::debug("JavaDebugMenu: Client instance or local player not available");
            return;
        }

        // Calculate FPS
        frameCount++;
        float currentTime = ImGui::GetTime();
        if (currentTime - lastFpsUpdate >= 1.0f) {
            fps = frameCount;
            frameCount = 0;
            lastFpsUpdate = currentTime;
        }

        // Update CPU usage every second
        if (currentTime - lastCpuUpdate >= 1.0f) {
            updateCpuUsage();
            lastCpuUpdate = currentTime;
        }

        // Update uptime every second
        if (currentTime - lastUptimeUpdate >= 1.0f) {
            uptime = getUptime();
            lastUptimeUpdate = currentTime;
        }

        // Gather debug information
        auto player = SDK::clientInstance->getLocalPlayer();
        Vec3<float> *pos = player->getPosition();
        BlockSource* blocksrc = SDK::clientInstance->getBlockSource();

        std::vector<std::string> leftDebugLines;
        std::vector<std::string> rightDebugLines;

        // Left column: Minecraft details
        if (settings.getSettingByName<bool>("showVersionInfo")->value) {
            leftDebugLines.push_back("Flarial V2 Beta, Minecraft " + VersionUtils::getFormattedVersion());
        }
        if (settings.getSettingByName<bool>("showFPS")->value) {
            leftDebugLines.push_back("FPS: " + std::to_string(fps));
        }
        if (settings.getSettingByName<bool>("showDimension")->value) {
            leftDebugLines.push_back("Dimension: " + std::string(blocksrc->getDimension()->getName().data()));
        }
        if (settings.getSettingByName<bool>("showCoords")->value) {
            leftDebugLines.push_back(getCoordinates());
        }
        if (settings.getSettingByName<bool>("showSpeed")->value) {
            leftDebugLines.push_back(speed);
        }
        if (settings.getSettingByName<bool>("showFacing")->value) {
            float yaw = 0.0f, pitch = 0.0f;
            auto rotationComponent = player->getActorRotationComponent();
            if (rotationComponent) {
                yaw = rotationComponent->rot.y;
                pitch = rotationComponent->rot.x;
            }
            leftDebugLines.push_back("Facing: " + getFacingDirection(yaw, pitch));
        }
        if (settings.getSettingByName<bool>("showLookingAt")->value) {
            bool advanced = settings.getSettingByName<bool>("showAdvancedLookingAt")->value;
            std::string lookingAt = getLookingAt(advanced);
            if (!lookingAt.empty()) leftDebugLines.push_back(lookingAt);
        }
        if (settings.getSettingByName<bool>("showBiome")->value) {
            leftDebugLines.push_back("Biome: " + getBiome());
        }
        if (settings.getSettingByName<bool>("showLight")->value) {
            auto [blockLight, skyLight] = getLightLevel();
            leftDebugLines.push_back("Light: " + std::to_string(blockLight) + " (Block), " + std::to_string(skyLight) + " (Sky)");
        }
        if (settings.getSettingByName<bool>("showWeather")->value) {
            leftDebugLines.push_back(getWeatherState());
        }
        if (settings.getSettingByName<bool>("showTPS")->value) {
            leftDebugLines.push_back(getTPS());
        }
        if (settings.getSettingByName<bool>("showUptime")->value) {
            leftDebugLines.push_back(uptime);
        }

        // Right column: System info (always render)
        auto [used, available] = getMemoryInfo();
        rightDebugLines.push_back(used);
        rightDebugLines.push_back(available);
        std::string cpuUsageStr = getCpuUsage();
        std::string cpuName = "CPU: " + getCpuName();
        std::string cpuArch = getCpuArchitecture();
        std::string gpuName = getGpuName();
        rightDebugLines.push_back(cpuUsageStr);
        rightDebugLines.push_back(cpuName);
        rightDebugLines.push_back(cpuArch);
        rightDebugLines.push_back(gpuName);

        Logger::debug("JavaDebugMenu: System info - RAM Used: {}, RAM Available: {}, CPU Usage: {}, CPU: {}, Arch: {}, GPU: {}",
            used, available, cpuUsageStr, cpuName, cpuArch, gpuName);

        if (leftDebugLines.empty() && rightDebugLines.empty()) {
            Logger::debug("JavaDebugMenu: No debug lines to render");
            return;
        }

        Logger::debug("JavaDebugMenu: Rendering - Left lines: {}, Right lines: {}", leftDebugLines.size(), rightDebugLines.size());

        const float uiscale = 0.5f;
        const float textsize = this->settings.getSettingByName<float>("textSize")->value;
        const float textspacing = 0.2f;
        float fontSize = Constraints::RelativeConstraint(textsize * uiscale, "height", true);
        float padding = Constraints::SpacingConstraint(0.2f, fontSize);
        float lineHeight = fontSize * textspacing;

        float posXLeft = padding;  // Default to a small padding from the left edge
        float posYLeft = padding;  // Default to a small padding from the top edge
        float posXRight = MC::windowSize.x - 300.0f - padding;
        float posYRight = posYLeft;

        D2D1_COLOR_F textColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        Logger::debug("JavaDebugMenu: Left text - X: {}, Y: {}", posXLeft, posYLeft);
        Logger::debug("JavaDebugMenu: Right text - X: {}, Y: {}", posXRight, posYRight);

        // Left column: Render Minecraft details as plain text
        float yOffsetBaseLeft = posYLeft;
        for (const auto& line : leftDebugLines) {
            std::vector<std::string> splitLines;
            size_t pos = 0;
            std::string delim = "\n";
            std::string text = line;
            while ((pos = text.find(delim)) != std::string::npos) {
                splitLines.push_back(text.substr(0, pos));
                text.erase(0, pos + delim.length());
            }
            if (!text.empty()) splitLines.push_back(text);

            for (const auto& splitLine : splitLines) {
                FlarialGUI::FlarialTextWithFont(
                    posXLeft,
                    yOffsetBaseLeft,
                    String::StrToWStr(splitLine).c_str(),
                    300.0f, // Width for wrapping (arbitrary since no background)
                    lineHeight,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    fontSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
                yOffsetBaseLeft += lineHeight;
            }
        }
        Logger::debug("JavaDebugMenu: Rendered left text with {} lines", leftDebugLines.size());

        // Right column: Render system info as plain text
        float yOffsetBaseRight = posYRight;
        for (const auto& line : rightDebugLines) {
            std::vector<std::string> splitLines;
            size_t pos = 0;
            std::string delim = "\n";
            std::string text = line;
            while ((pos = text.find(delim)) != std::string::npos) {
                splitLines.push_back(text.substr(0, pos));
                text.erase(0, pos + delim.length());
            }
            if (!text.empty()) splitLines.push_back(text);

            for (const auto& splitLine : splitLines) {
                FlarialGUI::FlarialTextWithFont(
                    posXRight,
                    yOffsetBaseRight,
                    String::StrToWStr(splitLine).c_str(),
                    300.0f, // Width for wrapping (arbitrary since no background)
                    lineHeight,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    fontSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
                yOffsetBaseRight += lineHeight;
            }
        }
        Logger::debug("JavaDebugMenu: Rendered right text with {} lines", rightDebugLines.size());
    }

    void onRender(RenderEvent& event) {
        std::string text;
        this->normalRender(20, text);
    }

    void onTick(TickEvent& event) {
        updateSpeed();
        updateTPS();
    }

    void onKey(KeyEvent& event) {
        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
            keybindActions[0]({});
        }
        if (!this->isKeybind(event.keys)) this->active = false;
    }
};