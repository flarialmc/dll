#include "HiveUtils.hpp"

#include "Client.hpp"
#include "Utils/Logger/Logger.hpp"
#include "Utils/Utils.hpp"

#include "SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "SDK/Client/Network/Packet/ModalFormRequestPacket.hpp"
#include "SDK/Client/Network/Packet/ModalFormResponsePacket.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/HiveMaps.hpp"
#include <json/json.hpp>
#include <set>

// For custom inline button rendering
#include "../ClickGUI/ClickGUI.hpp"
#include "Client/GUI/Engine/Engine.hpp"
#include "Client/GUI/Engine/Constraints.hpp"

void HiveUtils::onEnable()
{
    Listen(this, PacketEvent, &HiveUtils::onPacketReceive)
    Listen(this, KeyEvent, &HiveUtils::onKey)
    Module::onEnable();
}

void HiveUtils::onDisable()
{
    Deafen(this, PacketEvent, &HiveUtils::onPacketReceive)
    Deafen(this, KeyEvent, &HiveUtils::onKey)
    Module::onDisable();
}

void HiveUtils::defaultConfig()
{
    getKeybind();
    Module::defaultConfig("core");
    setDef("map", (std::string)"");
    setDef("ReQ", true);
    setDef("solo", false);
    setDef("eliminated", true);
    setDef("hub", false);
    setDef("murderer", false);
    setDef("sheriff", false);
    setDef("innocent", false);
    setDef("hider", false);
    setDef("seeker", false);
    setDef("death", false);
    setDef("runner", false);
    setDef("AutoMapAvoider", false);
    setDef("replace", true);
    setDef("copyCS", false);
    setDef("includecommand", false);
    setDef("promomessage", false);
    setDef("unlocks", false);
    setDef("joined", false);
    setDef("playermessage", false);
    setDef("playermessageplus", false);
    setDef("noteaming", false);
    setDef("friendaccept", false);
    setDef("partyaccept", false);
    setDef("deathcountenabled", false);
    setDef("deathcount", 5);

    // Auto Map Vote settings :3
    setDef("autoMapVote", false);
    setDef("autoMapVoteNotify", true);
    setDef("announceVote", false);
    setDef("announceVoteMessage", (std::string)"@here vote for {map}!");
    setDef("mapvote_selected_game", (std::string)"BedWars"); // Default game for settings UI
    // Map preferences are created on-demand when the user adds maps for each game+variant
}

void HiveUtils::settingsRender(float settingsOffset)
{
    initSettingsPage();
    addHeader("General");
    addToggle("Use /hub instead of /q", "", "hub");
    // this->addDropdown("Command to use", "Command to execute when somthing gets triggered",  std::vector<std::string>{"Re-Q same game", "Q a Random game", "Go back to the hub"}, getOps<std::string>("commandtouse"));

    addHeader("Auto Re Q");
    addToggle("Auto re-queue ", "Find a new game when the current game is over", "ReQ");
    addToggle("Solo mode ",
              "Re-Q when you finish a game or die and can't respawn.\nNot recomended while in a party.",
              "solo");
    addToggle("Team Elimination", "Re-Q when the team your on is fully ELIMINATED.", "eliminated");
    addKeybind("Requeue Keybind", "", "keybind", true);

    addHeader("Map Avoider");

    addToggle("Map Avoider",
              "Automatically finds you a new game when a specific map has won the vote",
              "AutoMapAvoider");

    // Map avoider uses the same game/variant selection and map list as auto vote
    if (getOps<bool>("AutoMapAvoider")) {
        // Base game options (same as auto vote)
        static const std::vector<std::string> avoiderGameOptions = {
            "BedWars", "SkyWars", "Treasure Wars", "Survival Games",
            "Death Run", "Hide and Seek", "Murder Mystery",
            "Capture The Flag", "Block Drop", "Ground Wars",
            "Build Battle", "The Bridge", "Gravity"
        };
        static const std::vector<std::string> avoiderGameIds = {
            "bed", "sky", "wars", "sg",
            "dr", "hide", "murder",
            "ctf", "drop", "ground",
            "build", "bridge", "grav"
        };
        static const std::map<std::string, std::vector<std::string>> avoiderGameVariants = {
            {"bed", {"Solos", "Duos", "Squads", "Mega"}},
            {"sky", {"Solos", "Duos", "Squads", "Mega", "Royale"}},
            {"wars", {"Solos", "Duos", "Squads", "Mega"}},
            {"sg", {"Solos", "Duos"}}
        };

        setDef("mapavoid_selected_game", (std::string)"BedWars");
        addDropdown("Game", "Select game to configure avoided maps",
                    avoiderGameOptions, "mapavoid_selected_game", false);

        // Get the selected base game
        std::string selectedAvoiderBase = "bed";
        auto* selectedAvoiderGameSetting = settings.getSettingByName<std::string>("mapavoid_selected_game");
        if (selectedAvoiderGameSetting) {
            for (size_t i = 0; i < avoiderGameOptions.size(); i++) {
                if (avoiderGameOptions[i] == selectedAvoiderGameSetting->value) {
                    selectedAvoiderBase = avoiderGameIds[i];
                    break;
                }
            }
        }

        // Show variant dropdown if this game has variants
        auto avoiderVariantIt = avoiderGameVariants.find(selectedAvoiderBase);
        if (avoiderVariantIt != avoiderGameVariants.end()) {
            setDef("mapavoid_selected_variant", (std::string)"Solos");
            addDropdown("Variant", "Select game variant",
                        avoiderVariantIt->second, "mapavoid_selected_variant", false);

            auto* avoiderVariantSetting = settings.getSettingByName<std::string>("mapavoid_selected_variant");
            std::string avoiderVariant = avoiderVariantSetting ? avoiderVariantSetting->value : "Solos";
            std::string avoiderVariantLower = avoiderVariant;
            std::transform(avoiderVariantLower.begin(), avoiderVariantLower.end(), avoiderVariantLower.begin(), ::tolower);
            selectedGameForAvoider = selectedAvoiderBase + "-" + avoiderVariantLower;
        } else {
            selectedGameForAvoider = selectedAvoiderBase;
        }

        // Refresh maps button
        std::string avoiderApiGameId = HiveMaps::gameIdToApiFormat(selectedGameForAvoider);
        bool avoiderRefreshing = HiveMaps::isRefreshInProgress(avoiderApiGameId);
        addButton(avoiderRefreshing ? "Refreshing..." : "Refresh Maps",
                  avoiderRefreshing ? "Fetching maps from API in background" : "Fetch the latest maps from Hive API",
                  avoiderRefreshing ? "..." : "Refresh", [this] {
            std::string apiGameId = HiveMaps::gameIdToApiFormat(selectedGameForAvoider);
            if (!HiveMaps::isRefreshInProgress(apiGameId)) {
                refreshMapsForAvoider(selectedGameForAvoider);
                FlarialGUI::Notify("Refreshing maps in background...");
            }
        });

        // If we haven't fetched maps yet, trigger a fetch
        // Only trigger a NEW async refresh when the game changes or cache is empty (not during/after refresh)
        bool avoiderJustCompleted = wasAvoiderRefreshing && !avoiderRefreshing;
        wasAvoiderRefreshing = avoiderRefreshing;
        if (lastFetchedAvoiderGame != selectedGameForAvoider || currentAvoiderMaps.empty()) {
            // Game changed or no maps - need to refresh
            refreshMapsForAvoider(selectedGameForAvoider);
        } else if (avoiderJustCompleted) {
            // Refresh just completed - only update local maps from cache, don't trigger new refresh
            updateAvoiderMapsFromCache(selectedGameForAvoider);
        }

        // Get current avoided maps
        auto avoidedMaps = getAvoidedMaps(selectedGameForAvoider);

        extraPadding();
        addHeader("Avoided Maps");

        if (avoidedMaps.empty()) {
            addElementText("No avoided maps set", "Add maps from the list below");
        } else {
            // Show avoided maps with Remove button
            for (size_t i = 0; i < avoidedMaps.size(); i++) {
                std::string mapName = avoidedMaps[i];
                addButton(mapName, "Click to remove from avoided maps", "Remove", [this, mapName] {
                    removeAvoidedMap(selectedGameForAvoider, mapName);
                });
            }
        }

        // Show available maps that aren't avoided yet
        extraPadding();
        addHeader("Available Maps");

        std::set<std::string> avoidedSet(avoidedMaps.begin(), avoidedMaps.end());

        if (!currentAvoiderMaps.empty()) {
            for (const auto& map : currentAvoiderMaps) {
                if (avoidedSet.find(map.name) == avoidedSet.end()) {
                    addButton(map.name, "Click to add to avoided maps", "Avoid", [this, mapName = map.name] {
                        addAvoidedMap(selectedGameForAvoider, mapName);
                    });
                }
            }
        } else {
            addElementText("No maps loaded", "Click 'Refresh Maps' above to fetch from API");
        }
    }

    addHeader("Murder Mystery");
    addToggle("Murderer", "Re Q when you get murderer", "murderer");
    addToggle("Sheriff", "Re Q when you get sheriff", "sheriff");
    addToggle("Innocent", "Re Q when you get innocent", "innocent");

    addHeader("Hide and Seek");
    addToggle("Hider", "Re Q when you get hider", "hider");
    addToggle("Seeker", "Re Q when you get seeker", "seeker");

    addHeader("DeathRun");
    addToggle("Death Limiter", "Re Q after specified amount of deaths", "deathcountenabled");
    addConditionalSliderInt(getOps<bool>("deathcountenabled"), "Death Limiter: Amount of Deaths",
                            "Configure the amount of deaths required here.", "deathcount", 100, 1);
    addToggle("Death", "Re Q when you get death", "death");
    addToggle("Runner", "Re Q when you get runner", "runner");

    addHeader("Copy Custom Server code");
    addToggle("Copy CS code", "Automatically copy custom server code to your clipboard", "copyCS");
    addToggle("Include command", "Include /cs when copying custom server code", "includecommand");

    addHeader("Debloat chat");
    addToggle("Promo message", "Removes all promo/info messages starting with [!]", "promomessage");
    addToggle("Unused Unlocks", "Removes 'You have unused unlocks in your Locker!'", "unlocks");
    addToggle("Player joined", "Removes player joined message", "joined");
    addToggle("Player message", "Removes all messages by non ranked players", "playermessage");
    addToggle("Player message Hive+", "Removes all messages from Hive+ players",
              "playermessageplus");
    addToggle("No Teaming", "Removes No Teaming!", "noteaming");

    addHeader("Auto accept");
    addToggle("Friend request", "Automatically accept incoming friend requests.", "friendaccept");
    addToggle("Party request", "Automatically accept incoming party requests.", "partyaccept");

    addHeader("Auto Map Vote");
    addToggle("Enable Auto Map Vote",
              "Automatically vote for your preferred map when the vote modal appears",
              "autoMapVote");
    addConditionalToggle(getOps<bool>("autoMapVote"), "Show Notification",
              "Show a notification when auto-voting for a map",
              "autoMapVoteNotify");

    addToggle("Announce Vote",
              "Automatically send a chat message when you (or auto-vote) vote for a map",
              "announceVote");
    addConditionalTextBox(getOps<bool>("announceVote"),
              "Announcement Message", "Use {map} as placeholder for the map name",
              64, "announceVoteMessage");

    // Game selector dropdown for map preferences :3
    if (getOps<bool>("autoMapVote")) {
        // Base game options (no variants in main dropdown)
        static const std::vector<std::string> gameOptions = {
            "BedWars", "SkyWars", "Treasure Wars", "Survival Games",
            "Death Run", "Hide and Seek", "Murder Mystery",
            "Capture The Flag", "Block Drop", "Ground Wars",
            "Build Battle", "The Bridge", "Gravity"
        };
        // Base game IDs
        static const std::vector<std::string> gameIds = {
            "bed", "sky", "wars", "sg",
            "dr", "hide", "murder",
            "ctf", "drop", "ground",
            "build", "bridge", "grav"
        };
        // Which games have variants (and their variant options)
        static const std::map<std::string, std::vector<std::string>> gameVariants = {
            {"bed", {"Solos", "Duos", "Squads", "Mega"}},
            {"sky", {"Solos", "Duos", "Squads", "Mega", "Royale"}},
            {"wars", {"Solos", "Duos", "Squads", "Mega"}},
            {"sg", {"Solos", "Duos"}}
        };

        addDropdown("Game", "Select game to configure",
                    gameOptions, "mapvote_selected_game", false);

        // Get the selected base game
        std::string selectedBaseGame = "bed";
        auto* selectedGameSetting = settings.getSettingByName<std::string>("mapvote_selected_game");
        if (selectedGameSetting) {
            for (size_t i = 0; i < gameOptions.size(); i++) {
                if (gameOptions[i] == selectedGameSetting->value) {
                    selectedBaseGame = gameIds[i];
                    break;
                }
            }
        }

        // Show variant dropdown if this game has variants
        auto variantIt = gameVariants.find(selectedBaseGame);
        if (variantIt != gameVariants.end()) {
            // Ensure variant setting exists
            setDef("mapvote_selected_variant", (std::string)"Solos");

            addDropdown("Variant", "Select game variant",
                        variantIt->second, "mapvote_selected_variant", false);

            // Build full game ID from base + variant
            auto* variantSetting = settings.getSettingByName<std::string>("mapvote_selected_variant");
            std::string variant = variantSetting ? variantSetting->value : "Solos";
            std::string variantLower = variant;
            std::transform(variantLower.begin(), variantLower.end(), variantLower.begin(), ::tolower);
            selectedGameForPrefs = selectedBaseGame + "-" + variantLower;
        } else {
            // No variants for this game
            selectedGameForPrefs = selectedBaseGame;
        }

        // Refresh button
        std::string prefsApiGameId = HiveMaps::gameIdToApiFormat(selectedGameForPrefs);
        bool prefsRefreshing = HiveMaps::isRefreshInProgress(prefsApiGameId);
        addButton(prefsRefreshing ? "Refreshing..." : "Refresh Maps",
                  prefsRefreshing ? "Fetching maps from API in background" : "Fetch the latest maps from Hive API",
                  prefsRefreshing ? "..." : "Refresh", [this] {
            std::string apiGameId = HiveMaps::gameIdToApiFormat(selectedGameForPrefs);
            if (!HiveMaps::isRefreshInProgress(apiGameId)) {
                refreshMapsForGame(selectedGameForPrefs);
                FlarialGUI::Notify("Refreshing maps in background...");
            }
        });

        // Get current preferences
        auto prefs = getMapPreferences(selectedGameForPrefs);

        // If we haven't fetched maps yet, trigger a fetch
        // Only trigger a NEW async refresh when the game changes or cache is empty (not during/after refresh)
        bool prefsJustCompleted = wasPrefsRefreshing && !prefsRefreshing;
        wasPrefsRefreshing = prefsRefreshing;
        if (lastFetchedGame != selectedGameForPrefs || currentGameMaps.empty()) {
            // Game changed or no maps - need to refresh
            refreshMapsForGame(selectedGameForPrefs);
        } else if (prefsJustCompleted) {
            // Refresh just completed - only update local maps from cache, don't trigger new refresh
            updatePrefsMapsFromCache(selectedGameForPrefs);
        }

        extraPadding();
        addHeader("Your Map Preferences");

        if (prefs.empty()) {
            addElementText("No preferred maps set", "Add maps from the list below");
        } else {
            // Show preferred maps with action dropdown
            for (size_t i = 0; i < prefs.size(); i++) {
                std::string mapName = prefs[i];
                std::string label = "#" + std::to_string(i + 1) + ": " + mapName;
                std::string actionKey = "mapvote_action_" + selectedGameForPrefs + "_" + std::to_string(i);

                // Build action options based on position
                std::vector<std::string> actions;
                actions.push_back("Choose action");
                if (i > 0) actions.push_back("Move Up");
                if (i < prefs.size() - 1) actions.push_back("Move Down");
                actions.push_back("Remove");

                // Ensure the setting exists with default value
                setDef(actionKey, (std::string)"Choose action");

                addDropdown(label, "", actions, actionKey, false);

                // Check if an action was selected
                auto* actionSetting = settings.getSettingByName<std::string>(actionKey);
                if (actionSetting && actionSetting->value != "Choose action") {
                    std::string action = actionSetting->value;
                    actionSetting->value = "Choose action"; // Reset immediately

                    auto currentPrefs = getMapPreferences(selectedGameForPrefs);
                    if (action == "Move Up" && i > 0) {
                        std::swap(currentPrefs[i], currentPrefs[i - 1]);
                        setMapPreferences(selectedGameForPrefs, currentPrefs);
                    } else if (action == "Move Down" && i < currentPrefs.size() - 1) {
                        std::swap(currentPrefs[i], currentPrefs[i + 1]);
                        setMapPreferences(selectedGameForPrefs, currentPrefs);
                    } else if (action == "Remove") {
                        currentPrefs.erase(
                            std::remove(currentPrefs.begin(), currentPrefs.end(), mapName),
                            currentPrefs.end()
                        );
                        setMapPreferences(selectedGameForPrefs, currentPrefs);
                    }
                }
            }
        }

        // Show available maps that aren't in preferences yet
        extraPadding();
        addHeader("Available Maps");

        std::set<std::string> prefsSet(prefs.begin(), prefs.end());

        if (!currentGameMaps.empty()) {
            for (const auto& map : currentGameMaps) {
                if (prefsSet.find(map.name) == prefsSet.end()) {
                    addButton(map.name, "Click to add to your preferences", "Add", [this, mapName = map.name] {
                        auto currentPrefs = getMapPreferences(selectedGameForPrefs);
                        currentPrefs.push_back(mapName);
                        setMapPreferences(selectedGameForPrefs, currentPrefs);
                    });
                }
            }
        } else {
            addElementText("No maps loaded", "Click 'Refresh Maps' above to fetch from API");
        }
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HiveUtils::onPacketReceive(PacketEvent& event)
{
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    // Handle auto map vote :3
    if (id == MinecraftPacketIds::ShowModalForm && getOps<bool>("autoMapVote")) {
        // Direct cast like ZeqaUtils does - simpler and known to work
        auto* pkt = reinterpret_cast<ModalFormRequestPacket*>(event.getPacket());

        // Try to parse JSON
        if (!nlohmann::json::accept(pkt->mFormJSON)) {
            return;
        }

        auto json = nlohmann::json::parse(pkt->mFormJSON);

        // Check title
        if (!json.contains("title") || !json["title"].is_string()) {
            return;
        }

        std::string title = json["title"].get<std::string>();
        // Strip color codes from title before comparing :3
        std::string cleanTitle = String::removeColorCodes(title);

        // Case-insensitive check for "choose map"
        std::string lowerTitle = cleanTitle;
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
        // Trim whitespace
        while (!lowerTitle.empty() && std::isspace(lowerTitle.front())) lowerTitle.erase(0, 1);
        while (!lowerTitle.empty() && std::isspace(lowerTitle.back())) lowerTitle.pop_back();

        if (lowerTitle != "choose map") {
            return;
        }

        // Parse buttons
        if (!json.contains("buttons") || !json["buttons"].is_array()) {
            return;
        }

        std::vector<std::string> mapNames;
        for (const auto& button : json["buttons"]) {
            if (button.contains("text") && button["text"].is_string()) {
                std::string buttonText = button["text"].get<std::string>();
                // Clean up the button text (remove vote counts, etc.)
                size_t newlinePos = buttonText.find('\n');
                if (newlinePos != std::string::npos) {
                    buttonText = buttonText.substr(0, newlinePos);
                }
                // Remove color codes
                std::string cleanName = String::removeColorCodes(buttonText);
                // Trim whitespace
                while (!cleanName.empty() && std::isspace(cleanName.front())) cleanName.erase(0, 1);
                while (!cleanName.empty() && std::isspace(cleanName.back())) cleanName.pop_back();

                if (!cleanName.empty()) {
                    mapNames.push_back(cleanName);
                }
            }
        }

        if (mapNames.empty()) {
            return;
        }

        // Find the best map to vote for
        std::string currentGame = HiveModeCatcherListener::currentGame;
        auto preferredIndex = findPreferredMapIndex(mapNames, currentGame);

        if (preferredIndex.has_value()) {
            int idx = preferredIndex.value();
            std::string votedMap = idx < static_cast<int>(mapNames.size()) ? mapNames[idx] : "Unknown";

            // Send the response
            sendModalResponse(pkt->mFormId, idx);

            // Cancel the modal so it doesn't show
            event.cancel();

            // Notify user
            if (getOps<bool>("autoMapVoteNotify")) {
                FlarialGUI::Notify("Auto-voted for: " + votedMap);
            }

            return;
        }
    }

    if (id == MinecraftPacketIds::SetTitle)
    {
        auto* pkt = reinterpret_cast<SetTitlePacket*>(event.getPacket());
        if (getOps<bool>("ReQ"))
        {
            if (getOps<bool>("solo"))
            {
                if ( //pkt->text == "§cYou're a spectator!" || //this brobably isn't needed anymore
                    pkt->text == "§cYou died!" ||
                    pkt->text == "§7You're spectating the §as§eh§6o§cw§7!")
                {
                    reQ();
                }
            }
        }
        if (getOps<bool>("copyCS"))
        {
            if (!pkt->text.empty() && pkt->text.find(Copy_CS) == std::string::npos)
            {
                if (pkt->text.length() >= 15 && pkt->text.substr(0, 13) == "§eJoin Code:")
                {
                    Copy_CS = pkt->text;
                    if (!getOps<bool>("includecommand"))
                    {
                        ImGui::SetClipboardText(pkt->text.substr(17, pkt->text.length()).c_str());
                    }
                    else
                    {
                        ImGui::SetClipboardText(
                            ("/cs " + pkt->text.substr(17, pkt->text.length())).c_str());
                    }
                    FlarialGUI::Notify("Compied CS Code to Clipboard");
                }
            }
        }
    }
    if (id == MinecraftPacketIds::Text)
    {
        auto pktOpt = getTextPacket(event.getPacket());
        if (!pktOpt) return;
        auto& pkt = *pktOpt;

        // Announce vote feature :3
        // Hive sends "§b§l» §rYou voted for MapName" when you vote for a map
        if (getOps<bool>("announceVote")) {
            static const std::string votePrefix = "You voted for ";
            // Strip color codes to find the message
            std::string cleanMsg;
            for (size_t i = 0; i < pkt.message.size(); i++) {
                if (pkt.message[i] == '\xC2' && i + 1 < pkt.message.size() && pkt.message[i + 1] == '\xA7') {
                    i += 2; // Skip §X (UTF-8 encoded)
                } else if (pkt.message[i] == '\xA7' && i + 1 < pkt.message.size()) {
                    i += 1; // Skip §X
                } else {
                    cleanMsg += pkt.message[i];
                }
            }
            // Check if message starts with "» You voted for "
            if (cleanMsg.find("\xC2\xBB You voted for ") == 0 || cleanMsg.find("» You voted for ") == 0) {
                // Extract map name (after "» You voted for ")
                size_t prefixLen = (cleanMsg.find("» ") == 0) ? 2 : 0;
                prefixLen += votePrefix.length();
                if (cleanMsg.length() > prefixLen) {
                    std::string mapName = cleanMsg.substr(prefixLen);
                    // Trim any trailing whitespace
                    while (!mapName.empty() && std::isspace(mapName.back())) mapName.pop_back();

                    if (!mapName.empty()) {
                        // Get the announcement message template
                        std::string announcement = getOps<std::string>("announceVoteMessage");
                        // Replace {map} placeholder with actual map name
                        size_t pos = announcement.find("{map}");
                        while (pos != std::string::npos) {
                            announcement.replace(pos, 5, mapName);
                            pos = announcement.find("{map}", pos + mapName.length());
                        }

                        // Send the chat message
                        auto player = SDK::clientInstance->getLocalPlayer();
                        if (player) {
                            std::shared_ptr<Packet> chatPacket = SDK::createPacket(static_cast<int>(MinecraftPacketIds::Text));
                            craftChatPacket(chatPacket.get(), player->getPlayerName(), announcement);
                            SDK::clientInstance->getPacketSender()->sendToServer(chatPacket.get());
                        }
                    }
                }
            }
        }

        if (HiveModeCatcherListener::currentGame == "DR" and
        pkt.message == "§a§l» §r§bThe game has started! Run!"
        )
        deaths = 0;

        if (getOps<bool>("deathcountenabled") and HiveModeCatcherListener::currentGame
        ==
        "DR"
        and
        pkt.message == "§c§l» §r§cYou died!"
        )
        {
            deaths++;
            if (deaths > getOps<int>("deathcount"))
            {
                reQ();
                FlarialGUI::Notify("Death count limit reached.");
                deaths = 0;
            }
        }
        if (getOps<bool>("ReQ"))
        {
            //if(!module->getOps<bool>("solo")) {
            if (pkt.message == "§c§l» §r§c§lGame OVER!")
            {
                reQ();
                return;
            }
            if (getOps<bool>("eliminated"))
            {
                if (pkt.message.length() > 27)
                {
                    if (pkt.message.substr(12, 15) == "You are on the ")
                    {
                        teamcolor = pkt.message.substr(27, pkt.message.length() - 28);
                    }
                }

                if (pkt.message.find("§7has been §cELIMINATED§7!") != std::string::npos && pkt.
                    message.find(teamcolor) != std::string::npos && !teamcolor.empty())
                {
                    reQ();
                    FlarialGUI::Notify("Your Team has been ELIMINATED");
                    return;
                }
            }
            if (getOps<bool>("solo"))
            {
                if (pkt.message.substr(0, 48) == "§a§l» §r§eYou finished all maps and came in" ||
                    //gravity
                    pkt.message.substr(0, 30) == "§a§l» §r§eYou finished in")
                {
                    //deathrun
                    reQ();
                }
            }
        }
        if (getOps<bool>("murderer"))
        {
            if (pkt.message == "§c§l» §r§c§lMurderer")
            {
                reQ();
                FlarialGUI::Notify("Found role Murderer");
            }
        }
        if (getOps<bool>("sheriff"))
        {
            if (pkt.message == "§9§l» §r§9§lSheriff")
            {
                reQ();
                FlarialGUI::Notify("Found role Sheriff");
            }
        }
        if (getOps<bool>("innocent"))
        {
            if (pkt.message == "§a§l» §r§a§lInnocent")
            {
                reQ();
                FlarialGUI::Notify("Found role Innocent");
            }
        }
        if (getOps<bool>("death"))
        {
            if (pkt.message == "§d§l» §r§bYou are a §cDeath")
            {
                reQ();
                FlarialGUI::Notify("Found role Death");
            }
        }
        if (getOps<bool>("runner"))
        {
            if (pkt.message == "§d§l» §r§bYou are a §aRunner")
            {
                reQ();
                FlarialGUI::Notify("Found role Runner");
            }
        }
        if (getOps<bool>("hider"))
        {
            if (pkt.message == "§e§l» §rYou are a §eHIDER")
            {
                reQ();
                FlarialGUI::Notify("Found role Hider");
            }
        }
        if (getOps<bool>("seeker"))
        {
            if (pkt.message == "§c§l» §rYou are a §cSEEKER")
            {
                reQ();
                FlarialGUI::Notify("Found role Seeker");
            }
        }
        if (getOps<bool>("AutoMapAvoider"))
        {
            // Get current game mode to find avoided maps for this game
            std::string currentGame = HiveModeCatcherListener::currentGame;
            if (!currentGame.empty()) {
                // Normalize game ID (e.g., "BED-DUOS" -> "bed-duos")
                std::string gameKey = currentGame;
                std::transform(gameKey.begin(), gameKey.end(), gameKey.begin(), ::tolower);

                // Get avoided maps for this game
                auto avoidedMaps = getAvoidedMaps(gameKey);

                // If no avoided maps found and game has no variant suffix, try common variants
                // (Settings UI uses "bed-solos" format, but server sends "BED" for solo modes)
                if (avoidedMaps.empty() && gameKey.find('-') == std::string::npos) {
                    avoidedMaps = getAvoidedMaps(gameKey + "-solos");
                    if (avoidedMaps.empty()) {
                        avoidedMaps = getAvoidedMaps(gameKey + "-duos");
                    }
                    if (avoidedMaps.empty()) {
                        avoidedMaps = getAvoidedMaps(gameKey + "-squads");
                    }
                }

                if (!avoidedMaps.empty()) {
                    // Message format: §b§l» §r§eCitadel §7won with §f0 §7votes!
                    // We need to check if this is a "won with" message and extract the map name
                    std::string message = pkt.message.data();
                    std::string lowerMessage = message;
                    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);

                    // Check if this is a map win message (contains "won with" pattern)
                    if (lowerMessage.find("won with") != std::string::npos) {
                        // Look for the map name after §r§e and before §7won
                        // The prefix is "§b§l» §r§e"
                        static const std::string mapWinPrefix = "§b§l» §r§e";
                        size_t mapStart = message.find(mapWinPrefix);
                        if (mapStart != std::string::npos) {
                            std::string afterPrefix = message.substr(mapStart + mapWinPrefix.length());
                            // Map name ends at the next § (which starts "§7won")
                            size_t endPos = afterPrefix.find("§");
                            std::string winningMap = (endPos != std::string::npos)
                                ? afterPrefix.substr(0, endPos)
                                : afterPrefix;
                            // Trim whitespace
                            while (!winningMap.empty() && std::isspace(winningMap.front())) winningMap.erase(0, 1);
                            while (!winningMap.empty() && std::isspace(winningMap.back())) winningMap.pop_back();

                            if (!winningMap.empty()) {
                                // Check if this map is in our avoided list (case-insensitive)
                                std::string lowerWinningMap = winningMap;
                                std::transform(lowerWinningMap.begin(), lowerWinningMap.end(), lowerWinningMap.begin(), ::tolower);

                                for (const auto& avoidedMap : avoidedMaps) {
                                    std::string lowerAvoided = avoidedMap;
                                    std::transform(lowerAvoided.begin(), lowerAvoided.end(), lowerAvoided.begin(), ::tolower);

                                    // Exact match or partial match (in case of slight name variations)
                                    if (lowerWinningMap == lowerAvoided ||
                                        lowerWinningMap.find(lowerAvoided) != std::string::npos ||
                                        lowerAvoided.find(lowerWinningMap) != std::string::npos) {
                                        FlarialGUI::Notify("Avoided map won: " + winningMap);
                                        ImGui::SetClipboardText(pkt.message.data());
                                        reQ();
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (getOps<bool>("promomessage"))
        {
            if (pkt.message.find("§6[§e!§6]") != std::string::npos)
            {
                event.cancel();
            }
        }

        if (getOps<bool>("unlocks"))
        {
            if (pkt.message == "§a§l» §rYou have unused unlocks in your Locker!")
            {
                event.cancel();
            }
        }
        if (getOps<bool>("joined"))
        {
            std::regex pattern("joined\\. §8\\[\\d+/\\d+\\]");

            if (std::regex_search(pkt.message.data(), pattern))
            {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessage"))
        {
            if (pkt.message.contains(" §7§l» §r") && pkt.message.substr(0, 3) == "§7")
            {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessageplus"))
        {
            if (pkt.message.contains("§8 [§a+§8] §7§l» §r"))
            {
                event.cancel();
            }
        }
        if (getOps<bool>("noteaming"))
        {
            if (pkt.message == "§c§l» §r§c§lNo teaming! §r§6Teamers will be banned.")
            {
                event.cancel();
            }
        }
        if (getOps<bool>("friendaccept"))
        {
            if (pkt.message.substr(0, 40) == "§aYou received a friend invite from §b")
            {
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                std::string command;
                command.reserve(64);

                command.append("/f accept \"");
                command.append(pkt.message.substr(40, pkt.message.length() - 44));
                command.append("\"");
                command_packet->command = std::move(command);
                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                std::string notification;
                notification.reserve(64);
                notification.append("Accepted friend invite from:");
                notification.append(pkt.message.substr(40, pkt.message.length() - 44));
                notification.append(".");
                FlarialGUI::Notify(std::move(notification));
            }
        }
        if (getOps<bool>("partyaccept"))
        {
            if (pkt.message.find("§b wants you to join their party!") != std::string::npos)
            {
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                // command_packet->command = "/p accept \"" + pkt.message.substr(6, pkt.message.length() - 40) + "\"";
                std::string command;
                command.reserve(64);
                command.append("/p accept \"");
                command.append(pkt.message.substr(6, pkt.message.length() - 40));
                command.append("\"");
                command_packet->command = std::move(command);
                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                std::string notification;
                notification.reserve(64);
                notification.append("Accepted party invite from ");
                notification.append(pkt.message.substr(6, pkt.message.length() - 40));
                notification.append(".");
                FlarialGUI::Notify(std::move(notification));
            }
        }
    }
}

void HiveUtils::reQ()
{
    std::string gm = HiveModeCatcherListener::fullgamemodename;
    if (!this->isEnabled() or
    gm.empty()
    or
    gm.find("Hub") != std::string::npos
    )
    return;
    if (!getOps<bool>("hub"))
    {
        FlarialGUI::Notify("Finding a new game of " + gm);

        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
        command_packet->command = "/q " + HiveModeCatcherListener::currentGame;

        command_packet->origin.type = CommandOriginType::Player;

        command_packet->InternalSource = true;
        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
    }
    else
    {
        FlarialGUI::Notify("Executing command /hub");
        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

        command_packet->command = "/hub";

        command_packet->origin.type = CommandOriginType::Player;

        command_packet->InternalSource = true;

        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
    }
}

void HiveUtils::onKey(KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (isKeybind(event.keys) && isKeyPartOfKeybind(event.key) &&
        (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f3_screen" ||
            SDK::getCurrentScreen() == "zoom_screen")
    )
        reQ();
}

// Auto Map Vote helper implementations :3

std::optional<std::pair<uint32_t, std::vector<std::string>>> HiveUtils::parseMapVoteModal(const std::string& formJson)
{
    try {
        if (!nlohmann::json::accept(formJson)) {
            return std::nullopt;
        }

        auto json = nlohmann::json::parse(formJson);

        // Check if this is a "Choose map" modal
        // The form should have a title field
        if (!json.contains("title") || !json["title"].is_string()) {
            return std::nullopt;
        }

        std::string title = json["title"].get<std::string>();

        // Case-insensitive check for "choose map"
        std::string lowerTitle = title;
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);

        if (lowerTitle != "choose map") {
            return std::nullopt;
        }

        // This is a map vote modal! Parse the buttons
        if (!json.contains("buttons") || !json["buttons"].is_array()) {
            return std::nullopt;
        }

        std::vector<std::string> mapNames;
        for (const auto& button : json["buttons"]) {
            if (button.contains("text") && button["text"].is_string()) {
                std::string buttonText = button["text"].get<std::string>();
                // Clean up the button text (remove vote counts, etc.)
                // Hive buttons might be like "MapName\n§73 votes"
                size_t newlinePos = buttonText.find('\n');
                if (newlinePos != std::string::npos) {
                    buttonText = buttonText.substr(0, newlinePos);
                }
                // Remove color codes
                std::string cleanName;
                for (size_t i = 0; i < buttonText.size(); i++) {
                    if (buttonText[i] == '\xC2' && i + 1 < buttonText.size() && buttonText[i + 1] == '\xA7') {
                        i += 2; // Skip §X
                    } else if (buttonText[i] == '\xA7' && i + 1 < buttonText.size()) {
                        i += 1; // Skip §X
                    } else {
                        cleanName += buttonText[i];
                    }
                }
                // Trim whitespace
                while (!cleanName.empty() && std::isspace(cleanName.front())) cleanName.erase(0, 1);
                while (!cleanName.empty() && std::isspace(cleanName.back())) cleanName.pop_back();

                if (!cleanName.empty()) {
                    mapNames.push_back(cleanName);
                }
            }
        }

        if (mapNames.empty()) {
            return std::nullopt;
        }

        // FormId isn't in the JSON, it's in the packet header
        // Return 0 as placeholder, caller will use packet's formId
        return std::make_pair(0u, mapNames);

    } catch (const std::exception& e) {
        Logger::error("Error parsing map vote modal: {}", e.what());
        return std::nullopt;
    }
}

std::optional<int> HiveUtils::findPreferredMapIndex(const std::vector<std::string>& modalMaps, const std::string& currentGame)
{
    if (modalMaps.empty()) {
        return std::nullopt;
    }

    // Get user preferences for this game+variant
    // Try exact match first (e.g., "BED-DUOS" or "SKY-SOLOS")
    auto prefs = getMapPreferences(currentGame);

    // If no preferences found and the game has no variant suffix, try common variants
    if (prefs.empty() && currentGame.find('-') == std::string::npos) {
        // Try solos first (most common)
        prefs = getMapPreferences(currentGame + "-SOLOS");

        // Could also try duos, squads, etc. if needed
        if (prefs.empty()) {
            prefs = getMapPreferences(currentGame + "-DUOS");
        }
        if (prefs.empty()) {
            prefs = getMapPreferences(currentGame + "-SQUADS");
        }
    }

    if (prefs.empty()) {
        return std::nullopt;
    }

    // Create lowercase versions for comparison
    std::vector<std::string> lowerModalMaps;
    for (const auto& map : modalMaps) {
        std::string lower = map;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        lowerModalMaps.push_back(lower);
    }

    // Find the highest priority preferred map that's in the modal
    for (const auto& pref : prefs) {
        std::string lowerPref = pref;
        std::transform(lowerPref.begin(), lowerPref.end(), lowerPref.begin(), ::tolower);

        for (size_t i = 0; i < lowerModalMaps.size(); i++) {
            if (lowerModalMaps[i] == lowerPref) {
                return static_cast<int>(i);
            }
        }
    }

    // No preferred map found in the modal options
    return std::nullopt;
}

void HiveUtils::sendModalResponse(uint32_t formId, int buttonIndex)
{
    // Create the response packet
    std::shared_ptr<Packet> packet = SDK::createPacket(static_cast<int>(MinecraftPacketIds::ModalFormResponse));
    if (!packet) {
        return;
    }

    // PayloadPacket memory layout (from LeviLamina):
    // Packet base class: 0x00-0x2F (48 bytes, sizeof(Packet) == 0x30)
    // Payload starts at 0x30:
    //   mFormId: 0x30 (uint32_t, 4 bytes)
    //   padding: 0x34 (4 bytes for 8-byte alignment)
    //   mJSONResponse: 0x38 (std::optional<Json::Value>, 24 bytes)
    //   mFormCancelReason: 0x50 (std::optional<ModalFormCancelReason>, 2 bytes)

    uint8_t* pktData = reinterpret_cast<uint8_t*>(packet.get());

    // Zero out the payload area first to ensure clean state
    memset(pktData + 0x30, 0, 0x24);

    // Set mFormId at offset 0x30
    *reinterpret_cast<uint32_t*>(pktData + 0x30) = formId;

    // Set mJSONResponse (std::optional<Json::Value>) starting at 0x38
    *reinterpret_cast<int64_t*>(pktData + 0x38) = static_cast<int64_t>(buttonIndex);  // mValue.mInt
    *reinterpret_cast<int32_t*>(pktData + 0x40) = static_cast<int32_t>(MinecraftJson::ValueType::Int);  // mType
    *reinterpret_cast<uint8_t*>(pktData + 0x48) = 1;  // has_value = true

    // mFormCancelReason at 0x50 stays nullopt (zeroed = has_value false)

    // Send it
    if (SDK::clientInstance && SDK::clientInstance->getPacketSender()) {
        SDK::clientInstance->getPacketSender()->sendToServer(packet.get());
    }
}

void HiveUtils::refreshMapsForGame(const std::string& gameId)
{
    // Store the full gameId (with variant) for cache lookup
    lastFetchedGame = gameId;

    // Extract base game and variant from gameId (e.g., "bed-duos" -> "bed", "DUOS")
    std::string apiGameId = HiveMaps::gameIdToApiFormat(gameId);
    std::string variant = HiveMaps::extractVariant(gameId);

    // Get cached maps immediately (non-blocking)
    auto allMaps = HiveMaps::getMapsFromCacheOnly(apiGameId);

    // Trigger async refresh in background
    HiveMaps::refreshMapsAsync(apiGameId);

    // Filter maps by variant and deduplicate :3
    std::set<std::string> seenNames;
    std::vector<HiveMaps::MapInfo> filteredMaps;
    for (const auto& map : allMaps) {
        // Check if map matches the selected variant
        if (!HiveMaps::mapMatchesVariant(map, variant)) {
            continue;
        }

        std::string lowerName = map.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (seenNames.find(lowerName) == seenNames.end()) {
            seenNames.insert(lowerName);
            filteredMaps.push_back(map);
        }
    }
    currentGameMaps = filteredMaps;

    mapsFetched = true;
}

void HiveUtils::updatePrefsMapsFromCache(const std::string& gameId)
{
    // Only update local maps from cache - do NOT trigger a new async refresh
    std::string apiGameId = HiveMaps::gameIdToApiFormat(gameId);
    std::string variant = HiveMaps::extractVariant(gameId);

    auto allMaps = HiveMaps::getMapsFromCacheOnly(apiGameId);

    std::set<std::string> seenNames;
    std::vector<HiveMaps::MapInfo> filteredMaps;
    for (const auto& map : allMaps) {
        if (!HiveMaps::mapMatchesVariant(map, variant)) {
            continue;
        }

        std::string lowerName = map.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (seenNames.find(lowerName) == seenNames.end()) {
            seenNames.insert(lowerName);
            filteredMaps.push_back(map);
        }
    }
    currentGameMaps = filteredMaps;
}

std::vector<std::string> HiveUtils::getMapPreferences(const std::string& gameId)
{
    // Use gameId directly (with variant preserved) - getMapPrefsKey handles normalization
    std::string key = getMapPrefsKey(gameId);

    auto* setting = settings.getSettingByName<std::string>(key);
    if (!setting || setting->value.empty()) {
        return {};
    }

    // Parse comma-separated list
    std::vector<std::string> result;
    std::stringstream ss(setting->value);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Trim whitespace
        while (!item.empty() && std::isspace(item.front())) item.erase(0, 1);
        while (!item.empty() && std::isspace(item.back())) item.pop_back();
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

void HiveUtils::setMapPreferences(const std::string& gameId, const std::vector<std::string>& maps)
{
    // Use gameId directly (with variant preserved) - getMapPrefsKey handles normalization
    std::string key = getMapPrefsKey(gameId);

    // Join with commas
    std::string value;
    for (size_t i = 0; i < maps.size(); i++) {
        if (i > 0) value += ",";
        value += maps[i];
    }

    // Update the setting
    auto* setting = settings.getSettingByName<std::string>(key);
    if (setting) {
        setting->value = value;
    } else {
        settings.addSetting(key, value);
    }

    Client::SaveSettings();
}

void HiveUtils::moveMapPreference(const std::string& gameId, int fromIndex, int toIndex)
{
    auto prefs = getMapPreferences(gameId);

    if (fromIndex < 0 || fromIndex >= static_cast<int>(prefs.size()) ||
        toIndex < 0 || toIndex >= static_cast<int>(prefs.size())) {
        return;
    }

    std::string item = prefs[fromIndex];
    prefs.erase(prefs.begin() + fromIndex);
    prefs.insert(prefs.begin() + toIndex, item);

    setMapPreferences(gameId, prefs);
}

// Map avoider helper implementations

void HiveUtils::refreshMapsForAvoider(const std::string& gameId)
{
    lastFetchedAvoiderGame = gameId;

    std::string apiGameId = HiveMaps::gameIdToApiFormat(gameId);
    std::string variant = HiveMaps::extractVariant(gameId);

    // Get cached maps immediately (non-blocking)
    auto allMaps = HiveMaps::getMapsFromCacheOnly(apiGameId);

    // Trigger async refresh in background
    HiveMaps::refreshMapsAsync(apiGameId);

    std::set<std::string> seenNames;
    std::vector<HiveMaps::MapInfo> filteredMaps;
    for (const auto& map : allMaps) {
        if (!HiveMaps::mapMatchesVariant(map, variant)) {
            continue;
        }

        std::string lowerName = map.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (seenNames.find(lowerName) == seenNames.end()) {
            seenNames.insert(lowerName);
            filteredMaps.push_back(map);
        }
    }
    currentAvoiderMaps = filteredMaps;
}

void HiveUtils::updateAvoiderMapsFromCache(const std::string& gameId)
{
    // Only update local maps from cache - do NOT trigger a new async refresh
    std::string apiGameId = HiveMaps::gameIdToApiFormat(gameId);
    std::string variant = HiveMaps::extractVariant(gameId);

    auto allMaps = HiveMaps::getMapsFromCacheOnly(apiGameId);

    std::set<std::string> seenNames;
    std::vector<HiveMaps::MapInfo> filteredMaps;
    for (const auto& map : allMaps) {
        if (!HiveMaps::mapMatchesVariant(map, variant)) {
            continue;
        }

        std::string lowerName = map.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (seenNames.find(lowerName) == seenNames.end()) {
            seenNames.insert(lowerName);
            filteredMaps.push_back(map);
        }
    }
    currentAvoiderMaps = filteredMaps;
}

std::vector<std::string> HiveUtils::getAvoidedMaps(const std::string& gameId)
{
    std::string key = getAvoidedMapsKey(gameId);

    auto* setting = settings.getSettingByName<std::string>(key);
    if (!setting || setting->value.empty()) {
        return {};
    }

    std::vector<std::string> result;
    std::stringstream ss(setting->value);
    std::string item;
    while (std::getline(ss, item, ',')) {
        while (!item.empty() && std::isspace(item.front())) item.erase(0, 1);
        while (!item.empty() && std::isspace(item.back())) item.pop_back();
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

void HiveUtils::setAvoidedMaps(const std::string& gameId, const std::vector<std::string>& maps)
{
    std::string key = getAvoidedMapsKey(gameId);

    std::string value;
    for (size_t i = 0; i < maps.size(); i++) {
        if (i > 0) value += ",";
        value += maps[i];
    }

    auto* setting = settings.getSettingByName<std::string>(key);
    if (setting) {
        setting->value = value;
    } else {
        settings.addSetting(key, value);
    }

    Client::SaveSettings();
}

void HiveUtils::addAvoidedMap(const std::string& gameId, const std::string& mapName)
{
    auto avoided = getAvoidedMaps(gameId);

    // Check if already in list (case-insensitive)
    std::string lowerNewMap = mapName;
    std::transform(lowerNewMap.begin(), lowerNewMap.end(), lowerNewMap.begin(), ::tolower);

    for (const auto& existing : avoided) {
        std::string lowerExisting = existing;
        std::transform(lowerExisting.begin(), lowerExisting.end(), lowerExisting.begin(), ::tolower);
        if (lowerExisting == lowerNewMap) {
            return; // Already in list
        }
    }

    avoided.push_back(mapName);
    setAvoidedMaps(gameId, avoided);
}

void HiveUtils::removeAvoidedMap(const std::string& gameId, const std::string& mapName)
{
    auto avoided = getAvoidedMaps(gameId);

    std::string lowerRemove = mapName;
    std::transform(lowerRemove.begin(), lowerRemove.end(), lowerRemove.begin(), ::tolower);

    avoided.erase(
        std::remove_if(avoided.begin(), avoided.end(), [&lowerRemove](const std::string& s) {
            std::string lower = s;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower == lowerRemove;
        }),
        avoided.end()
    );

    setAvoidedMaps(gameId, avoided);
}