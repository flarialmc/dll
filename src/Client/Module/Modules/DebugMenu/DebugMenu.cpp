#include <numbers>
#include <cmath>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include <format>

#include <initguid.h>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <devpkey.h>

#include "DebugMenu.hpp"
#include "Tags.hpp"
#include "Modules/Time/Time.hpp"
#include "Modules/CPS/CPSCounter.hpp"

#include "SDK/Client/Block/BlockLegacy.hpp"
#include "Utils/WinrtUtils.hpp"

void JavaDebugMenu::onEnable() {
    Listen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
    Listen(this, RenderEvent, &JavaDebugMenu::onRender)
    Listen(this, TickEvent, &JavaDebugMenu::onTick)
    Listen(this, KeyEvent, &JavaDebugMenu::onKey)
    Listen(this, MouseEvent, &JavaDebugMenu::onMouse)
    Listen(this, HudCursorRendererRenderEvent, &JavaDebugMenu::onHudCursorRendererRender)
    Listen(this, SetTopScreenNameEvent, &JavaDebugMenu::onSetTopScreenName)
    Listen(this, PerspectiveEvent, &JavaDebugMenu::onGetViewPerspective)
    Module::onEnable();
}

void JavaDebugMenu::onDisable() {
    Deafen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
    Deafen(this, RenderEvent, &JavaDebugMenu::onRender)
    Deafen(this, TickEvent, &JavaDebugMenu::onTick)
    Deafen(this, KeyEvent, &JavaDebugMenu::onKey)
    Deafen(this, MouseEvent, &JavaDebugMenu::onMouse)
    Deafen(this, HudCursorRendererRenderEvent, &JavaDebugMenu::onHudCursorRendererRender)
    Deafen(this, SetTopScreenNameEvent, &JavaDebugMenu::onSetTopScreenName)
    Deafen(this, PerspectiveEvent, &JavaDebugMenu::onGetViewPerspective)
    Module::onDisable();
}

void JavaDebugMenu::defaultConfig() {
    setDef("enabled", true);

    settings.changeType<float, int>("noOfTags");
    settings.renameSetting("textColor", "textCol");
    setDef("keybind", (std::string) "F3");
    getKeybind();
    Module::defaultConfig("core");

    setDef("uiscale", 0.65f);
    setDef("hideModules", true);
    setDef("rounding", 0.0f);
    setDef("showBg", true);
    setDef("textShadow", true);
    setDef("text", (std::string) "ffffff", 1.f, false);
    setDef("textShadow", (std::string) "00000", 0.55f, false);
    setDef("bg", (std::string) "000000", 0.4f, false);
    setDef("textShadowOffset", 0.003f);

    setDef("imPoorButIWannaLookRich", false);
    setDef("f5crosshair", false);

    setDef("enableEverything", true);

    setDef("showBlock1", true); // FPS, 1% Lows, Frametime

    setDef("showBlock2", true);; // E, T, Dimension, Biome

    setDef("showBlock3", true); // Coords, Chunk, Direction, Weather
    setDef("showCoords", true);
    setDef("showWeather", true);

    setDef("showBlock4", true); // Speed, Velocity, Break Progress
    setDef("alwaysShowBreakProg", true);

    setDef("showBlock5", true); // World Time and Name

    setDef("showBlock6", true); // Server IP, Port, Ping, TPS

    setDef("showBlock7", true); // Memory

    setDef("showBlock8", true); // CPU

    setDef("showBlock9", true); // Res, Renderer, GPU

    setDef("showBlock10", true); // Local Time, CPU Uptime, Minecraft Uptime

    setDef("showBlock11", true); // Looking At
    setDef("showTargetedBlockTags", true);
    setDef("showMaxTags", true);
    setDef("noOfTags", 10);

    setDef("showFTgraph", true);
    setDef("showMinMaxFT", true);
    setDef("showThreshold", true);
    setDef("FTgraphHeight", 25.f);
    setDef("FTgraphWidth", 100.f);
    setDef("FTbarWidth", 6.f);
}

void JavaDebugMenu::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Main");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    addSlider("UI Scale", "", "uiscale", 2.0f);
    addToggle("Hide Modules", "", "hideModules");
    addSlider("Rounding", "Rounding of the rectangle", "rounding", 100, 0, false);
    addToggle("Background", "", "showBg");
    addToggle("Text Shadow", "", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    addColorPicker("Text Color", "", "text");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
    addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
    extraPadding();

    addHeader("Module Settings");
    addToggle("I'm broke but I wanna look rich :(", "only for the real broke sigmas", "imPoorButIWannaLookRich");
    addToggle("Show Debug Crosshair In F5", "", "f5crosshair");
    extraPadding();

    bool c = getOps<bool>("showFTgraph");

    addToggle("Show FrameTime Graph", "nerdy stats!", "showFTgraph");
    addConditionalToggle(c && getOps<bool>("showFTgraph"), "Show Min/Max FrameTime", "", "showMinMaxFT");
    addConditionalToggle(c && getOps<bool>("showFTgraph"), "Show 30/60FPS Lines And Text", "", "showThreshold");
    addConditionalSlider(c && getOps<bool>("showFTgraph"), "Graph Height", "", "FTgraphHeight", 50.f, 10.f);
    addConditionalSlider(c && getOps<bool>("showFTgraph"), "Graph Width", "", "FTgraphWidth", 200.f, 40.f);
    addConditionalSlider(c && getOps<bool>("showFTgraph"), "Bar Width", "", "FTbarWidth", 15.f, 1.f);
    extraPadding();

    addToggle("Enable All Text", "", "enableEverything");

    c = !getOps<bool>("enableEverything");

    if (c) extraPadding();

    addConditionalToggle(c, "Block 1", "FPS, 1% Lows, Frametime", "showBlock1");
    addConditionalToggle(c, "Block 2", "E, T, Dimension, Biome", "showBlock2");

    addConditionalToggle(c, "Block 3", "Coordinates, Weather", "showBlock3");
    addConditionalToggle(c && getOps<bool>("showBlock3"), "Show Coordinates", "", "showCoords");
    addConditionalToggle(c && getOps<bool>("showBlock3"), "Show Weather", "", "showWeather");
    if (c && getOps<bool>("showBlock3")) extraPadding();

    addConditionalToggle(c, "Block 4", "Speed, Velocity, Break Progress", "showBlock4");
    addConditionalToggle(c && getOps<bool>("showBlock4"), "Always Show Break Progress", "", "alwaysShowBreakProg");
    if (c && getOps<bool>("showBlock4")) extraPadding();

    addConditionalToggle(c, "Block 5", "World Time and Name", "showBlock5");
    addConditionalToggle(c, "Block 6", "Server Info", "showBlock6");
    addConditionalToggle(c, "Block 7", "Memory", "showBlock7");
    addConditionalToggle(c, "Block 8", "CPU", "showBlock8");
    addConditionalToggle(c, "Block 9", "Resolution, Renderer, GPU", "showBlock9");
    addConditionalToggle(c, "Block 10", "Local Time, CPU Uptime, Minecraft Uptime", "showBlock10");

    addConditionalToggle(c, "Block 11", "Looking At", "showBlock11");
    addConditionalToggle(c && getOps<bool>("showBlock11"), "Show Targeted Block Tags", "", "showTargetedBlockTags");
    addConditionalToggle(c && getOps<bool>("showBlock11") && getOps<bool>("showTargetedBlockTags"), "Show Max Tags", "", "showMaxTags");
    addConditionalSliderInt(c && getOps<bool>("showBlock11") && getOps<bool>("showTargetedBlockTags") && !getOps<bool>("showMaxTags"), "Number Of Tags To Be Show", "", "noOfTags", 20, 1);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

bool JavaDebugMenu::isOnBlock(int block) {
    return getOps<bool>("enableEverything") || getOps<bool>(std::format("showBlock{}", block));
}

bool JavaDebugMenu::isOnSetting(std::string settingName, int block = -1) {
    if (block == -1) return getOps<bool>("enableEverything") || getOps<bool>(settingName);
    else return getOps<bool>("enableEverything") || (getOps<bool>(std::format("showBlock{}", block)) && getOps<bool>(settingName));
}

void JavaDebugMenu::updateTimedVector(std::vector<TimedObj> &vec, float diff) {
    double microTime = Microtime();
    vec.erase(
        std::remove_if(vec.begin(), vec.end(), [microTime, diff](const TimedObj &obj) {
            return (microTime - obj.timestamp) > diff;
        }), vec.end()
    );
}

void JavaDebugMenu::getOnePercLows() {
    if (prevFrameTimes.empty()) {
        cached1PercLow = 0.f;
        return;
    }
    std::vector<float> sorted(prevFrameTimes.begin(), prevFrameTimes.end());
    std::sort(sorted.begin(), sorted.end(), std::greater<float>());

    size_t count = std::max<size_t>(1, sorted.size() / 100);

    std::vector<float> worst1Percent(sorted.begin(), sorted.begin() + count);
    std::vector<float> fpsValues;
    for (float ms: worst1Percent) fpsValues.push_back(1000.0f / ms);

    cached1PercLow = std::accumulate(fpsValues.begin(), fpsValues.end(), 0.0f) / fpsValues.size();
}

std::string JavaDebugMenu::getFacingDirection(LocalPlayer *player) {
    std::string direction;
    if (lerpYaw >= -45 && lerpYaw < 45) direction = "South (Towards positive Z)";
    else if (lerpYaw >= 45 && lerpYaw < 135) direction = "West (Towards negative X)";
    else if (lerpYaw >= 135 || lerpYaw < -135) direction = "North (Towards negative Z)";
    else direction = "East (Towards positive X)";
    return std::format("Facing: {} ({:.1f} / {:.1f})", direction, lerpYaw, lerpPitch);
}

std::wstring JavaDebugMenu::GetCpuName() {
    HDEVINFO handle = SetupDiGetClassDevsW(&GUID_DEVCLASS_PROCESSOR, NULL, NULL, DIGCF_PRESENT);
    SP_DEVINFO_DATA data = {.cbSize = sizeof(SP_DEVINFO_DATA)};

    std::vector<byte> buffer{};

    SetupDiEnumDeviceInfo(handle, 0, &data);

    DWORD length{};
    DEVPROPTYPE type{};
    SetupDiGetDevicePropertyW(handle, &data, &DEVPKEY_Device_FriendlyName, &type, NULL, 0, &length, 0);

    buffer.resize(length);
    SetupDiGetDevicePropertyW(handle, &data, &DEVPKEY_Device_FriendlyName, &type, buffer.data(), length, NULL, 0);

    SetupDiDestroyDeviceInfoList(handle);

    return std::wstring(reinterpret_cast<PWSTR>(buffer.data()));
}

DWORD JavaDebugMenu::GetCpuCoreCount() {
    DWORD count{}, length{};
    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer{};

    GetLogicalProcessorInformation(nullptr, &length);
    buffer.resize(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

    GetLogicalProcessorInformation(buffer.data(), &length);

    return std::count_if(buffer.begin(), buffer.end(),
                         [](const SYSTEM_LOGICAL_PROCESSOR_INFORMATION &info) { return !info.Relationship; });
}

DWORD JavaDebugMenu::GetCpuThreadCount() {
    SYSTEM_INFO info{};
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

std::string JavaDebugMenu::getDimensionName() {
    BlockSource *blocksrc = SDK::clientInstance->getBlockSource();
    if (!blocksrc) return "Unknown dimension";

    Dimension *dimension = blocksrc->getDimension();
    if (!dimension) return "Unknown dimension";

    std::string dim = dimension->getName();

    if (dim == "Overworld") return "minecraft:overworld";
    else if (dim == "Nether") return "minecraft:nether";
    else if (dim == "TheEnd") return "minecraft:the_end";
    else return "Unknown dimension";
}

std::pair<std::string, std::vector<float> > JavaDebugMenu::getWeatherInfo() {
    std::string weatherStr;
    std::vector<float> weatherVals;

    std::pair<std::string, std::vector<float> > res = {weatherStr, weatherVals};

    BlockSource *blocksrc = SDK::clientInstance->getBlockSource();
    if (!blocksrc) return res;

    Dimension *dimension = blocksrc->getDimension();
    if (!dimension) return res;

    Weather *weather = dimension->getweather();
    if (!weather) return res;

    float rain = weather->getrainLevel();
    float lightning = weather->getlightningLevel();

    if (lightning == 1) weatherStr = "Thunderstorm";
    else if (lightning > 0) weatherStr = "Light Thunder";
    else if (rain == 1) weatherStr = "Heavy Rain";
    else if (rain > 0.5) weatherStr = "Rain";
    else if (rain > 0) weatherStr = "Light Rain";
    else weatherStr = "Clear";

    return std::pair<std::string, std::vector<float> >{weatherStr, std::vector<float>{rain, lightning}};
}

std::string JavaDebugMenu::getTime() {
    const auto now = std::time(nullptr);
    const std::tm calendarTime = localtime_xp(now);

    std::string meridiem;

    int hour = calendarTime.tm_hour;
    int minute = calendarTime.tm_min;

    if (hour - 12 < 0) meridiem = "AM";
    else if (hour == 0) hour = 12, meridiem = "AM";
    else if (hour == 12) hour = 12, meridiem = "PM";
    else meridiem = "PM", hour -= 12;

    std::string seperator = minute < 10 ? ":0" : ":";

    if (hour == 24) hour = 0;

    return FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;
}

std::string JavaDebugMenu::getFormattedTime(long long seconds) {
    std::string text;

    int days = seconds / 86400;
    int hours = (seconds % 86400) / 3600;
    int mins = (seconds % 3600) / 60;
    int secs = seconds % 60;

    if (days >= 1) text += std::format("{} days, ", days);
    if (hours >= 1) text += std::format("{} hours, ", hours);
    if (mins >= 1) text += std::format("{} mins, ", mins);
    text += std::format("{} secs", secs);

    return text;
}

void JavaDebugMenu::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer()) return;

    if (isOnBlock(4)) {
        // Speed and Velocity
        auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
        if (stateVectorComponent != nullptr) {
            xVelo = (stateVectorComponent->Pos.x - PrevPos.x) * 20;
            yVelo = (stateVectorComponent->Pos.y - PrevPos.y) * 20;
            zVelo = (stateVectorComponent->Pos.z - PrevPos.z) * 20;
            PrevPos = stateVectorComponent->Pos;
        }
    }

    if (isOnBlock(6)) {
        // TPS
        TimedObj tick{};
        tick.timestamp = Microtime();
        tickList.insert(tickList.begin(), tick);
    }

    if (isOnBlock(2)) {
        // Biome
        Vec3<float> pos = *SDK::clientInstance->getLocalPlayer()->getPosition();
        BlockPos bp{
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            static_cast<int>(pos.z),
        };
        if (SDK::clientInstance->getBlockSource() && SDK::clientInstance->getBlockSource()->getBiome(bp)) {
            curBiome = SDK::clientInstance->getBlockSource()->getBiome(bp);
        }
    }
}

void JavaDebugMenu::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
    if (!SDK::clientInstance->getBlockSource()) return;

    if (isOnBlock(11)) {
        // Looking At
        HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();
        BlockPos pos = {
            result.blockPos.x,
            result.blockPos.y,
            result.blockPos.z
        };
        BlockSource *blockSource = SDK::clientInstance->getBlockSource();
        try {
            BlockLegacy *block = blockSource->getBlock(pos)->getBlockLegacy();
            if (!block) return;
            try {
                lookingAt = block->getNamespace() + ":" + block->getName();

                if (isOnSetting("showTargetedBlockTags") && lookingAt != lastLookingAt) {
                    lastLookingAt = lookingAt;
                    std::vector<std::string> tags = {};
                    for (auto i: tagMap) {
                        if (std::ranges::find(i.second, block->getName()) != i.second.end()) {
                            tags.emplace_back(i.first);
                        }
                    }
                    lookingAtTags = tags;
                }
            } catch (const std::exception &e) { LOG_ERROR("Failed to get block name: {}", e.what()); }
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to get block: {}", e.what());
        }
    }

    if (isOnBlock(4) && MC::heldLeft) {
        // Break Progress
        Gamemode *gm = SDK::clientInstance->getLocalPlayer()->getGamemode();
        float breakProgress = gm->getLastBreakProgress() * 100;
        if (lastBreakProgress != breakProgress) {
            if (lastBreakProgress < breakProgress || breakProgress == 0.f) {
                currentBreakProgress = breakProgress;
            }
            lastBreakProgress = breakProgress;
        }
    } else {
        currentBreakProgress = 0.0f;
    }
}

void JavaDebugMenu::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (this->active && (SDK::getCurrentScreen() == "f3_screen" || SDK::getCurrentScreen() == "hud_screen")) {
        float uiscale = getOps<float>("uiscale");
        float uiscaleConst = Constraints::RelativeConstraint(0.001f * uiscale);
        float textHeight = Constraints::RelativeConstraint(0.1f * uiscale);
        float textSize = Constraints::SpacingConstraint(2.0f, textHeight);
        float yPadding = Constraints::SpacingConstraint(0.025f, textHeight);
        float rounding = getOps<float>("rounding");

        D2D1_COLOR_F textColor = getColor("text");
        D2D1_COLOR_F textShadowColor = getColor("textShadow");
        D2D1_COLOR_F bgColor = getColor("bg");

        float textShadowOffset = Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");

        if (SDK::clientInstance == nullptr) return;
        LocalPlayer *player = SDK::clientInstance->getLocalPlayer();
        if (!player) return;
        ActorRotationComponent *rotComponent = player->getActorRotationComponent();

        if (rotComponent) {
            if (rotComponent->rot.y != 0) lerpYaw = rotComponent->rot.y;
            if (rotComponent->rot.x != 0) lerpPitch = rotComponent->rot.x;
        }

        std::vector<std::string> left;
        std::vector<std::string> right;

        bool spoof = getOps<bool>("imPoorButIWannaLookRich");

        if (versionName.empty()) versionName = std::format("Flarial V2 Open Beta, Minecraft {}", WinrtUtils::getFormattedVersion());
        left.emplace_back(versionName);

        if (isOnBlock(1)) {
            if (spoof) {
                left.emplace_back(std::format("{} FPS", static_cast<int>(MC::fps * 222.2)));
                left.emplace_back("1% Lows: \u221E FPS");
            } else {
                left.emplace_back(std::format("{} FPS", MC::fps));
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - last1PercLowUpdate).count() >= 1) {
                    getOnePercLows();
                    last1PercLowUpdate = now;
                }
                left.emplace_back(std::format("1% Lows: {:.0f} FPS", cached1PercLow));
            }
            left.emplace_back(std::format("Frametime: {:.2f}ms", MC::frameTime));
        }

        left.emplace_back("");

        if (isOnBlock(2)) {
            std::string temp = "Unknown";
            if (curBiome != nullptr) temp = std::format("{:.2f}", curBiome->gettemperature());
            left.emplace_back(std::format("E: {} T: {}", player->getLevel()->getRuntimeActorList().size(), temp));

            left.emplace_back(getDimensionName());

            if (curBiome != nullptr) left.emplace_back(std::format("minecraft:{}", curBiome->getName()));
            else left.emplace_back("Unknown biome");

            left.emplace_back("");
        }

        if (isOnBlock(3)) {
            if (isOnSetting("showCoords")) {
                Vec3<float> pos = *player->getPosition();
                left.emplace_back(std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", pos.x, pos.y, pos.z));

                Vec3<int> blockPos(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z));
                left.emplace_back(std::format("Block: {} {} {}", blockPos.x, blockPos.y, blockPos.z));
                left.emplace_back(std::format("Chunk: {} {} {}", static_cast<int>(pos.x / 16), static_cast<int>(pos.y / 16), static_cast<int>(pos.z / 16)));
                left.emplace_back(std::format("Chunk Coordinate: {} {}", static_cast<int>(pos.x) % 16, static_cast<int>(pos.z) % 16));

                left.emplace_back(getFacingDirection(player));
                HitResult target = player->getLevel()->getHitResult();
                BlockPos targetPos = {target.blockPos.x, target.blockPos.y, target.blockPos.z};
                left.emplace_back(std::format("Looking at block: {} {} {}", targetPos.x, targetPos.y, targetPos.z));
            }

            if (isOnSetting("showWeather")) {
                std::pair<std::string, std::vector<float> > weatherInfo = getWeatherInfo();
                if (weatherInfo.second.empty()) {
                    left.emplace_back("Weather: Unknown");
                } else {
                    left.emplace_back(std::format("Weather: {}", weatherInfo.first));
                    left.emplace_back(std::format("Rain / Lightning: {:.0f}% / {:.0f}%", weatherInfo.second[0] * 100, weatherInfo.second[1] * 100));
                }
            }

            left.emplace_back("");
        }

        if (isOnBlock(4)) {
            left.emplace_back(std::format("Speed: {:.2f} blocks/s", sqrt(std::pow(xVelo, 2) + std::pow(yVelo, 2) + std::pow(zVelo, 2))));
            left.emplace_back(std::format("Velocity: {:.2f} / {:.2f} / {:.2f} blocks/s", xVelo, yVelo, zVelo));

            if (isOnSetting("alwaysShowBreakProg") || currentBreakProgress != 0.0f) left.emplace_back(std::format("Break Progress: {}%", static_cast<int>(currentBreakProgress)));

            left.emplace_back("");
        }

        if (isOnBlock(5)) {
            left.emplace_back(std::format("World Time: {}", Time::formatMCTime(Time::curTime, false)));
            left.emplace_back(std::format("World Name: {}", SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName()));

            left.emplace_back("");
        }

        if (isOnBlock(6)) {
            left.emplace_back("Server:");
            std::string serverIp = SDK::getServerIP();

            if (serverIp == "world") {
                left.emplace_back("IP: local");
            } else {
                left.emplace_back(std::format("IP: {}", SDK::getServerIP()));
                left.emplace_back(std::format("Port: {}", SDK::getServerPort()));
                left.emplace_back(std::format("Ping: {} ms", SDK::getServerPing()));
            }

            updateTimedVector(tickList, 1.0f);
            left.emplace_back(std::format("TPS: {}", std::to_string(tickList.size())));
        }


        right.emplace_back("64bit");

        if (isOnBlock(7)) {
            MEMORYSTATUSEX memory_status;
            memory_status.dwLength = sizeof(memory_status);
            GlobalMemoryStatusEx(&memory_status);
            int total_memory = static_cast<int>(memory_status.ullTotalPhys / 1000000);
            int free_memory = static_cast<int>(memory_status.ullAvailPhys / 1000000);
            int used_memory = total_memory - free_memory;
            if (spoof) {
                int totallyRealTotalMemory = 2097152;
                right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / totallyRealTotalMemory), used_memory, totallyRealTotalMemory));
            } else right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / total_memory), used_memory, total_memory));
        }

        right.emplace_back("");

        if (isOnBlock(8)) {
            if (spoof) right.emplace_back("CPU: 69x Intel 9 7900X3D ProMax Plus (420 Cores)");
            else {
                if (cpuName.empty()) {
                    std::wstring temp(std::format(L"CPU: {}x {} ({} Cores)", GetCpuThreadCount(), GetCpuName(), GetCpuCoreCount()));
                    cpuName = std::string(temp.begin(), temp.end());
                }
                right.emplace_back(cpuName);
            }

            right.emplace_back("");
        }

        if (isOnBlock(9)) {
            right.emplace_back(std::format("Display: {}x{}", MC::windowSize.x, MC::windowSize.y));
            right.emplace_back(std::format("Active Renderer: {}", SwapchainHook::queue == nullptr ? "DirectX11" : "DirectX12"));

            if (spoof) right.emplace_back("AMD GFX 5090 Ti AI Accelerated DLSS 12.0");
            else {
                if (!MC::GPU.empty()) right.emplace_back(MC::GPU);
                else right.emplace_back("Unknown GPU");
            }

            right.emplace_back("");
        }

        if (isOnBlock(10)) {
            right.emplace_back(std::format("Local Time: {}", getTime()));
            right.emplace_back(std::format("CPU Uptime: {}", getFormattedTime(static_cast<long long>(GetTickCount64() / 1000))));
            right.emplace_back(std::format("Minecraft Uptime: {}", getFormattedTime(
                                               static_cast<long long>(
                                                   std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()
                                               ) / 1000
                                           )));

            right.emplace_back("");
        }

        if (isOnBlock(11)) {
            right.emplace_back("Targeted Block:");
            right.emplace_back(lookingAt);
            right.emplace_back("");

            if (isOnSetting("showTargetedBlockTags")) {
                bool showMax = getOps<bool>("showMaxTags");
                int maxAllowedTags = getOps<int>("noOfTags");
                int maxFittableTags = static_cast<int>(MC::windowSize.y / (textHeight / 3.0f + yPadding * 2)) - right.size();

                if (lookingAtTags.size() >= maxFittableTags && (showMax || maxAllowedTags >= maxFittableTags)) {
                    for (int i = 0; i < maxFittableTags; i++) right.emplace_back('#' + lookingAtTags[i]);
                    right.emplace_back(std::format("{} more tags...", lookingAtTags.size() - maxFittableTags));
                } else if (showMax) {
                    for (const auto &i: lookingAtTags) right.emplace_back('#' + i);
                } else if (lookingAtTags.size() > maxAllowedTags) {
                    for (int i = 0; i < maxAllowedTags; i++) right.emplace_back('#' + lookingAtTags[i]);
                    right.emplace_back(std::format("{} more tags...", lookingAtTags.size() - maxAllowedTags));
                } else {
                    for (int i = 0; i < lookingAtTags.size(); i++) right.emplace_back('#' + lookingAtTags[i]);
                }
            }
        }


        int leftYoffset = 0.0f;
        for (const auto &i: left) {
            if (getOps<bool>("showBg") && !i.empty()) {
                float lineWidth = FlarialGUI::getFlarialTextSize(
                    String::StrToWStr(i).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    true
                ).x;
                FlarialGUI::RoundedRect(
                    0.0f, leftYoffset - yPadding + 0.05f,
                    bgColor,
                    lineWidth, textHeight / 3.0f + yPadding * 2 - 1.0f,
                    rounding, rounding
                );
            }

            if (getOps<bool>("textShadow"))
                FlarialGUI::FlarialTextWithFont(
                    textShadowOffset, leftYoffset + textShadowOffset,
                    String::StrToWStr(i).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textShadowColor,
                    true
                );

            FlarialGUI::FlarialTextWithFont(
                0.0f, leftYoffset,
                String::StrToWStr(i).c_str(),
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
        for (const auto &i: right) {
            if (getOps<bool>("showBg") && !i.empty()) {
                float lineWidth = FlarialGUI::getFlarialTextSize(
                    String::StrToWStr(i).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    true
                ).x;
                FlarialGUI::RoundedRect(
                    MC::windowSize.x - lineWidth, rightYoffset - yPadding + 0.05f,
                    bgColor,
                    lineWidth, textHeight / 3.0f + yPadding * 2 - 1.0f,
                    rounding, rounding
                );
            }

            if (getOps<bool>("textShadow"))
                FlarialGUI::FlarialTextWithFont(
                    (MC::windowSize.x - 30.0f) + textShadowOffset, rightYoffset + textShadowOffset,
                    String::StrToWStr(i).c_str(),
                    30.0f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textShadowColor,
                    true
                );

            FlarialGUI::FlarialTextWithFont(
                MC::windowSize.x - 30.0f, rightYoffset,
                String::StrToWStr(i).c_str(),
                30.0f, textHeight / 3.0f,
                DWRITE_TEXT_ALIGNMENT_TRAILING,
                textSize,
                DWRITE_FONT_WEIGHT_NORMAL,
                textColor,
                true
            );
            rightYoffset += textHeight / 3.0f + yPadding * 2;
        }

        // other cool stuff

        ImDrawList *drawList = ImGui::GetBackgroundDrawList();

        // frametime graph start

        if (isOnSetting("showFTgraph") || isOnBlock(1)) {
            while (prevFrameTimes.size() >= static_cast<int>(Constraints::SpacingConstraint(getOps<float>("FTgraphWidth") * 10, uiscaleConst) / Constraints::SpacingConstraint(getOps<float>("FTbarWidth"), uiscaleConst))) prevFrameTimes.pop_front();
            prevFrameTimes.push_back(MC::frameTime);
        }

        if (isOnSetting("showFTgraph")) {
            float max = 1000.f / 30;
            float maxRectHeight = Constraints::SpacingConstraint(getOps<float>("FTgraphHeight") * 10, uiscaleConst);
            float maxRectWidth = Constraints::SpacingConstraint(getOps<float>("FTgraphWidth") * 10, uiscaleConst);
            float startHeight = MC::windowSize.y - maxRectHeight;
            float barWidth = Constraints::SpacingConstraint(getOps<float>("FTbarWidth"), uiscaleConst);
            float borderSize = Constraints::SpacingConstraint(4.f, uiscaleConst);
            float startX = borderSize;

            int graphLen = static_cast<int>(maxRectWidth / barWidth);

            FlarialGUI::RoundedRect(
                borderSize, startHeight,
                D2D1_COLOR_F(0.5, 0.5, 0.5, 0.2),
                barWidth * graphLen - borderSize, maxRectHeight - borderSize,
                0, 0
            );

            for (float ft: prevFrameTimes) {
                D2D1_COLOR_F barCol;
                if (ft / max >= 1) barCol = D2D1_COLOR_F(1, 0, 0, 1);
                else barCol = FlarialGUI::HSVtoColorF(120.f * (1 - ft / max), 1.f, 1.f);
                FlarialGUI::RoundedRect(
                    startX, startHeight + maxRectHeight * (1 - ft / max) - borderSize,
                    barCol,
                    barWidth, maxRectHeight * (ft / max),
                    0, 0
                );
                startX += barWidth;
            }

            FlarialGUI::RoundedHollowRect(
                borderSize, startHeight,
                borderSize,
                D2D1_COLOR_F(1, 1, 1, 1),
                barWidth * graphLen - borderSize, maxRectHeight - borderSize,
                0, 0
            );

            float minFT = *std::ranges::min_element(prevFrameTimes);
            float maxFT = *std::ranges::max_element(prevFrameTimes);
            float avgFT = std::accumulate(prevFrameTimes.begin(), prevFrameTimes.end(), 0.f) / prevFrameTimes.size();

            if (getOps<bool>("showMinMaxFT")) {
                // minimum frame time

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        borderSize * 2 + textShadowOffset, (startHeight - textHeight / 3.0f) + textShadowOffset,
                        String::StrToWStr(std::format("{:.1f} ms min", minFT)).c_str(),
                        0.f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_LEADING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        textShadowColor,
                        true
                    );

                FlarialGUI::FlarialTextWithFont(
                    borderSize * 2, startHeight - textHeight / 3.0f,
                    String::StrToWStr(std::format("{:.1f} ms min", minFT)).c_str(),
                    0.f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );

                // average frame time

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        (barWidth * graphLen / 2) + textShadowOffset, (startHeight - textHeight / 3.0f) + textShadowOffset,
                        String::StrToWStr(std::format("{:.1f} ms avg", avgFT)).c_str(),
                        0.f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_CENTER,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        textShadowColor,
                        true
                    );

                FlarialGUI::FlarialTextWithFont(
                    barWidth * graphLen / 2, startHeight - textHeight / 3.0f,
                    String::StrToWStr(std::format("{:.1f} ms avg", avgFT)).c_str(),
                    0.f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_CENTER,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );

                // max frame time

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        (barWidth * graphLen) + textShadowOffset, (startHeight - textHeight / 3.0f) + textShadowOffset,
                        String::StrToWStr(std::format("{:.1f} ms max", maxFT)).c_str(),
                        0.f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_TRAILING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        textShadowColor,
                        true
                    );

                FlarialGUI::FlarialTextWithFont(
                    barWidth * graphLen, startHeight - textHeight / 3.0f,
                    String::StrToWStr(std::format("{:.1f} ms max", maxFT)).c_str(),
                    0.f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
            }

            if (getOps<bool>("showThreshold")) {
                drawList->AddLine(
                    ImVec2(borderSize, startHeight + maxRectHeight / 2),
                    ImVec2(barWidth * graphLen, startHeight + maxRectHeight / 2 + borderSize),
                    IM_COL32(255, 255, 255, 255),
                    borderSize
                );

                // 30 fps text

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        (borderSize * 2) + textShadowOffset, startHeight + textShadowOffset,
                        String::StrToWStr(std::format("30 FPS", minFT)).c_str(),
                        0.f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_LEADING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        textShadowColor,
                        true
                    );

                FlarialGUI::FlarialTextWithFont(
                    borderSize * 2, startHeight,
                    String::StrToWStr(std::format("30 FPS", minFT)).c_str(),
                    0.f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        (borderSize * 2) + textShadowOffset, (startHeight + maxRectHeight / 2 + borderSize) + textShadowOffset,
                        String::StrToWStr(std::format("60 FPS", minFT)).c_str(),
                        0.f, textHeight / 3.0f,
                        DWRITE_TEXT_ALIGNMENT_LEADING,
                        textSize,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        textShadowColor,
                        true
                    );

                FlarialGUI::FlarialTextWithFont(
                    borderSize * 2, startHeight + maxRectHeight / 2 + borderSize,
                    String::StrToWStr(std::format("60 FPS", minFT)).c_str(),
                    0.f, textHeight / 3.0f,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    textSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor,
                    true
                );
            }
        }

        // frametime graph end

        if (ModuleManager::getModule("ClickGUI")->active || (!getOps<bool>("f5crosshair") && curPerspective != Perspective::FirstPerson)) return;

        // debug menu crosshair start

        float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

        float lineWidth = 2.5f;
        float lineLength = guiscale * 8.f;

        ImVec2 center = ImVec2(MC::windowSize.x / 2.0f, MC::windowSize.y / 2.0f);

        float yawRad = (180.f + lerpYaw) * (std::numbers::pi / 180.f);
        float pitchRad = (-lerpPitch) * (std::numbers::pi / 180.f);

        ImU32 red = IM_COL32(255, 0, 0, 255);
        ImU32 green = IM_COL32(0, 255, 0, 255);
        ImU32 blue = IM_COL32(0, 0, 255, 255);

        ImVec2 redPos(
            center.x + lineLength * cos(yawRad),
            center.y + lineLength * sin(yawRad) * sin(pitchRad)
        );

        ImVec2 greenPos(center.x, center.y - lineLength * cos(pitchRad));

        ImVec2 bluePos(
            center.x + lineLength * sin(yawRad),
            center.y - lineLength * cos(yawRad) * sin(pitchRad)
        );

        // red line (-yaw)
        if (lerpYaw < 0.0f) drawVector(drawList, center, redPos, red, lineWidth, lineLength, guiscale);

        // blue line (< 90abs yaw)
        if (abs(lerpYaw) < 90.f) drawVector(drawList, center, bluePos, blue, lineWidth, lineLength, guiscale);

        // green line
        drawVector(drawList, center, greenPos, green, lineWidth, lineLength, guiscale);

        // red line (+yaw)
        if (lerpYaw > 0.0f) drawVector(drawList, center, redPos, red, lineWidth, lineLength, guiscale);

        // blue line (> 90abs yaw)
        if (abs(lerpYaw) > 90.f) drawVector(drawList, center, bluePos, blue, lineWidth, lineLength, guiscale);

        // debug menu crosshair end
    }
}

void JavaDebugMenu::drawVector(ImDrawList *drawList, ImVec2 center, ImVec2 endPos, ImU32 col, float lineWidth, float lineLength, float guiscale) {
    drawList->AddLine(center, endPos, IM_COL32(0, 0, 0, 255), lineWidth + (guiscale * 0.3));
    drawList->AddLine(center, endPos, col, lineWidth);
}

void JavaDebugMenu::onHudCursorRendererRender(HudCursorRendererRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (this->active && SDK::clientInstance && (
            SDK::getCurrentScreen() == "hud_screen" ||
            SDK::getCurrentScreen() == "f3_screen" ||
            SDK::getCurrentScreen() == "zoom_screen"
        ) && SDK::clientInstance->getLocalPlayer() != nullptr) {
        event.cancel();
    }
}

void JavaDebugMenu::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeyPartOfKeybind(event.key)) {
        if (this->isKeybind(event.keys)) {
            keybindActions[0]({});
        }
    }
}

void JavaDebugMenu::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) {
        keybindActions[0]({});
    }
}

void JavaDebugMenu::onSetTopScreenName(SetTopScreenNameEvent &event) {
    if (!this->isEnabled()) return;
    if (this->active && getOps<bool>("hideModules") && event.getLayer() == "hud_screen") event.setCustomLayer("f3_screen");
}

void JavaDebugMenu::onGetViewPerspective(PerspectiveEvent &event) {
    if (!this->active || !this->isEnabled() || !SDK::clientInstance->getLocalPlayer()) return;
    curPerspective = event.getPerspective();
}
