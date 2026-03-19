#include "HiveStat.hpp"

#include <thread>
#include <vector>
#include <algorithm>
#include <optional>
#include <shared_mutex>
#include <unordered_set>

#include "Client.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

namespace
{
    // Track players we already logged as having a missing/unhandled team char
    std::unordered_set<std::string> loggedMissingTeamPlayers;
    std::shared_mutex loggedMissingTeamMutex;

    struct ExtraColumnConfig
    {
        std::string key;
        const char* label;
    };

    ImVec2 getTableAnchorPivot(const std::string& anchor)
    {
        if (anchor == "Top Center") return ImVec2(0.5f, 0.0f);
        if (anchor == "Top Right") return ImVec2(1.0f, 0.0f);
        if (anchor == "Middle Left") return ImVec2(0.0f, 0.5f);
        if (anchor == "Middle Center") return ImVec2(0.5f, 0.5f);
        if (anchor == "Middle Right") return ImVec2(1.0f, 0.5f);
        if (anchor == "Bottom Left") return ImVec2(0.0f, 1.0f);
        if (anchor == "Bottom Center") return ImVec2(0.5f, 1.0f);
        if (anchor == "Bottom Right") return ImVec2(1.0f, 1.0f);
        return ImVec2(0.0f, 0.0f);
    }

    std::string formatFirstPlayedDate(long long unixTimestamp)
    {
        if (unixTimestamp <= 0) return "-";

        const std::time_t rawTime = static_cast<std::time_t>(unixTimestamp);
        std::tm timeInfo{};
        localtime_s(&timeInfo, &rawTime);

        char buffer[11] = {};
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeInfo);
        return std::string(buffer);
    }

    std::optional<ExtraColumnConfig> getPrimaryExtraColumn(const std::string& gameId)
    {
        if (gameId == "bed") return ExtraColumnConfig{"beds_destroyed", "Beds"};
        if (gameId == "sky") return ExtraColumnConfig{"ores_mined", "Ores"};
        if (gameId == "murder") return ExtraColumnConfig{"coins", "Coins"};
        if (gameId == "ctf") return ExtraColumnConfig{"flags_captured", "Flags Cap"};
        if (gameId == "sg") return ExtraColumnConfig{"crates", "Crates"};
        return std::nullopt;
    }

    std::optional<ExtraColumnConfig> getSecondaryExtraColumn(const std::string& gameId)
    {
        if (gameId == "sky") return ExtraColumnConfig{"mystery_chests_destroyed", "Chests"};
        if (gameId == "murder") return ExtraColumnConfig{"murderer_eliminations", "Murderer Kills"};
        if (gameId == "ctf") return ExtraColumnConfig{"assists", "Assists"};
        if (gameId == "sg") return ExtraColumnConfig{"deathmatches", "Deathmatches"};
        return std::nullopt;
    }

    std::string getGameDisplayName(const std::string& gameId)
    {
        if (gameId == "bed") return "BedWars";
        if (gameId == "sky") return "SkyWars";
        if (gameId == "murder") return "Murder Mystery";
        if (gameId == "ctf") return "Capture the Flag";
        if (gameId == "sg") return "Survival Games";
        return gameId;
    }

    std::string resolveSupportedHiveStatsGame(std::string currentGame, bool showInHubs)
    {
        if (currentGame.empty()) return "";

        std::ranges::transform(currentGame, currentGame.begin(),
                               [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

        auto mapSupportedGame = [](const std::string& gameCode) -> std::string
        {
            if (gameCode == "BED") return "bed";
            if (gameCode == "SKY") return "sky";
            if (gameCode == "MURDER") return "murder";
            if (gameCode == "CTF") return "ctf";
            if (gameCode == "SG") return "sg";
            return "";
        };

        if (currentGame.rfind("HUB-", 0) == 0)
        {
            if (!showInHubs) return "";

            std::string hubGame = currentGame.substr(4);
            const size_t dashPos = hubGame.find('-');
            if (dashPos != std::string::npos)
            {
                hubGame = hubGame.substr(0, dashPos);
            }

            return mapSupportedGame(hubGame);
        }

        std::string baseGame = currentGame;
        const size_t dashPos = baseGame.find('-');
        if (dashPos != std::string::npos)
        {
            baseGame = baseGame.substr(0, dashPos);
        }

        return mapSupportedGame(baseGame);
    }
}

void HiveStat::onSetup()
{
    keybindActions.clear();
}

void HiveStat::onEnable()
{
    Listen(this, RenderEvent, &HiveStat::onRender)
    Listen(this, PacketEvent, &HiveStat::onPacketReceive)
    Listen(this, KeyEvent, &HiveStat::onKey)
    Module::onEnable();
    FlarialGUI::Notify("Hive Overlay works only in sky, bed, mm, ctf, sg (hubs optional)");
}

void HiveStat::onDisable()
{
    stopThread = true;
    for (auto& thread : playerThreads | std::views::values)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    playerThreads.clear();
    playerStatsList.clear();
    queueList.clear();
    {
        std::unique_lock lock(playerTeamMutex);
        playerTeamCache.clear();
    }
    {
        std::unique_lock l(loggedMissingTeamMutex);
        loggedMissingTeamPlayers.clear();
    }
    {
        std::lock_guard<std::mutex> lock(leaderboardMutex);
        leaderboardEntries.clear();
        leaderboardCode = 0;
        leaderboardGameId.clear();
        leaderboardMonthly = false;
        leaderboardLastFetch = std::chrono::steady_clock::time_point::min();
        leaderboardFetchInProgress = false;
    }
    Deafen(this, RenderEvent, &HiveStat::onRender)
    Deafen(this, PacketEvent, &HiveStat::onPacketReceive)
    Deafen(this, KeyEvent, &HiveStat::onKey)
    Module::onDisable();
}

void HiveStat::defaultConfig()
{
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");

    setDef("uiscale", 1.f);

    setDef("Overlay", static_cast<std::string>("O"));
    setDef("showFKDR", true);
    setDef("showKD", true);
    setDef("showWR", true);
    setDef("showLevel", true);
    setDef("showWins", false);
    setDef("showLosses", false);
    setDef("showKills", false);
    setDef("showFinalKills", false);
    setDef("showDeaths", false);
    setDef("showPlayed", false);
    setDef("showFirstPlayed", false);
    setDef("showPrimaryExtra", false);
    setDef("showSecondaryExtra", false);
    setDef("showInHubs", false);
    setDef("tryGetTeamColor", false);
    setDef("enableHighlight", false);
    setDef("highlightPriority", true);
    setDef("highlightCount", 0);
    setDef("tableAnchor", static_cast<std::string>("Top Left"));
    setDef("showLeaderboard", false);
    setDef("separateLeaderboard", false);
    setDef("leaderboardMode", static_cast<std::string>("All-Time"));
    setDef("leaderboardRows", 10);
    setDef("leaderboardRefreshSec", 60);
    setDef("lbShowKills", false);
    setDef("lbShowDeaths", false);
    setDef("lbShowKD", false);
    setDef("lbShowLevel", false);
    setDef("lbShowSecondaryExtra", false);

    setDef("showThresholdFKDR", true);
    setDef("showThresholdKD", true);
    setDef("showThresholdWR", true);
    setDef("showThresholdLevel", true);

    setDef("threshold-FKDR-1", 10.f);
    setDef("threshold-FKDR-2", 5.f);
    setDef("threshold-FKDR-3", 3.f);
    setDef("threshold-FKDR-4", 1.5f);

    setDef("threshold-KD-1", 10.f);
    setDef("threshold-KD-2", 5.f);
    setDef("threshold-KD-3", 3.f);
    setDef("threshold-KD-4", 1.5f);

    setDef("threshold-WR-1", 90.f);
    setDef("threshold-WR-2", 80.f);
    setDef("threshold-WR-3", 50.f);
    setDef("threshold-WR-4", 20.f);

    setDef("threshold-Level-1", 100);
    setDef("threshold-Level-2", 80);
    setDef("threshold-Level-3", 50);
    setDef("threshold-Level-4", 30);
    setDef("threshold-Level-5", 25);
    setDef("threshold-Level-6", 20);
    setDef("threshold-Level-7", 10);

    setDef("showThresholdBeds", false);
    setDef("threshold-Beds-1", 5000);
    setDef("threshold-Beds-2", 3000);
    setDef("threshold-Beds-3", 1000);
    setDef("threshold-Beds-4", 500);
    setDef("threshold-Beds-5", 200);
    setDef("threshold-Beds-6", 100);
    setDef("threshold-Beds-7", 50);

    setDef("showThresholdPlayed", false);
    setDef("threshold-Played-1", 10000);
    setDef("threshold-Played-2", 5000);
    setDef("threshold-Played-3", 3000);
    setDef("threshold-Played-4", 1000);
    setDef("threshold-Played-5", 500);
    setDef("threshold-Played-6", 200);
    setDef("threshold-Played-7", 100);

    setDef("showThresholdFinalKills", false);
    setDef("threshold-FinalKills-1", 10000);
    setDef("threshold-FinalKills-2", 5000);
    setDef("threshold-FinalKills-3", 3000);
    setDef("threshold-FinalKills-4", 1000);
    setDef("threshold-FinalKills-5", 500);
    setDef("threshold-FinalKills-6", 200);
    setDef("threshold-FinalKills-7", 100);
}

void HiveStat::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addKeybind("Toggle Overlay Keybind", "", "Overlay", true);
    addSlider("UI Scale", "", "uiscale", 5.f, 0.f);
    extraPadding();

    addHeader("Misc");
    addToggle("Show FKDR", "Shows final kill final death ratio", "showFKDR");
    addToggle("Show KD", "Shows the amount of kills per death", "showKD");
    addToggle("Show Win Rate", "Shows the player's win rate as a percentage ", "showWR");
    addToggle("Show Level", "Shows the level of a player", "showLevel");
    addToggle("Show Victories", "Shows the amount of wins a player has", "showWins");
    addToggle("Show Losses", "Shows the amount of losses a player has", "showLosses");
    addToggle("Show Kills", "Shows the amount of kills a player has", "showKills");
    addToggle("Show Final Kills", "Shows the amount of final kills a player has (BedWars only)", "showFinalKills");
    addToggle("Show Deaths", "Shows the amount of deaths a player has", "showDeaths");
    addToggle("Show Played", "Shows how many games the player has played", "showPlayed");
    addToggle("Show First Played", "Shows the date this player first played this game",
              "showFirstPlayed");
    addToggle("Show Primary API Stat",
              "Shows a game-specific API stat (Beds/Ores/Coins/Flags/Crates)", "showPrimaryExtra");
    addToggle("Show Secondary API Stat",
              "Shows another game-specific API stat (Final Kills/Chests/Murderer Kills/Assists/Deathmatches)",
              "showSecondaryExtra");
    addToggle("Show in Hubs", "Shows stats while in game hubs for supported games", "showInHubs");
    addToggle("Deduce Team Colors",
              "Tries to retrieve the players' team colors from nametags.\nPlayers need to be in visual range for colors to load.",
              "tryGetTeamColor");
    addDropdown("Table Anchor", "", std::vector<std::string>{
                    "Top Left",
                    "Top Center",
                    "Top Right",
                    "Middle Left",
                    "Middle Center",
                    "Middle Right",
                    "Bottom Left",
                    "Bottom Center",
                    "Bottom Right"
                }, "tableAnchor", true);
    addToggle("Show Leaderboard", "Show global leaderboard for this game", "showLeaderboard");
    addConditionalToggle(getOps<bool>("showLeaderboard"), "Separate Leaderboard Window",
                         "Render the leaderboard in its own window instead of attached to the stats table",
                         "separateLeaderboard");
    addConditionalDropdown(getOps<bool>("showLeaderboard"), "Leaderboard Mode", "",
                           std::vector<std::string>{"All-Time", "Monthly"}, "leaderboardMode",
                           true);
    addConditionalSliderInt(getOps<bool>("showLeaderboard"), "Leaderboard Rows",
                            "How many leaderboard rows to show", "leaderboardRows", 100, 1);
    addConditionalSliderInt(getOps<bool>("showLeaderboard"), "Leaderboard Refresh (s)",
                            "Refresh interval for leaderboard API fetches", "leaderboardRefreshSec",
                            300, 5);
    addConditionalToggle(getOps<bool>("showLeaderboard"), "LB: Show Kills", "", "lbShowKills");
    addConditionalToggle(getOps<bool>("showLeaderboard"), "LB: Show Deaths", "", "lbShowDeaths");
    addConditionalToggle(getOps<bool>("showLeaderboard"), "LB: Show KD", "", "lbShowKD");
    addConditionalToggle(getOps<bool>("showLeaderboard"), "LB: Show Level", "", "lbShowLevel");
    addConditionalToggle(getOps<bool>("showLeaderboard"), "LB: Show Secondary Extra",
                         "Shows a secondary game-specific stat (Final Kills/Chests/Murderer Kills/Assists/Deathmatches)",
                         "lbShowSecondaryExtra");
    extraPadding();

    addHeader("Player Highlights");
    addToggle("Enable Highlights", "Highlight specific players with custom colors", "enableHighlight");
    addConditionalToggle(getOps<bool>("enableHighlight") && getOps<bool>("tryGetTeamColor"),
                         "Highlight Priority over Team Color",
                         "Custom highlight color takes priority over deduced team color",
                         "highlightPriority");
    if (getOps<bool>("enableHighlight"))
    {
        totalHighlights = getOps<int>("highlightCount");
        addButton("Add Highlight", "Add a player to highlight", "Add", [this]
        {
            int idx = totalHighlights;
            std::string nameKey = "highlight-" + FlarialGUI::cached_to_string(idx) + "-name";
            settings.addSetting(nameKey, static_cast<std::string>(""));
            setDef("highlight-" + FlarialGUI::cached_to_string(idx), static_cast<std::string>("ff0000"), 1.f, false);
            totalHighlights++;
            this->settings.setValue("highlightCount", totalHighlights);
            Client::SaveSettings();
            FlarialGUI::Notify("Added! Scroll down to configure.");
        });
        for (int i = 0; i < totalHighlights; i++)
        {
            std::string nameKey = "highlight-" + FlarialGUI::cached_to_string(i) + "-name";
            std::string colorKey = "highlight-" + FlarialGUI::cached_to_string(i);
            if (settings.getSettingByName<std::string>(nameKey) != nullptr)
            {
                addTextBox(std::format("Player {}", i + 1), "Player name (case-insensitive)", 30, nameKey);
                addColorPicker(std::format("Color {}", i + 1), "", colorKey);
                addButton(std::format("Remove {}", i + 1), "", "Delete", [this, i, nameKey, colorKey]
                {
                    settings.deleteSetting(nameKey);
                    settings.deleteSetting(colorKey + "Col");
                    settings.deleteSetting(colorKey + "Opacity");
                    settings.deleteSetting(colorKey + "RGB");
                    Client::SaveSettings();
                });
            }
        }
    }
    extraPadding();

    addHeader("Thresholds");

    for (std::string s : std::vector<std::string>{"FKDR", "KD", "WR"})
    {
        addToggle(std::format("Show {} Threshold Settings", s),
                  "Colors are from highest threshold to lowest.\n1. Dark Blue\n2. Red\n3. Green\n4. Gold\n5. Gray",
                  std::format("showThreshold{}", s));
        const bool enabled = getOps<bool>(std::format("show{}", s)) && getOps<bool>(
            std::format("showThreshold{}", s));;
        for (int i = 1; i < 5; i++)
        {
            addConditionalSlider(
                enabled,
                std::format("{} Threshold {}", String::toUpper(s), i),
                "",
                std::format("threshold-{}-{}", s, i),
                100.f,
                0.f
            );
        }
        if (enabled) extraPadding();
    }

    addToggle("Show Level Threshold Settings",
              "Colors are from highest threshold to lowest.\n1. Dark blue\n2. Red\n3. Green\n4. Magenta\n5. Aqua\n6. Yellow\n7. Gold\n8. Gray",
              "showThresholdLevel");
    const bool c = getOps<bool>("showLevel") && getOps<bool>("showThresholdLevel");
    for (int i = 1; i < 8; i++)
    {
        addConditionalSliderInt(
            c,
            std::format("Level Threshold {}", i),
            "",
            std::format("threshold-Level-{}", i),
            100,
            0
        );
    }

    for (const auto& [label, key, showKey, maxVal] : std::vector<std::tuple<std::string, std::string, std::string, int>>{
        {"Beds", "Beds", "showPrimaryExtra", 10000},
        {"Played", "Played", "showPlayed", 20000},
        {"Final Kills", "FinalKills", "showFinalKills", 20000}
    })
    {
        addToggle(std::format("Show {} Threshold Settings", label),
                  "Colors are from highest threshold to lowest.\n1. Dark blue\n2. Red\n3. Green\n4. Magenta\n5. Aqua\n6. Yellow\n7. Gold\n8. Gray",
                  std::format("showThreshold{}", key));
        const bool en = getOps<bool>(showKey) && getOps<bool>(std::format("showThreshold{}", key));
        for (int i = 1; i < 8; i++)
        {
            addConditionalSliderInt(
                en,
                std::format("{} Threshold {}", label, i),
                "",
                std::format("threshold-{}-{}", key, i),
                maxVal,
                0
            );
        }
        if (en) extraPadding();
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HiveStat::fetchPlayerStats(const std::string& playerName)
{
    const std::string cg = resolveSupportedHiveStatsGame(HiveModeCatcherListener::currentGame,
                                                         getOps<bool>("showInHubs"));
    if (cg.empty()) return;

    Hive::PlayerStats stats = Hive::GetStats(cg, playerName);
    std::unique_lock<std::shared_mutex> lock(queueMutex);
    playerStatsList.emplace_back(playerName, stats);
}

void HiveStat::fetchLeaderboard(const std::string& gameId, bool monthly)
{
    Hive::LeaderboardResult result = Hive::GetLeaderboard(gameId, monthly);

    std::lock_guard<std::mutex> lock(leaderboardMutex);
    leaderboardEntries = std::move(result.entries);
    leaderboardCode = result.code;
    leaderboardGameId = gameId;
    leaderboardMonthly = monthly;
    leaderboardLastFetch = std::chrono::steady_clock::now();
    leaderboardFetchInProgress = false;
}

void HiveStat::onRender(RenderEvent& event)
{
    float uiScale = getOps<float>("uiscale");

    constexpr float width = 150.0f;
    constexpr float height = 50.0f;

    float scaledWidth = width * uiScale;
    float scaledHeight = height * uiScale;
    ImVec2 tableAnchorPivot = getTableAnchorPivot(getOps<std::string>("tableAnchor"));
    bool isTopLeftAnchor = tableAnchorPivot.x == 0.0f && tableAnchorPivot.y == 0.0f;

    if (ClientInstance::getTopScreenName() == "hud_screen" && this->isEnabled() && !
        ClickGUI::blurActive)
    {
        ClickGUI::HudFadeGuard fadeGuard;

        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"),
                                              getOps<float>("percentageY"));

        if (settingperc.x != 0.0f)
        {
            if (isTopLeftAnchor)
            {
                currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width),
                                         settingperc.y * (MC::windowSize.y - height));
            }
            else
            {
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            }
        }
        else
        {
            currentPos = isTopLeftAnchor
                             ? Constraints::CenterConstraint(width, height)
                             : Vec2<float>(MC::windowSize.x * 0.5f, MC::windowSize.y * 0.5f);
        }

        float anchorOffsetX = scaledWidth * tableAnchorPivot.x;
        float anchorOffsetY = scaledHeight * tableAnchorPivot.y;
        float anchorTopLeftX = currentPos.x - anchorOffsetX;
        float anchorTopLeftY = currentPos.y - anchorOffsetY;

        if (ClickGUI::editmenu)
        {
            FlarialGUI::SetWindowRect(anchorTopLeftX, anchorTopLeftY, scaledWidth, scaledHeight,
                                      123, this->name);
            FlarialGUI::FlarialTextWithFont(
                anchorTopLeftX, anchorTopLeftY,
                L"Overlay", scaledWidth, scaledHeight,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(1.05f, scaledWidth),
                DWRITE_FONT_WEIGHT_NORMAL
            );
        }

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(anchorTopLeftX, anchorTopLeftY, 123,
                                                        scaledWidth, scaledHeight);
        currentPos.x = vec2.x + anchorOffsetX;
        currentPos.y = vec2.y + anchorOffsetY;

        Vec2<float> percentages = isTopLeftAnchor
                                      ? Constraints::CalculatePercentage(
                                          currentPos.x, currentPos.y, width, height)
                                      : Constraints::CalculatePercentage(
                                          currentPos.x, currentPos.y, 0, 0);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
    }

    if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;

    if (!renderOverlay) return;

    std::string cg = resolveSupportedHiveStatsGame(HiveModeCatcherListener::currentGame,
                                                   getOps<bool>("showInHubs"));
    if (cg.empty()) return;

    auto primaryExtraColumn = getPrimaryExtraColumn(cg);
    auto secondaryExtraColumn = getSecondaryExtraColumn(cg);

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() ||
        SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
    {
        return;
    }

    // Build team color cache from runtime actor nametags
    bool wantTeamColors = getOps<bool>("tryGetTeamColor");
    if (wantTeamColors)
    {
        auto* me = SDK::clientInstance->getLocalPlayer();
        auto actors = me->getLevel()->getRuntimeActorList();
        for (auto* actor : actors)
        {
            if (!actor || actor == me) continue;
            std::string* nametag = actor->getNametag();
            if (!nametag || nametag->empty()) continue;

            char teamChar = extractTeamChar(*nametag);
            if (teamChar == '\0') continue;

            // Extract clean name to use as cache key
            std::string cleanName = String::removeNonAlphanumeric(
                String::removeColorCodes(*nametag));
            if (cleanName.empty()) continue;

            std::unique_lock lock(playerTeamMutex);
            playerTeamCache[cleanName] = teamChar;
        }
    }

    bool wantHighlights = getOps<bool>("enableHighlight");

    ImVec2 position(currentPos.x, currentPos.y);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, tableAnchorPivot);

    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowFontScale(uiScale);

    bool showLeaderboard = getOps<bool>("showLeaderboard");
    bool monthlyLeaderboard = getOps<std::string>("leaderboardMode") == "Monthly";
    int leaderboardRows = std::clamp(getOps<int>("leaderboardRows"), 1, 100);
    int leaderboardRefreshSec = std::max(5, getOps<int>("leaderboardRefreshSec"));

    if (showLeaderboard)
    {
        bool shouldFetch = false;
        {
            std::lock_guard<std::mutex> lock(leaderboardMutex);
            auto now = std::chrono::steady_clock::now();
            bool hasFetchedAtLeastOnce = leaderboardLastFetch !=
                std::chrono::steady_clock::time_point::min();
            bool settingsChanged = leaderboardGameId != cg || leaderboardMonthly !=
                monthlyLeaderboard;
            bool shouldRefresh = !hasFetchedAtLeastOnce ||
                std::chrono::duration_cast<std::chrono::seconds>(now - leaderboardLastFetch).count()
                >= leaderboardRefreshSec;

            if (!leaderboardFetchInProgress && (settingsChanged || shouldRefresh))
            {
                leaderboardFetchInProgress = true;
                shouldFetch = true;
            }
        }

        if (shouldFetch)
        {
            std::thread([this, cg, monthlyLeaderboard]()
            {
                fetchLeaderboard(cg, monthlyLeaderboard);
            }).detach();
        }
    }

    int columnAmount = 1;

    if (getOps<bool>("showFKDR") == true && cg == "bed")
        columnAmount++;
    if (getOps<bool>("showKD") == true)
        columnAmount++;
    if (getOps<bool>("showWR") == true)
        columnAmount++;
    if (getOps<bool>("showLevel") == true)
        columnAmount++;
    if (getOps<bool>("showWins") == true)
        columnAmount++;
    if (getOps<bool>("showLosses") == true)
        columnAmount++;
    if (getOps<bool>("showKills") == true)
        columnAmount++;
    if (getOps<bool>("showFinalKills") == true && cg == "bed")
        columnAmount++;
    if (getOps<bool>("showDeaths") == true)
        columnAmount++;
    if (getOps<bool>("showPlayed") == true)
        columnAmount++;
    if (getOps<bool>("showPrimaryExtra") == true && primaryExtraColumn.has_value())
        columnAmount++;
    if (getOps<bool>("showSecondaryExtra") == true && secondaryExtraColumn.has_value())
        columnAmount++;
    if (getOps<bool>("showFirstPlayed") == true)
        columnAmount++;

    if (ImGui::BeginTable("Player Stats", columnAmount,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_BordersV))
    {
        ImGui::TableSetupColumn("Player Name");
        if (getOps<bool>("showFKDR") == true && cg == "bed")
            ImGui::TableSetupColumn("FKDR");
        if (getOps<bool>("showKD") == true)
            ImGui::TableSetupColumn("KD");
        if (getOps<bool>("showWR") == true)
            ImGui::TableSetupColumn("Win Rate");
        if (getOps<bool>("showLevel") == true)
            ImGui::TableSetupColumn("Level");
        if (getOps<bool>("showWins") == true)
            ImGui::TableSetupColumn("Victories");
        if (getOps<bool>("showLosses") == true)
            ImGui::TableSetupColumn("Losses");
        if (getOps<bool>("showKills") == true)
            ImGui::TableSetupColumn("Kills");
        if (getOps<bool>("showFinalKills") == true && cg == "bed")
            ImGui::TableSetupColumn("FKills");
        if (getOps<bool>("showDeaths") == true)
            ImGui::TableSetupColumn("Deaths");
        if (getOps<bool>("showPlayed") == true)
            ImGui::TableSetupColumn("Played");
        if (getOps<bool>("showPrimaryExtra") == true && primaryExtraColumn.has_value())
            ImGui::TableSetupColumn(primaryExtraColumn->label);
        if (getOps<bool>("showSecondaryExtra") == true && secondaryExtraColumn.has_value())
            ImGui::TableSetupColumn(secondaryExtraColumn->label);
        if (getOps<bool>("showFirstPlayed") == true)
            ImGui::TableSetupColumn("First Played");

        ImGui::TableHeadersRow();

        auto* levelForMap = SDK::clientInstance->getLocalPlayer()->getLevel();
        if (!levelForMap) return;
        for (const auto& pair : levelForMap->getPlayerMap())
        {
            if (pair.second.name.empty()) continue;
            std::string name = String::removeNonAlphanumeric(
                String::removeColorCodes(pair.second.name));

            auto it = std::ranges::find_if(playerStatsList,
                                           [&name](
                                           const std::pair<std::string, Hive::PlayerStats>& element)
                                           {
                                               return element.first == name;
                                           });

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // Determine name color based on priority setting
            ImU32 nameColor = IM_COL32(255, 255, 255, 255);
            bool hasNameColor = false;
            bool highlightFirst = wantHighlights && getOps<bool>("highlightPriority");

            // If highlight has priority, check it first
            if (highlightFirst)
            {
                ImU32 hlColor = getHighlightColor(name);
                if (hlColor != 0)
                {
                    nameColor = hlColor;
                    hasNameColor = true;
                }
            }

            if (!hasNameColor && wantTeamColors)
            {
                std::shared_lock lock(playerTeamMutex);
                auto teamIt = playerTeamCache.find(name);
                if (teamIt != playerTeamCache.end())
                {
                    auto colIt = teamColors.find(teamIt->second);
                    if (colIt != teamColors.end())
                    {
                        nameColor = colIt->second;
                        hasNameColor = true;
                    }
                    else
                    {
                        // Log missing team color code once
                        bool needLog = false;
                        {
                            std::shared_lock sl(loggedMissingTeamMutex);
                            needLog = !loggedMissingTeamPlayers.contains(name);
                        }
                        if (needLog)
                        {
                            std::unique_lock ul(loggedMissingTeamMutex);
                            if (loggedMissingTeamPlayers.insert(name).second)
                            {
                                Logger::info("Missing team color for code '{}' (player: {}).",
                                             teamIt->second, name);
                            }
                        }
                    }
                }
            }

            // If highlight didn't have priority but is still enabled, check it as fallback
            if (!hasNameColor && wantHighlights && !highlightFirst)
            {
                ImU32 hlColor = getHighlightColor(name);
                if (hlColor != 0)
                {
                    nameColor = hlColor;
                    hasNameColor = true;
                }
            }

            if (hasNameColor) ImGui::PushStyleColor(ImGuiCol_Text, nameColor);
            ImGui::Text("%s", name.c_str());
            if (hasNameColor) ImGui::PopStyleColor();

            if (columnAmount > 1)
            {
                int columnIndex = 1;

                ImGui::TableSetColumnIndex(columnIndex);
                if (it != playerStatsList.end())
                {
                    if (it->second.getCode() == 0 || it->second.getCode() == 2)
                    {
                        if (getOps<bool>("showFKDR") == true && cg == "bed")
                        {
                            std::ostringstream oss;
                            float fkdr = it->second.getFKDR();
                            oss << std::fixed << std::setprecision(2) << fkdr;

                            if (fkdr >= getOps<float>("threshold-FKDR-1"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (fkdr >= getOps<float>("threshold-FKDR-2"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (fkdr >= getOps<float>("threshold-FKDR-3"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (fkdr >= getOps<float>("threshold-FKDR-4"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showKD") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            float kd = it->second.getKD();
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(2) << kd;

                            if (kd >= getOps<float>("threshold-KD-1"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (kd >= getOps<float>("threshold-KD-2"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (kd >= getOps<float>("threshold-KD-3"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (kd >= getOps<float>("threshold-KD-4"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showWR") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            float winrate = it->second.getWinRate();
                            oss << std::fixed << std::setprecision(0) << winrate << "%";

                            if (winrate >= getOps<float>("threshold-WR-1"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (winrate >= getOps<float>("threshold-WR-2"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (winrate >= getOps<float>("threshold-WR-3"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (winrate >= getOps<float>("threshold-WR-4"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showLevel") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int level = it->second.getLevel();
                            oss << std::fixed << std::setprecision(0) << level;

                            if (level >= getOps<int>("threshold-Level-1"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (level >= getOps<int>("threshold-Level-2"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (level >= getOps<int>("threshold-Level-3"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (level >= getOps<int>("threshold-Level-4"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            }
                            else if (level >= getOps<int>("threshold-Level-5"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            }
                            else if (level >= getOps<int>("threshold-Level-6"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            }
                            else if (level >= getOps<int>("threshold-Level-7"))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s%s", it->second.getPrestige(), oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showWins") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int victories = it->second.getVictories();
                            oss << std::fixed << std::setprecision(0) << victories;

                            if (victories >= 10000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (victories >= 8000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (victories >= 5000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (victories >= 3000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            }
                            else if (victories >= 1000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            }
                            else if (victories >= 500)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            }
                            else if (victories >= 100)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showLosses") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int losses = it->second.getLosses();
                            oss << std::fixed << std::setprecision(0) << losses;

                            if (losses >= 10000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_red"));
                            }
                            else if (losses >= 8000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (losses >= 5000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("orange"));
                            }
                            else if (losses >= 3000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("olive"));
                            }
                            else if (losses >= 1000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (losses >= 500)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_aqua"));
                            }
                            else if (losses >= 100)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("light_gray"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showKills") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int kills = it->second.getKills();
                            oss << std::fixed << std::setprecision(0) << kills;

                            if (kills >= 10000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (kills >= 8000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (kills >= 5000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (kills >= 3000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            }
                            else if (kills >= 1000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            }
                            else if (kills >= 500)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            }
                            else if (kills >= 100)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showFinalKills") == true && cg == "bed")
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            int fkills = it->second.getExtraStat("final_kills", -1);
                            if (fkills >= 0)
                            {
                                bool useThresholds = getOps<bool>("showThresholdFinalKills");
                                if (useThresholds)
                                {
                                    if (fkills >= getOps<int>("threshold-FinalKills-1"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-2"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-3"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-4"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-5"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-6"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                                    else if (fkills >= getOps<int>("threshold-FinalKills-7"))
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                                    else
                                        ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                                }

                                ImGui::Text("%d", fkills);
                                if (useThresholds) ImGui::PopStyleColor();
                            }
                            else
                            {
                                ImGui::Text("-");
                            }
                            columnIndex++;
                        }

                        if (getOps<bool>("showDeaths") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int deaths = it->second.getDeaths();
                            oss << std::fixed << std::setprecision(0) << deaths;

                            if (deaths >= 20000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            }
                            else if (deaths >= 10000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_red"));
                            }
                            else if (deaths >= 8000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            }
                            else if (deaths >= 5000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("orange"));
                            }
                            else if (deaths >= 3000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("olive"));
                            }
                            else if (deaths >= 1000)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            }
                            else if (deaths >= 500)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_aqua"));
                            }
                            else if (deaths >= 100)
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("light_gray"));
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showPlayed") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            int played = it->second.getPlayed();
                            bool usePlayedThresh = getOps<bool>("showThresholdPlayed");
                            if (usePlayedThresh)
                            {
                                if (played >= getOps<int>("threshold-Played-1"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                                else if (played >= getOps<int>("threshold-Played-2"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                                else if (played >= getOps<int>("threshold-Played-3"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                                else if (played >= getOps<int>("threshold-Played-4"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                                else if (played >= getOps<int>("threshold-Played-5"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                                else if (played >= getOps<int>("threshold-Played-6"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                                else if (played >= getOps<int>("threshold-Played-7"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                                else
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%d", played);
                            if (usePlayedThresh) ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showPrimaryExtra") == true && primaryExtraColumn.
                            has_value())
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            int primaryValue = it->second.getExtraStat(primaryExtraColumn->key, -1);
                            if (primaryValue >= 0)
                            {
                                bool useBedsThresh = getOps<bool>("showThresholdBeds") && primaryExtraColumn->key == "beds_destroyed";
                                if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-1"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-2"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-3"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-4"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-5"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-6"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                                else if (useBedsThresh && primaryValue >= getOps<int>("threshold-Beds-7"))
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                                else if (useBedsThresh)
                                    ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));

                                ImGui::Text("%d", primaryValue);
                                if (useBedsThresh) ImGui::PopStyleColor();
                            }
                            else ImGui::Text("-");
                            columnIndex++;
                        }

                        if (getOps<bool>("showSecondaryExtra") == true && secondaryExtraColumn.
                            has_value())
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            int secondaryValue = it->second.getExtraStat(
                                secondaryExtraColumn->key, -1);
                            if (secondaryValue >= 0) ImGui::Text("%d", secondaryValue);
                            else ImGui::Text("-");
                            columnIndex++;
                        }

                        if (getOps<bool>("showFirstPlayed") == true)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            std::string firstPlayedDate = formatFirstPlayedDate(
                                it->second.getFirstPlayed());
                            ImGui::Text("%s", firstPlayedDate.c_str());
                            columnIndex++;
                        }
                    }
                    else
                    {
                        switch (it->second.getCode())
                        {
                        case 1:
                            ImGui::Text("err");
                            for (int col = 2; col < columnAmount; ++col)
                            {
                                ImGui::TableSetColumnIndex(col);
                                if (col == 2) ImGui::Text("nick");
                                else ImGui::Text("-");
                            }
                            break;

                        case 3:
                            ImGui::Text("err");
                            for (int col = 2; col < columnAmount; ++col)
                            {
                                ImGui::TableSetColumnIndex(col);
                                if (col == 2) ImGui::Text("API DEAD :skull:");
                                else ImGui::Text("-");
                            }
                            break;

                        default:
                            ImGui::Text("err");
                            for (int col = 2; col < columnAmount; ++col)
                            {
                                ImGui::TableSetColumnIndex(col);
                                if (col == 2) ImGui::Text("unknown error");
                                else ImGui::Text("-");
                            }
                        }
                    }
                }
                else
                {
                    std::unique_lock lock(queueMutex);
                    if (std::ranges::find(queueList, name) == queueList.end())
                    {
                        queueList.push_back(name);
                        playerThreads[name] = std::thread([this, name]()
                        {
                            fetchPlayerStats(name);
                        });
                        playerThreads[name].detach();
                    }
                    ImGui::Text("Loading stats...");
                    for (int col = 2; col < columnAmount; ++col)
                    {
                        ImGui::TableSetColumnIndex(col);
                        ImGui::Text("-");
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    bool separateLeaderboard = getOps<bool>("separateLeaderboard");

    if (showLeaderboard && !separateLeaderboard)
    {
        ImGui::Separator();
    }

    ImGui::End(); // End main overlay window

    if (showLeaderboard)
    {
        bool lbShowKills = getOps<bool>("lbShowKills");
        bool lbShowDeaths = getOps<bool>("lbShowDeaths");
        bool lbShowKD = getOps<bool>("lbShowKD");
        bool lbShowLevel = getOps<bool>("lbShowLevel");
        bool lbShowSecondaryExtra = getOps<bool>("lbShowSecondaryExtra");

        if (separateLeaderboard)
        {
            ImGui::SetNextWindowPos(ImVec2(currentPos.x + 20, currentPos.y + 20), ImGuiCond_FirstUseEver);
            ImGui::Begin("Hive Leaderboard", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoTitleBar);
            ImGui::SetWindowFontScale(uiScale);
        }
        else
        {
            // Render inline in a continuation window right after the main one
            ImGui::SetNextWindowPos(position, ImGuiCond_Always, tableAnchorPivot);
            ImGui::Begin("Hive Leaderboard Inline", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
            ImGui::SetWindowFontScale(uiScale);
        }

        std::string lbTitle = std::format("{} {} Leaderboard", getGameDisplayName(cg),
                                          monthlyLeaderboard ? "Monthly" : "All-Time");
        ImGui::Text("%s", lbTitle.c_str());

        std::vector<Hive::LeaderboardEntry> entries;
        int code = 0;
        bool fetchInProgress = false;
        {
            std::lock_guard<std::mutex> lock(leaderboardMutex);
            entries = leaderboardEntries;
            code = leaderboardCode;
            fetchInProgress = leaderboardFetchInProgress;
        }

        if (entries.empty())
        {
            if (fetchInProgress) ImGui::Text("Loading leaderboard...");
            else if (code == 1) ImGui::Text("Leaderboard unavailable for this game");
            else if (code == 2) ImGui::Text("Leaderboard data format error");
            else if (code == 3) ImGui::Text("Leaderboard API error");
            else ImGui::Text("No leaderboard data");
        }
        else
        {
            // Count columns: #, Player, Wins, Played are always shown
            int leaderboardColumns = 4;
            if (primaryExtraColumn.has_value()) leaderboardColumns++;
            if (lbShowKills) leaderboardColumns++;
            if (lbShowDeaths) leaderboardColumns++;
            if (lbShowKD) leaderboardColumns++;
            if (lbShowLevel) leaderboardColumns++;
            if (lbShowSecondaryExtra && secondaryExtraColumn.has_value()) leaderboardColumns++;

            if (ImGui::BeginTable("Leaderboard Table", leaderboardColumns,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_BordersV))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Player");
                ImGui::TableSetupColumn("Wins");
                ImGui::TableSetupColumn("Played");
                if (primaryExtraColumn.has_value()) ImGui::TableSetupColumn(primaryExtraColumn->label);
                if (lbShowKills) ImGui::TableSetupColumn("Kills");
                if (lbShowDeaths) ImGui::TableSetupColumn("Deaths");
                if (lbShowKD) ImGui::TableSetupColumn("KD");
                if (lbShowLevel) ImGui::TableSetupColumn("Level");
                if (lbShowSecondaryExtra && secondaryExtraColumn.has_value())
                    ImGui::TableSetupColumn(secondaryExtraColumn->label);
                ImGui::TableHeadersRow();

                int rows = std::min(static_cast<int>(entries.size()), leaderboardRows);
                for (int i = 0; i < rows; i++)
                {
                    const auto& entry = entries[i];
                    int rank = entry.humanIndex > 0
                                   ? entry.humanIndex
                                   : (entry.index >= 0 ? entry.index + 1 : i + 1);
                    int colIdx = 0;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(colIdx++);
                    ImGui::Text("%d", rank);
                    ImGui::TableSetColumnIndex(colIdx++);
                    ImGui::Text("%s", entry.username.c_str());
                    ImGui::TableSetColumnIndex(colIdx++);
                    if (entry.victories >= 0) ImGui::Text("%d", entry.victories);
                    else ImGui::Text("-");
                    ImGui::TableSetColumnIndex(colIdx++);
                    if (entry.played >= 0) ImGui::Text("%d", entry.played);
                    else ImGui::Text("-");

                    if (primaryExtraColumn.has_value())
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        int value = entry.getExtraStat(primaryExtraColumn->key, -1);
                        if (value >= 0) ImGui::Text("%d", value);
                        else ImGui::Text("-");
                    }
                    if (lbShowKills)
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        if (entry.kills >= 0) ImGui::Text("%d", entry.kills);
                        else ImGui::Text("-");
                    }
                    if (lbShowDeaths)
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        if (entry.deaths >= 0) ImGui::Text("%d", entry.deaths);
                        else ImGui::Text("-");
                    }
                    if (lbShowKD)
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        if (entry.kills >= 0 && entry.deaths > 0)
                        {
                            float kd = static_cast<float>(entry.kills) / static_cast<float>(entry.deaths);
                            ImGui::Text("%.2f", kd);
                        }
                        else if (entry.kills >= 0 && entry.deaths == 0)
                        {
                            ImGui::Text("%.2f", static_cast<float>(entry.kills));
                        }
                        else ImGui::Text("-");
                    }
                    if (lbShowLevel)
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        if (entry.xp >= 0) ImGui::Text("%d", Hive::getLevelFromXP(entry.xp));
                        else ImGui::Text("-");
                    }
                    if (lbShowSecondaryExtra && secondaryExtraColumn.has_value())
                    {
                        ImGui::TableSetColumnIndex(colIdx++);
                        int value = entry.getExtraStat(secondaryExtraColumn->key, -1);
                        if (value >= 0) ImGui::Text("%d", value);
                        else ImGui::Text("-");
                    }
                }
                ImGui::EndTable();
            }
        }

        ImGui::End(); // End leaderboard window
    }
}


void HiveStat::onPacketReceive(PacketEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::getServerIP().find("hive") == std::string::npos) return;
    const MinecraftPacketIds id = event.getPacket()->getId();

    if (shouldReset)
    {
        if (id == MinecraftPacketIds::ChangeDimension)
        {
            playerThreads.clear();
            playerStatsList.clear();
            queueList.clear();
            {
                std::unique_lock lock(playerTeamMutex);
                playerTeamCache.clear();
            }
            {
                std::unique_lock l(loggedMissingTeamMutex);
                loggedMissingTeamPlayers.clear();
            }
            {
                std::lock_guard lock(leaderboardMutex);
                leaderboardEntries.clear();
                leaderboardCode = 0;
                leaderboardGameId.clear();
                leaderboardMonthly = false;
                leaderboardLastFetch = std::chrono::steady_clock::time_point::min();
                leaderboardFetchInProgress = false;
            }
        }
    }
    else shouldReset = true;
}

void HiveStat::onKey(const KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (event.getKey() == Utils::getStringAsKey(getOps<std::string>("Overlay")) &&
        event.getAction() == ActionType::Released)
    {
        renderOverlay = !renderOverlay;
        if (renderOverlay)
        {
            playerThreads.clear();
            playerStatsList.clear();
            queueList.clear();
            std::lock_guard lock(leaderboardMutex);
            leaderboardEntries.clear();
            leaderboardCode = 0;
            leaderboardGameId.clear();
            leaderboardMonthly = false;
            leaderboardLastFetch = std::chrono::steady_clock::time_point::min();
            leaderboardFetchInProgress = false;
        }
    }
}

char HiveStat::extractTeamChar(const std::string& rawName)
{
    for (size_t i = 0; i + 1 < rawName.size(); ++i)
    {
        auto b = static_cast<unsigned char>(rawName[i]);
        // UTF-8 encoded section sign: 0xC2 0xA7
        if (b == 0xC2 && static_cast<unsigned char>(rawName[i + 1]) == 0xA7)
        {
            if (i + 2 < rawName.size())
                return static_cast<char>(std::tolower(static_cast<unsigned char>(rawName[i + 2])));
            return '\0';
        }
        // Raw section sign byte
        if (b == 0xA7)
        {
            if (i + 1 < rawName.size())
                return static_cast<char>(std::tolower(static_cast<unsigned char>(rawName[i + 1])));
            return '\0';
        }
    }
    return '\0';
}

ImU32 HiveStat::getHighlightColor(const std::string& playerName)
{
    int count = 0;
    auto* countSetting = settings.getSettingByName<int>("highlightCount");
    if (countSetting) count = countSetting->value;

    std::string lowerName = playerName;
    std::ranges::transform(lowerName, lowerName.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (int i = 0; i < count; i++)
    {
        std::string nameKey = "highlight-" + FlarialGUI::cached_to_string(i) + "-name";
        auto* nameSetting = settings.getSettingByName<std::string>(nameKey);
        if (!nameSetting || nameSetting->value.empty()) continue;

        std::string hlName = nameSetting->value;
        std::ranges::transform(hlName, hlName.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (lowerName == hlName)
        {
            std::string colorKey = "highlight-" + FlarialGUI::cached_to_string(i);
            auto* colSetting = settings.getSettingByName<std::string>(colorKey + "Col");
            auto* opacSetting = settings.getSettingByName<float>(colorKey + "Opacity");
            if (!colSetting) continue;

            D2D1_COLOR_F color = FlarialGUI::HexToColorF(colSetting->value);
            float opacity = opacSetting ? opacSetting->value : 1.0f;
            return IM_COL32(
                static_cast<int>(color.r * 255),
                static_cast<int>(color.g * 255),
                static_cast<int>(color.b * 255),
                static_cast<int>(opacity * 255));
        }
    }
    return 0; // No highlight found
}
