#include <numbers>
#include <cmath>
#include <algorithm>
#include <deque>
#include "DebugMenu.hpp"
#include "Tags.hpp"
#include "Modules/Time/Time.hpp"
#include "SDK/Client/Block/BlockLegacy.hpp"
#include "Modules/CPS/CPSCounter.hpp"
#include "Utils/WinrtUtils.hpp"

void JavaDebugMenu::onEnable() {
    Listen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
    Listen(this, RenderEvent, &JavaDebugMenu::onRender)
    Listen(this, TickEvent, &JavaDebugMenu::onTick)
    Listen(this, KeyEvent, &JavaDebugMenu::onKey)
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
    Deafen(this, HudCursorRendererRenderEvent, &JavaDebugMenu::onHudCursorRendererRender)
    Deafen(this, SetTopScreenNameEvent, &JavaDebugMenu::onSetTopScreenName)
    Deafen(this, PerspectiveEvent, &JavaDebugMenu::onGetViewPerspective)
    Module::onDisable();
}

void JavaDebugMenu::defaultConfig() {
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
    setDef("textShadow", (std::string)"00000", 0.55f, false);
    setDef("bg", (std::string) "000000", 0.5f, false);
    setDef("textShadowOffset", 0.003f);

    setDef("imPoorButIWannaLookRich", false);
    setDef("f5crosshair", false);

    setDef("showFTgraph", true);
    setDef("showMinMaxFT", true);
    setDef("showThreshold", true);
    setDef("FTgraphHeight", 25.f);
    setDef("FTgraphWidth", 100.f);
    setDef("FTbarWidth", 6.f);

    setDef("enableEverything", true);
    setDef("showFPS", true);
    setDef("showOnePercLows", true);
    setDef("showDim", true);
    setDef("showBiome", true);
    setDef("showWeather", true);
    setDef("showCoords", true);
    setDef("showSpeed", true);
    setDef("showVelocity", true);
    setDef("showBreakProg", true);
    setDef("alwaysShowBreakProg", true);
    setDef("showServer", true);
    setDef("showMemory", true);
    setDef("showCpuGpu", true);
    setDef("showTime", true);
    setDef("showInGameTime", true);
    setDef("showWorldName", true);
    setDef("showUptime", true);
    setDef("showTargetedBlock", true);
    setDef("showTargetedBlockTags", true);
    setDef("showMaxTags", true);
    setDef("noOfTags", 10);
}

void JavaDebugMenu::customToggle(std::string text, std::string subtext, std::string settingName) {
    addConditionalToggle(!getOps<bool>("enableEverything"), text, subtext, settingName);
}

void JavaDebugMenu::customConditionalToggle(bool condition, std::string text, std::string subtext, std::string settingName) {
    addConditionalToggle(!getOps<bool>("enableEverything") && condition, text, subtext, settingName);
}

void JavaDebugMenu::customConditionalSlider(bool condition, std::string text, std::string subtext, std::string settingName, float maxVal, float minVal, bool zerosafe) {
    addConditionalSlider(!getOps<bool>("enableEverything") && condition, text, subtext, settingName, maxVal, minVal, zerosafe);
}

void JavaDebugMenu::customConditionalSliderInt(bool condition, std::string text, std::string subtext, std::string settingName, int maxVal, int minVal) {
    addConditionalSliderInt(!getOps<bool>("enableEverything") && condition, text, subtext, settingName, maxVal, minVal);
}

void JavaDebugMenu::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

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

    addToggle("Show FrameTime Graph", "nerdy stats!", "showFTgraph");
    addConditionalToggle(getOps<bool>("showFTgraph"), "Show Min/Max FrameTime", "", "showMinMaxFT");
    addConditionalToggle(getOps<bool>("showFTgraph"), "Show 30/60FPS Lines And Text", "", "showThreshold");
    addConditionalSlider(getOps<bool>("showFTgraph"), "Graph Height", "", "FTgraphHeight", 50.f, 10.f);
    addConditionalSlider(getOps<bool>("showFTgraph"), "Graph Width", "", "FTgraphWidth", 200.f, 40.f);
    addConditionalSlider(getOps<bool>("showFTgraph"), "Bar Width", "", "FTbarWidth", 15.f, 1.f);
    extraPadding();

    addToggle("Enable All Text", "", "enableEverything");

    if (!getOps<bool>("enableEverything")) {
        extraPadding();
    }

    customToggle("Show FPS and Frametime", "", "showFPS");
    customToggle("Show 1% Low FPS", "", "showOnePercLows");
    customToggle("Show Entity Counter and Dimension", "", "showDim");
    customToggle("Show Biome", "", "showBiome");
    customToggle("Show Weather", "", "showWeather");
    customToggle("Show Coordinates Section", "", "showCoords");
    customToggle("Show Speed", "", "showSpeed");
    customToggle("Show Velocity", "", "showVelocity");
    customToggle("Show Break Progress", "", "showBreakProg");
    customConditionalToggle(getOps<bool>("showBreakProg"), "Always Show Break Progress", "", "alwaysShowBreakProg");
    customToggle("Show Server Section", "", "showServer");
    customToggle("Show Memory", "", "showMemory");
    customToggle("Show CPU, GPU and Renderer", "", "showCpuGpu");
    customToggle("Show Local Time", "", "showTime");
    customToggle("Show Ingame Time", "", "showInGameTime");
    customToggle("Show World Name", "", "showWorldName");
    customToggle("Show Uptime", "", "showUptime");
    customToggle("Show Targeted Block", "", "showTargetedBlock");
    customConditionalToggle(getOps<bool>("showTargetedBlock"), "Show Targeted Block Tags", "", "showTargetedBlockTags");
    customConditionalToggle(getOps<bool>("showTargetedBlock") && getOps<bool>("showTargetedBlockTags"), "Show Maximum Number Of Block Tags", "", "showMaxTags");
    customConditionalSliderInt(getOps<bool>("showTargetedBlock") && getOps<bool>("showTargetedBlockTags") && !getOps<bool>("showMaxTags"), "Number Of Tags To Be Show", "", "noOfTags", 20, 1);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

bool JavaDebugMenu::isOn(std::string settingName) {
    return getOps<bool>("enableEverything") || getOps<bool>(settingName);
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
    if (!prevFrameTimes.size()) {
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

std::string JavaDebugMenu::getCPU() {
    // AI Slop
    int cpuInfo[4] = {-1};
    char cpuBrand[0x40] = {0};
    __cpuid(cpuInfo, 0x80000002);
    memcpy(cpuBrand, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000003);
    memcpy(cpuBrand + 16, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000004);
    memcpy(cpuBrand + 32, cpuInfo, sizeof(cpuInfo));

    std::string cpuBrandStr = std::string(cpuBrand);

    // more ai slop
    int threadCount = 0;
	__cpuid(cpuInfo, 0);
	unsigned int maxLeaf = cpuInfo[0];

    if (maxLeaf >= 1) {
		__cpuid(cpuInfo, 1);
		threadCount = (cpuInfo[1] >> 16) & 0xFF; 
    } else {
        threadCount = 1; // Default to 1 if unable to determine
	}

    return std::to_string(threadCount) + "x " + cpuBrandStr;
}

std::string JavaDebugMenu::getDimensionName() {
    BlockSource *blocksrc = SDK::clientInstance->getBlockSource();
    if (!blocksrc) return "Unknown dimension";

    Dimension* dimension = blocksrc->getDimension();
    if (!dimension) return "Unknown dimension";

    std::string dim = dimension->getName();

    if (dim == "Overworld") return "minecraft:overworld";
    else if (dim == "Nether") return "minecraft:nether";
    else if (dim == "TheEnd") return "minecraft:the_end";
    else return "Unknown dimension";
}

std::vector<float> JavaDebugMenu::getWeatherInfo() {
    BlockSource *blocksrc = SDK::clientInstance->getBlockSource();
    if (!blocksrc) return std::vector<float>{};

    Dimension* dimension = blocksrc->getDimension();
    if (!dimension) return std::vector<float>{};

    Weather* weather = dimension->getweather();
    if (!weather) return std::vector<float>{};

    return std::vector<float>{weather->getrainLevel(), weather->getlightningLevel()};
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

    if (isOn("showSpeed") || isOn("showVelocity")) {
        auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
        if (stateVectorComponent != nullptr) {
            xVelo = (stateVectorComponent->Pos.x - PrevPos.x) * 20;
            yVelo = (stateVectorComponent->Pos.y - PrevPos.y) * 20;
            zVelo = (stateVectorComponent->Pos.z - PrevPos.z) * 20;
            PrevPos = stateVectorComponent->Pos;
        }
    }

    if (isOn("showServer")) {
        TimedObj tick{};
        tick.timestamp = Microtime();
        tickList.insert(tickList.begin(), tick);
    }

    if (isOn("showBiome")) {
        Vec3<float> pos = *SDK::clientInstance->getLocalPlayer()->getPosition();
        BlockPos bp {
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            static_cast<int>(pos.z),
        };
        if (SDK::clientInstance->getBlockSource() && SDK::clientInstance->getBlockSource()->getBiome(bp)) {
            curBiome = SDK::clientInstance->getBlockSource()->getBiome(bp)->getName();
        }
    }
}

void JavaDebugMenu::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
    if (!SDK::clientInstance->getBlockSource()) return;

    if (isOn("showTargetedBlock")) {
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

                if (isOn("showTargetedBlockTags") && lookingAt != lastLookingAt) {
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

    if (isOn("showBreakProg") && MC::heldLeft) {
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

        if (versionName.empty()) versionName = std::format("Flarial V2 Open Beta, Minecraft {}", WinrtUtils::getFormattedVersion());
        left.emplace_back(versionName);

        if (isOn("showFPS")) {
            if (getOps<bool>("imPoorButIWannaLookRich")) {
                left.emplace_back(std::format("{} FPS", static_cast<int>(MC::fps * 222.2)));
                if (isOn("showOnePercLows")) left.emplace_back("1% Lows: \u221E FPS");
            } else {
                left.emplace_back(std::format("{} FPS", MC::fps));
                if (isOn("showOnePercLows")) {
                    auto now = std::chrono::steady_clock::now();
                    if (std::chrono::duration_cast<std::chrono::seconds>(now - last1PercLowUpdate).count() >= 1) {
                        getOnePercLows();
                        last1PercLowUpdate = now;
                    }
                    left.emplace_back(std::format("1% Lows: {:.0f} FPS", cached1PercLow));
                }
            }
            left.emplace_back(std::format("Frametime: {:.2f}ms", MC::frameTime));
        }

        left.emplace_back("");

        if (isOn("showDim")) {
            left.emplace_back(std::format("E: {}", player->getLevel()->getRuntimeActorList().size()));
            left.emplace_back(getDimensionName());
            left.emplace_back("");
        }

        if (isOn("showBiome")) {
            left.emplace_back(std::format("Biome: {}", curBiome));
            left.emplace_back("");
        }

        if (isOn("showWeather")) { // Lightning always shows 100% for some reason
            std::vector<float> weatherInfo = getWeatherInfo();
            if (weatherInfo.empty()) {
                left.emplace_back("Weather: N/A");
                left.emplace_back("Lightning Level: N/A");
            }
            else {
                left.emplace_back(std::format("Rain Level: {:.0f}%", weatherInfo[0] * 100));
                left.emplace_back(std::format("Lightning Level: {:.0f}%", weatherInfo[1] * 100));
            }
            left.emplace_back("");
        }

        if (isOn("showCoords")) {
            Vec3<float> pos = *player->getPosition();
            left.emplace_back(std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", pos.x, pos.y, pos.z));
            Vec3<int> blockPos(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z));
            left.emplace_back(std::format("Block: {} {} {}", blockPos.x, blockPos.y, blockPos.z));
            left.emplace_back(std::format("Chunk: {} {} {}", static_cast<int>(pos.x / 16), static_cast<int>(pos.y / 16), static_cast<int>(pos.z / 16)));
            left.emplace_back(std::format("Chunk Coordinate: {} {}", static_cast<int>(pos.x) % 16, static_cast<int>(pos.z) % 16));
            left.emplace_back(getFacingDirection(player));
            left.emplace_back("");
            HitResult target = player->getLevel()->getHitResult();
            BlockPos targetPos = {target.blockPos.x, target.blockPos.y, target.blockPos.z};
            left.emplace_back(std::format("Looking at block: {} {} {}", targetPos.x, targetPos.y, targetPos.z));
        }

        if (isOn("showSpeed")) left.emplace_back(std::format("Speed: {:.2f} blocks/s", sqrt(std::pow(xVelo, 2) + std::pow(yVelo, 2) + std::pow(zVelo, 2))));

        if (isOn("showVelocity")) left.emplace_back(std::format("Velocity: {:.2f} / {:.2f} / {:.2f} blocks/s", xVelo, yVelo, zVelo));

        bool renderBreakProg = isOn("showBreakProg") && (getOps<bool>("alwaysShowBreakProg") || currentBreakProgress != 0.0f);
        if (renderBreakProg) left.emplace_back(std::format("Break Progress: {}%", static_cast<int>(currentBreakProgress)));

        if (isOn("showCoords") || isOn("showSpeed") || isOn("showVelocity") || renderBreakProg) left.emplace_back("");

        if (isOn("showInGameTime")) left.emplace_back(std::format("World Time: {}", Time::formatMCTime(Time::curTime, false)));

        if (isOn("showWorldName")) left.emplace_back(std::format("World Name: {}", SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName()));

        if (isOn("showInGameTime") || isOn("showWorldName")) left.emplace_back("");

        if (isOn("showServer")) {
            left.emplace_back("Server:");
            std::string serverIp = SDK::getServerIP();
            if (serverIp == "world") {
                left.emplace_back("IP: local");
                left.emplace_back("Port: 0");
                left.emplace_back("Ping: -1 ms");
            }
            else {
                left.emplace_back(std::format("IP: {}", SDK::getServerIP()));
                left.emplace_back(std::format("Port: {}", SDK::getServerPort()));
                left.emplace_back(std::format("Ping: {} ms", SDK::getServerPing()));
            }
            updateTimedVector(tickList, 1.0f);
            left.emplace_back(std::format("TPS: {}", std::to_string(tickList.size())));
        }


        right.emplace_back("64bit");
        if (isOn("showMemory")) {
            MEMORYSTATUSEX memory_status;
            memory_status.dwLength = sizeof(memory_status);
            GlobalMemoryStatusEx(&memory_status);
            int total_memory = static_cast<int>(memory_status.ullTotalPhys / 1000000);
            int free_memory = static_cast<int>(memory_status.ullAvailPhys / 1000000);
            int used_memory = total_memory - free_memory;
            if (getOps<bool>("imPoorButIWannaLookRich")) {
                int totallyRealTotalMemory = 2097152;
                right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / totallyRealTotalMemory), used_memory, totallyRealTotalMemory));
            } else right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / total_memory), used_memory, total_memory));
            right.emplace_back("");
        }

        if (isOn("showCpuGpu")) {
            if (getOps<bool>("imPoorButIWannaLookRich")) right.emplace_back("Intel 9 7900X3D ProMax Plus");
            else {
                if (cpuName.empty()) std::string cpuName = getCPU();
                right.emplace_back(std::format("CPU: {}", getCPU()));
            }

            right.emplace_back("");

            right.emplace_back(std::format("Display: {}x{}", MC::windowSize.x, MC::windowSize.y));
            right.emplace_back(std::format("Active Renderer: {}", SwapchainHook::queue == nullptr ? "DirectX11" : "DirectX12"));
            if (getOps<bool>("imPoorButIWannaLookRich")) right.emplace_back("AMD GFX 5090 Ti AI Accelerated DLSS 12.0");
            else {
                if (!MC::GPU.empty()) right.emplace_back(MC::GPU);
                else right.emplace_back("Unknown GPU");
            }

            right.emplace_back("");
        }

        if (isOn("showTime")) right.emplace_back(std::format("Local Time: {}", getTime()));

        if (isOn("showUptime")) {
            right.emplace_back(std::format("CPU Uptime: {}", getFormattedTime(static_cast<long long>(GetTickCount64() / 1000))));
            right.emplace_back(std::format("Minecraft Uptime: {}", getFormattedTime(
                                               static_cast<long long>(
                                                   std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()
                                               ) / 1000
                                           )));
            right.emplace_back("");
        }

        if (isOn("showTargetedBlock")) {
            right.emplace_back("Targeted Block:");
            right.emplace_back(lookingAt);
            right.emplace_back("");

            if (isOn("showTargetedBlockTags")) {
                bool showMax = getOps<bool>("showMaxTags");
                int maxAllowedTags = getOps<int>("noOfTags");
                int maxFittableTags = static_cast<int>(MC::windowSize.y / (textHeight / 3.0f + yPadding * 2)) - right.size();

                if (lookingAtTags.size() >= maxFittableTags && (showMax || maxAllowedTags >= maxFittableTags)) {
                    for (int i = 0; i < maxFittableTags; i++) right.emplace_back('#' + lookingAtTags[i]);
                    right.emplace_back(std::format("{} more tags...", lookingAtTags.size() - maxFittableTags));
                } else if (showMax) {
                    for (const auto &i: lookingAtTags) right.emplace_back('#' + i);
                } else {
                    for (int i = 0; i < maxAllowedTags; i++) right.emplace_back('#' + lookingAtTags[i]);
                    right.emplace_back(std::format("{} more tags...", lookingAtTags.size() - maxAllowedTags));
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

            if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

            if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

        if (isOn("showFTgraph") || isOn("showOnePercLows")) {
            while (prevFrameTimes.size() >= static_cast<int>(Constraints::SpacingConstraint(getOps<float>("FTgraphWidth") * 10, uiscaleConst) / Constraints::SpacingConstraint(getOps<float>("FTbarWidth"), uiscaleConst))) prevFrameTimes.pop_front();
            prevFrameTimes.push_back(MC::frameTime);
        }

        if (getOps<bool>("showFTgraph")) {
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

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
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
    if (this->active && SDK::clientInstance && SDK::clientInstance->getScreenName() == "hud_screen" && SDK::clientInstance->getLocalPlayer() != nullptr) {
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

void JavaDebugMenu::onSetTopScreenName(SetTopScreenNameEvent &event) {
    if (!this->isEnabled()) return;
    if (this->active && getOps<bool>("hideModules") && event.getLayer() == "hud_screen") event.setCustomLayer("f3_screen");
}

void JavaDebugMenu::onGetViewPerspective(PerspectiveEvent &event) {
    if (!this->active || !this->isEnabled() || !SDK::clientInstance->getLocalPlayer()) return;
    curPerspective = event.getPerspective();
}
