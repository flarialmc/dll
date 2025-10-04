#include "HiveStat.hpp"
#include <SDK/Client/Level/Level.hpp>
#include <GUI/Engine/EngineImGui.hpp>
#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"

#include <thread>
#include <vector>
#include <algorithm>
#include <functional>


#include "Events/Render/RenderEvent.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

void HiveStat::onEnable() {
    Listen(this, RenderEvent, &HiveStat::onRender)
    Listen(this, PacketEvent, &HiveStat::onPacketReceive)
    Listen(this, KeyEvent, &HiveStat::onKey)
    Module::onEnable();
    FlarialGUI::Notify("Hive Overlay works only in sky, bed, mm, ctf");
}

void HiveStat::onDisable() {
    stopThread = true;
    for (auto &[name, thread]: playerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    playerThreads.clear();
    Deafen(this, RenderEvent, &HiveStat::onRender)
    Deafen(this, PacketEvent, &HiveStat::onPacketReceive)
    Deafen(this, KeyEvent, &HiveStat::onKey)
    Module::onDisable();
}

void HiveStat::defaultConfig() {
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");

    setDef("uiscale", 1.f);

    setDef("Overlay", (std::string) "O");
    setDef("showFKDR", true);
    setDef("showKD", true);
    setDef("showWR", true);
    setDef("showLevel", true);
    setDef("showWins", false);
    setDef("showLosses", false);
    setDef("showKills", false);
    setDef("showDeaths", false);

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

}

void HiveStat::settingsRender(float settingsOffset) {
    initSettingsPage();

    addKeybind("Toggle Overlay Keybind", "When setting, hold the new bind for 2 seconds", "Overlay", true);
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
    addToggle("Show Deaths", "Shows the amount of deaths a player has", "showDeaths");
    extraPadding();

    addHeader("Thresholds");

    for (std::string s: std::vector<std::string>{"FKDR", "KD", "WR"}) {
        addToggle(std::format("Show {} Threshold Settings", s), "Colors are from highest threshold to lowest.\n1. Dark Blue\n2. Red\n3. Green\n4. Gold\n5. Gray", std::format("showThreshold{}", s));
        bool enabled = getOps<bool>(std::format("show{}", s)) && getOps<bool>(std::format("showThreshold{}", s));;
        for (int i = 1; i < 5; i++) {
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

    addToggle("Show Level Threshold Settings", "Colors are from highest threshold to lowest.\n1. Dark blue\n2. Red\n3. Green\n4. Magenta\n5. Aqua\n6. Yellow\n7. Gold\n8. Gray", "showThresholdLevel");
    bool c = getOps<bool>("showLevel") && getOps<bool>("showThresholdLevel");
    for (int i = 1; i < 8; i++) {
        addConditionalSliderInt(
            c,
            std::format("Level Threshold {}", i),
            "",
            std::format("threshold-Level-{}", i),
            100,
            0
        );
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HiveStat::fetchPlayerStats(const std::string &playerName) {
    std::string cg = HiveModeCatcherListener::currentGame;

    size_t dashPos = cg.find('-');

    if (dashPos != std::string::npos) {
        cg = cg.substr(0, dashPos);
    }

    std::transform(cg.begin(), cg.end(), cg.begin(), ::tolower);
    Hive::PlayerStats stats = Hive::GetStats(cg, playerName);
    std::unique_lock<std::shared_mutex> lock(queueMutex);
    playerStatsList.emplace_back(playerName, stats);
}

void HiveStat::onRender(RenderEvent &event) {

    float uiScale = getOps<float>("uiscale");

    const float width  = 150.0f;
    const float height = 50.0f;

    float scaledWidth  = width  * uiScale;
    float scaledHeight = height * uiScale;

    if (ClientInstance::getTopScreenName() == "hud_screen" && this->isEnabled()) {

        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"),
                                              getOps<float>("percentageY"));

        if (settingperc.x != 0.0f)
            currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width),
                                     settingperc.y * (MC::windowSize.y - height));
        else
            currentPos = Constraints::CenterConstraint(width, height);

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, scaledWidth, scaledHeight, 123, this->name);
            FlarialGUI::FlarialTextWithFont(
                currentPos.x, currentPos.y,
                L"Overlay", scaledWidth, scaledHeight,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                Constraints::SpacingConstraint(1.05f, scaledWidth),
                DWRITE_FONT_WEIGHT_NORMAL
            );
        }

        Vec2<float> vec2;
        vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 123, scaledWidth, scaledHeight);

        currentPos.x = vec2.x;
        currentPos.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
    }

    if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;

    if (!renderOverlay) return;

    std::string cg = HiveModeCatcherListener::currentGame;

    size_t dashPos = cg.find('-');
    if (dashPos != std::string::npos) cg = cg.substr(0, dashPos);

    std::transform(cg.begin(), cg.end(), cg.begin(), ::tolower);
    if (cg != "bed" && cg != "sky" && cg != "murder" && cg != "ctf") return;

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() ||
        SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel()) {
        return;
    }

    ImVec2 position(currentPos.x, currentPos.y);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always);

    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowFontScale(uiScale);

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
    if (getOps<bool>("showDeaths") == true)
        columnAmount++;

    if (ImGui::BeginTable("Player Stats", columnAmount,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV)) {
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
        if (getOps<bool>("showDeaths") == true)
            ImGui::TableSetupColumn("Deaths");

        ImGui::TableHeadersRow();

        for (const auto &pair : SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {
            if (pair.second.name.empty()) continue;
            std::string name = String::removeNonAlphanumeric(String::removeColorCodes(pair.second.name));

            auto it = std::find_if(playerStatsList.begin(), playerStatsList.end(),
                                   [&name](const std::pair<std::string, Hive::PlayerStats> &element) {
                                       return element.first == name;
                                   });

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", name.c_str());

            if (columnAmount > 1) {
                int columnIndex = 1;

                ImGui::TableSetColumnIndex(columnIndex);
                if (it != playerStatsList.end()) {
                    if (it->second.getCode() == 0) {
                        if (getOps<bool>("showFKDR") == true && cg == "bed") {

                            std::ostringstream oss;
                            float fkdr = it->second.getFKDR();
                            oss << std::fixed << std::setprecision(2) << fkdr;

                            if (fkdr >= getOps<float>("threshold-FKDR-1")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (fkdr >= getOps<float>("threshold-FKDR-2")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (fkdr >= getOps<float>("threshold-FKDR-3")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (fkdr >= getOps<float>("threshold-FKDR-4")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showKD") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            float kd = it->second.getKD();
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(2) << kd;

                            if (kd >= getOps<float>("threshold-KD-1")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (kd >= getOps<float>("threshold-KD-2")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (kd >= getOps<float>("threshold-KD-3")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (kd >= getOps<float>("threshold-KD-4")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showWR") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            float winrate = it->second.getWinRate();
                            oss << std::fixed << std::setprecision(0) << winrate << "%";

                            if (winrate >= getOps<float>("threshold-WR-1")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (winrate >= getOps<float>("threshold-WR-2")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (winrate >= getOps<float>("threshold-WR-3")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (winrate >= getOps<float>("threshold-WR-4")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showLevel") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int level = it->second.getLevel();
                            oss << std::fixed << std::setprecision(0) << level;

                            if (level >= getOps<int>("threshold-Level-1")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (level >= getOps<int>("threshold-Level-2")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (level >= getOps<int>("threshold-Level-3")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (level >= getOps<int>("threshold-Level-4")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            } else if (level >= getOps<int>("threshold-Level-5")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            } else if (level >= getOps<int>("threshold-Level-6")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            } else if (level >= getOps<int>("threshold-Level-7")) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s%s", it->second.getPrestige(), oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showWins") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int victories = it->second.getVictories();
                            oss << std::fixed << std::setprecision(0) << victories;

                            if (victories >= 10000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (victories >= 8000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (victories >= 5000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (victories >= 3000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            } else if (victories >= 1000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            } else if (victories >= 500) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            } else if (victories >= 100) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showLosses") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int losses = it->second.getLosses();
                            oss << std::fixed << std::setprecision(0) << losses;

                            if (losses >= 10000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_red"));
                            } else if (losses >= 8000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (losses >= 5000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("orange"));
                            } else if (losses >= 3000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("olive"));
                            } else if (losses >= 1000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (losses >= 500) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_aqua"));
                            } else if (losses >= 100) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("light_gray"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showKills") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int kills = it->second.getKills();
                            oss << std::fixed << std::setprecision(0) << kills;

                            if (kills >= 10000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (kills >= 8000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (kills >= 5000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (kills >= 3000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("magenta"));
                            } else if (kills >= 1000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("aqua"));
                            } else if (kills >= 500) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("yellow"));
                            } else if (kills >= 100) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gold"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                            columnIndex++;
                        }

                        if (getOps<bool>("showDeaths") == true) {

                            ImGui::TableSetColumnIndex(columnIndex);
                            std::ostringstream oss;
                            int deaths = it->second.getDeaths();
                            oss << std::fixed << std::setprecision(0) << deaths;

                            if (deaths >= 20000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_blue"));
                            } else if (deaths >= 10000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_red"));
                            } else if (deaths >= 8000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("red"));
                            } else if (deaths >= 5000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("orange"));
                            } else if (deaths >= 3000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("olive"));
                            } else if (deaths >= 1000) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("green"));
                            } else if (deaths >= 500) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("dark_aqua"));
                            } else if (deaths >= 100) {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("light_gray"));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, colorMap.at("gray"));
                            }

                            ImGui::Text("%s", oss.str().c_str());
                            ImGui::PopStyleColor();
                        }

                    } else {
                        switch (it->second.getCode()) {
                            case 1:

                                ImGui::Text("err");
                                for (int col = 2; col < columnAmount; ++col) {
                                    ImGui::TableSetColumnIndex(col);
                                    if (col == 2) ImGui::Text("nick");
                                    else ImGui::Text("-");
                                }
                                break;

                            case 2:

                                ImGui::Text("err");
                                for (int col = 2; col < columnAmount; ++col) {
                                    ImGui::TableSetColumnIndex(col);
                                    if (col == 2) ImGui::Text("missing data");
                                    else ImGui::Text("-");
                                }
                                break;

                            case 3:

                                ImGui::Text("err");
                                for (int col = 2; col < columnAmount; ++col) {
                                    ImGui::TableSetColumnIndex(col);
                                    if (col == 2) ImGui::Text("API DEAD :skull:");
                                    else ImGui::Text("-");
                                }
                                break;

                        }
                    }
                } else {
                    std::unique_lock<std::shared_mutex> lock(queueMutex);
                    if (std::find(queueList.begin(), queueList.end(), name) == queueList.end()) {
                        queueList.push_back(name);
                        playerThreads[name] = std::thread([this, name]() {
                            fetchPlayerStats(name);
                        });
                        playerThreads[name].detach();
                    }
                    ImGui::Text("Loading stats...");
                    for (int col = 3; col < columnAmount; ++col) {
                        ImGui::TableSetColumnIndex(col);
                        ImGui::Text("-");
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();
}


void HiveStat::onPacketReceive(PacketEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::getServerIP().find("hive") == std::string::npos) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    if (shoudReset) {
        if (id == MinecraftPacketIds::ChangeDimension) {
            //std::unique_lock<std::shared_mutex> lock(queueMutex);
            playerThreads.clear();
            playerStatsList.clear();
            queueList.clear();
        }
    }
    else {
        shoudReset = true;
    }
}

void HiveStat::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (event.getKey() == Utils::getStringAsKey(getOps<std::string>("Overlay")) &&
        static_cast<ActionType>(event.getAction()) == ActionType::Released) {
        renderOverlay = !renderOverlay;
        if (renderOverlay) {
            playerThreads.clear();
            playerStatsList.clear();
            queueList.clear();
        }
    }

}
