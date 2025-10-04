#include "HiveUtils.hpp"

#include "Client.hpp"
#include "SDK/Client/Network/Packet/ModalFormRequestPacket.hpp"
#include "SDK/Client/Network/Packet/ModalFormResponsePacket.hpp"

#include "SDK/Client/Network/Packet/SetTitlePacket.hpp"

void HiveUtils::onEnable() {
    Listen(this, PacketEvent, &HiveUtils::onPacketReceive)
    Listen(this, KeyEvent, &HiveUtils::onKey)
    Module::onEnable();
}

void HiveUtils::onDisable() {
    Deafen(this, PacketEvent, &HiveUtils::onPacketReceive)
    Deafen(this, KeyEvent, &HiveUtils::onKey)
    Module::onDisable();
}

void HiveUtils::defaultConfig() {
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

    // auto select
    setDef("bedwarsenabled", false);
    setDef("bedwars", (std::string) "Solos");
    setDef("skywarsenabled", false);
    setDef("skywars", (std::string) "Solos");
    setDef("buildbattleenabled", false);
    setDef("buildbattle", (std::string) "Solos");
    setDef("survivalgamesenabled", false);
    setDef("survivalgames", (std::string) "Solos");
    setDef("thebridgeenabled", false);
    setDef("thebridge", (std::string) "Solos");
}

void HiveUtils::settingsRender(float settingsOffset) {
    initSettingsPage();
    addHeader("General");
    addToggle("Use /hub instead of /q", "", "hub");
    // this->addDropdown("Command to use", "Command to execute when somthing gets triggered",  std::vector<std::string>{"Re-Q same game", "Q a Random game", "Go back to the hub"}, getOps<std::string>("commandtouse"));

    addHeader("Auto Re Q");
    addToggle("Auto re-queue ", "Find a new game when the current game is over", "ReQ");
    addToggle("Solo mode ", "Re-Q when you finish a game or die and can't respawn.\nNot recomended while in a party.", "solo");
    addToggle("Team Elimination", "Re-Q when the team your on is fully ELIMINATED.", "eliminated");
    addKeybind("Requeue Keybind", "When setting, hold the new bind for 2 seconds.", "keybind", true);

    addHeader("Map avoider");

    addToggle("Map Avoider", "Automatically finds you a new game when a specific map has won the vote", "AutoMapAvoider");
    // this->addTextBox("Maps", " Input one or more maps using comma's.", getOps<std::string>("text"), 100);
    addButton("Add Map", "Add a map you want to avoid.", "Add Map", [this] {

        // std::string keybindName = "keybind-" + FlarialGUI::cached_to_string(totalKeybinds);
        std::string commandName = "map-" + FlarialGUI::cached_to_string(totalmaps);

        // this->settings.addSetting(keybindName, (std::string)"");
        this->settings.addSetting(commandName, (std::string)"");


        int i = totalmaps;

        Client::SaveSettings();
        FlarialGUI::Notify("New textbox created, input a map to avoid!");
        totalmaps++;
    });

    for (int i = 0; i < totalmaps; ++i) {
        std::string commandSettingName = "map-" + FlarialGUI::cached_to_string(i);

        if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
            addTextBox(
                "Map " + FlarialGUI::cached_to_string(i),
                "Input a map you would like to avoid.",
                settings.getSettingByName<std::string>(commandSettingName)->value
            );
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
    addConditionalSliderInt(getOps<bool>("deathcountenabled"), "Death Limiter: Amount of Deaths", "Configure the amount of deaths required here.", "deathcount", 100, 1);
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
    addToggle("Player message Hive+", "Removes all messages from Hive+ players", "playermessageplus");
    addToggle("No Teaming", "Removes No Teaming!", "noteaming");

    addHeader("Auto accept");
    addToggle("Friend request", "Automatically accept incoming friend requests.", "friendaccept");
    addToggle("Party request", "Automatically accept incoming party requests.", "partyaccept");

    addHeader("Auto Select Variants");
    addToggle("BedWars", "Choose between solos, duos, squads etc.", "bedwarsenabled");
    addConditionalDropdown(getOps<bool>("bedwarsenabled"), "BedWars: Choose Variant", "Select from the drop-down.", std::vector<std::string>{"Solos", "Duos", "Squads", "Manor", "MEGA"}, "bedwars", true);
    addToggle("SkyWars", "Choose between solos, duos, squads etc.", "skywarsenabled");
    addConditionalDropdown(getOps<bool>("skywarsenabled"), "SkyWars: Choose Variant", "Select from the drop-down.", std::vector<std::string>{"Solos", "Duos", "Squads", "MEGA LTM"}, "skywars", true);
    addToggle("Build Battle", "Choose between solos, duos, etc.", "buildbattleenabled");
    addConditionalDropdown(getOps<bool>("buildbattleenabled"), "Build Battle: Choose Variant", "Select from the drop-down.", std::vector<std::string>{"Solos", "Duos", "Speed Builders (LTM)", "Solos (Double Build Time)", "Duos (Double Build Time)"}, "buildbattle", true);
    addToggle("Survival Games", "Choose between solos, duos, squads etc.", "survivalgamesenabled");
    addConditionalDropdown(getOps<bool>("survivalgamesenabled"), "Survival Games: Choose Variant", "Select from the drop-down.", std::vector<std::string>{"Solos", "Duos"}, "survivalgames", true);
    addToggle("The Bridge", "Choose between solo and duos.", "thebridgeenabled");
    addConditionalDropdown(getOps<bool>("thebridgeenabled"), "The Bridge: Choose Variant", "Select from the drop-down.", std::vector<std::string>{"Solos", "Duos"}, "thebridge", true);
    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HiveUtils::onPacketReceive(PacketEvent &event) {
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    if (id == MinecraftPacketIds::SetTitle) {
        auto* pkt = reinterpret_cast<SetTitlePacket*>(event.getPacket());
        if (getOps<bool>("ReQ")) {
            if (getOps<bool>("solo")) {


                if (//pkt->text == "§cYou're a spectator!" || //this brobably isn't needed anymore
                    pkt->text == "§cYou died!" ||
                    pkt->text == "§7You're spectating the §as§eh§6o§cw§7!") {
                    reQ();

                }
            }
        }
        if (getOps<bool>("copyCS"))
        {
            if (!pkt->text.empty() && pkt->text.find(Copy_CS) == std::string::npos) {
                if (pkt->text.length() >= 15 && pkt->text.substr(0, 13) == "§eJoin Code:") {
                    Copy_CS = pkt->text;
                    if (!getOps<bool>("includecommand")) {
                        ImGui::SetClipboardText(pkt->text.substr(17, pkt->text.length()).c_str());
                    }
                    else {
                        ImGui::SetClipboardText(("/cs " + pkt->text.substr(17, pkt->text.length())).c_str());
                    }
                    FlarialGUI::Notify("Compied CS Code to Clipboard");
                }
            }
        }
    }
    if (id == MinecraftPacketIds::Text) {
        auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());
        if (HiveModeCatcherListener::currentGame == "DR" and pkt->message == "§a§l» §r§bThe game has started! Run!") deaths = 0;
        
        if (getOps<bool>("deathcountenabled") and HiveModeCatcherListener::currentGame == "DR" and pkt->message == "§c§l» §r§cYou died!")
        {
            deaths++;
            if (deaths > getOps<int>("deathcount"))
            {
                reQ();
                FlarialGUI::Notify("Death count limit reached.");
                deaths = 0;
            }
        }
        if (getOps<bool>("ReQ")) {
            //if(!module->getOps<bool>("solo")) {
            if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                reQ();
                return;
            }
            if (getOps<bool>("eliminated")) {
                if (pkt->message.length() > 27) {
                    if (pkt->message.substr(12, 15) == "You are on the ") {
                        teamcolor = pkt->message.substr(27, pkt->message.length() - 28);
                    }
                }

                if (pkt->message.find("§7has been §cELIMINATED§7!") != std::string::npos && pkt->message.find(teamcolor) != std::string::npos && !teamcolor.empty()) {
                    reQ();
                    FlarialGUI::Notify("Your Team has been ELIMINATED");
                    return;
                }
            }
            if (getOps<bool>("solo")) {

                if (pkt->message.substr(0, 48) == "§a§l» §r§eYou finished all maps and came in" || //gravity
                    pkt->message.substr(0, 30) == "§a§l» §r§eYou finished in") { //deathrun
                    reQ();

                }
            }
        }
        if (getOps<bool>("murderer")) {

            if (pkt->message == "§c§l» §r§c§lMurderer") {
                reQ();
                FlarialGUI::Notify("Found role Murderer");

            }
        }
        if (getOps<bool>("sheriff")) {

            if (pkt->message == "§9§l» §r§9§lSheriff") {
                reQ();
                FlarialGUI::Notify("Found role Sheriff");

            }
        }
        if (getOps<bool>("innocent")) {

            if (pkt->message == "§a§l» §r§a§lInnocent") {
                reQ();
                FlarialGUI::Notify("Found role Innocent");

            }
        }
        if (getOps<bool>("death")) {

            if (pkt->message == "§d§l» §r§bYou are a §cDeath") {
                reQ();
                FlarialGUI::Notify("Found role Death");

            }
        }
        if (getOps<bool>("runner")) {

            if (pkt->message == "§d§l» §r§bYou are a §aRunner") {
                reQ();
                FlarialGUI::Notify("Found role Runner");

            }
        }
        if (getOps<bool>("hider")) {

            if (pkt->message == "§e§l» §rYou are a §eHIDER") {
                reQ();
                FlarialGUI::Notify("Found role Hider");

            }
        }
        if (getOps<bool>("seeker")) {

            if (pkt->message == "§c§l» §rYou are a §cSEEKER") {
                reQ();
                FlarialGUI::Notify("Found role Seeker");

            }
        }
        if (getOps<bool>("AutoMapAvoider")) {
            for (int i = 0; i < totalmaps; ++i)
            {
                std::string count;
                /*if (i > 0) */count = "-" + FlarialGUI::cached_to_string(i);

                std::string map = this->settings.getSettingByName<std::string>("map" + count)->value;
                std::transform(map.begin(), map.end(), map.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (map.empty()) continue;

                std::string message = pkt->message;
                std::transform(message.begin(), message.end(), message.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });

                if (message.find("§b§l» §r§e" + map) != std::string::npos)
                {
                    FlarialGUI::Notify("Found Map: " + map);
                    ImGui::SetClipboardText(pkt->message.c_str());
                    reQ();
                    return;
                }
            }
        }

        if (getOps<bool>("promomessage")) {
            if (pkt->message.find("§6[§e!§6]") != std::string::npos) {
                event.cancel();
            }
        }

        if (getOps<bool>("unlocks")) {
            if (pkt->message == "§a§l» §rYou have unused unlocks in your Locker!") {
                event.cancel();
            }
        }
        if (getOps<bool>("joined")) {
            std::regex pattern("joined\\. §8\\[\\d+/\\d+\\]");

            if (std::regex_search(pkt->message, pattern)) {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessage")) {
            if (pkt->message.contains(" §7§l» §r") && pkt->message.substr(0, 3) == "§7") {
                event.cancel();
            }
        }
        if (getOps<bool>("playermessageplus")) {
            if (pkt->message.contains("§8 [§a+§8] §7§l» §r")) {
                event.cancel();
            }
        }
        if (getOps<bool>("noteaming")) {
            if (pkt->message == "§c§l» §r§c§lNo teaming! §r§6Teamers will be banned.") {
                event.cancel();
            }
        }
        if (getOps<bool>("friendaccept")) {
            if (pkt->message.substr(0, 40) == "§aYou received a friend invite from §b") {

                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                command_packet->command = "/f accept \"" + pkt->message.substr(40, pkt->message.length() - 44) + "\"";
                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(40, pkt->message.length() - 44));
            }
        }
        if (getOps<bool>("partyaccept")) {
            if (pkt->message.find("§b wants you to join their party!") != std::string::npos) {

                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                command_packet->command = "/p accept \"" + pkt->message.substr(6, pkt->message.length() - 40) + "\"";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                FlarialGUI::Notify("Accepted party invite from: " + pkt->message.substr(6, pkt->message.length() - 40));
            }
        }
    }
    if (id == MinecraftPacketIds::ShowModalForm) {
        auto smf = reinterpret_cast<ModalFormRequestPacket*>(event.getPacket());
        std::string formJsonStr = smf->mFormJSON;
        int formId = smf->mFormId;

        if (formJsonStr.empty()) return;

        try {
            auto dialog = nlohmann::json::parse(formJsonStr);

            // clean up "garbage"
            auto norm = [](const std::string &s_in)->std::string {
                std::string s = String::removeColorCodes(s_in);
                for (char &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                size_t a = s.find_first_not_of(" \t\r\n");
                if (a == std::string::npos) return std::string();
                size_t b = s.find_last_not_of(" \t\r\n");
                return s.substr(a, b - a + 1);
            };

            std::string rawTitle;
            if (dialog.contains("title") && dialog["title"].is_string()) rawTitle = dialog["title"].get<std::string>();
            else if (dialog.contains("header") && dialog["header"].is_string()) rawTitle = dialog["header"].get<std::string>();
            else if (dialog.contains("content") && dialog["content"].is_string()) rawTitle = dialog["content"].get<std::string>();

            std::string titleNoColor = String::removeColorCodes(rawTitle);
            std::string titleNorm = norm(rawTitle);

            std::vector<std::pair<int,std::string>> options;
            const std::vector<std::string> candidateArrays = {"buttons","options","choices","entries","elements","content"};
            for (auto &name : candidateArrays) {
                if (dialog.contains(name) && dialog[name].is_array()) {
                    int idx = 0;
                    for (auto &elem : dialog[name]) {
                        std::string label;
                        if (elem.is_string()) label = elem.get<std::string>();
                        else if (elem.contains("text") && elem["text"].is_string()) label = elem["text"].get<std::string>();
                        else if (elem.contains("label") && elem["label"].is_string()) label = elem["label"].get<std::string>();
                        else try { label = elem.dump(); } catch(...) { label = std::string(); }
                        options.emplace_back(idx, label);
                        ++idx;
                    }
                    break;
                }
            }

            // for (size_t i = 0; i < options.size() && i < 6; ++i) {
            //     Logger::debug("  opt[{}] = \"{}\"", options[i].first, String::removeColorCodes(options[i].second));
            // }
            // if (options.size() > 6) Logger::debug("  ... ({} more)", options.size() - 6);

            if (options.empty()) return;

            // craft the packet, cancel out dialog popup
            auto sendResponseAndCancel = [&](int chosenIdx, const std::string &chosenLabel, const std::string &notifyMsg) -> void {
                event.cancel();
                if (!notifyMsg.empty()) FlarialGUI::Notify(notifyMsg);
                std::shared_ptr<Packet> packet = SDK::createPacket(static_cast<int>(MinecraftPacketIds::ModalFormResponse));
                auto* formResponsePacket = reinterpret_cast<ModalFormResponsePacket*>(packet.get());
                formResponsePacket->mFormId = formId;
                MinecraftJson::Value json;
                json.mType = MinecraftJson::ValueType::Int;
                json.mValue.mInt = chosenIdx;
                formResponsePacket->mFormResponse = json;
                // Logger::info("sel index={} label=\"{}\"", chosenIdx, chosenLabel);
                SDK::clientInstance->getPacketSender()->sendToServer(formResponsePacket);
            };

            // find btn with something...
            auto findOptionContaining = [&](const std::string &targetRaw)->int {
                if (targetRaw.empty()) return -1;
                std::string target = norm(targetRaw);
                for (auto &p : options) {
                    std::string labelNorm = norm(p.second);
                    if (!target.empty() && labelNorm.find(target) != std::string::npos) return p.first;
                }
                return -1;
            };

            // try to auto-select based on title and config
            auto tryAuto = [&](const std::string &gameTitleToken,
                               const std::function<bool()> &isEnabled,
                               const std::function<std::string()> &getWant,
                               const std::string &notifyPrefix)->bool {
                std::string gnorm = gameTitleToken;
                if (titleNorm.find(gnorm) == std::string::npos) return false;
                if (!isEnabled()) return false;

                std::string want = getWant();
                int idx = findOptionContaining(want);
                if (idx >= 0) {
                    std::string chosenLabel = String::removeColorCodes(options[idx].second);
                    sendResponseAndCancel(idx, chosenLabel, notifyPrefix + want);
                    return true;
                }
                return false;
            };

            bool handled = false;

            handled = tryAuto("bedwars",
                [&](){ return getOps<bool>("bedwarsenabled"); },
                [&](){ return getOps<std::string>("bedwars"); },
                "Selecting BedWars ");

            if (!handled) handled = tryAuto("skywars",
                [&](){ return getOps<bool>("skywarsenabled"); },
                [&](){ return getOps<std::string>("skywars"); },
                "Selecting SkyWars ");

            if (!handled) handled = tryAuto("build",
                [&](){ return getOps<bool>("buildbattleenabled"); },
                [&](){ return getOps<std::string>("buildbattle"); },
                "Selecting Build Battle ");

            if (!handled) handled = tryAuto("bridge",
                [&](){ return getOps<bool>("thebridgeenabled"); },
                [&](){ return getOps<std::string>("thebridge"); },
                "Selecting The Bridge ");

            if (!handled) handled = tryAuto("survival",
                [&](){ return getOps<bool>("survivalgamesenabled"); },
                [&](){ return getOps<std::string>("survivalgames"); },
                "Selecting Survival Games ");

        } catch (const std::exception &e) {
            Logger::error("can't parse modal json: {}", e.what());
        }
    }
}

void HiveUtils::reQ() {
    std::string gm = HiveModeCatcherListener::fullgamemodename;
    if (!this->isEnabled() or gm.empty() or gm.find("Hub") != std::string::npos) return;
    if (!getOps<bool>("hub")) {
        FlarialGUI::Notify("Finding a new game of " + gm);

        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
        command_packet->command = "/q " + HiveModeCatcherListener::currentGame;

        command_packet->origin.type = CommandOriginType::Player;

        command_packet->InternalSource = true;
        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
    }
    else {
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
    if (isKeybind(event.keys) && isKeyPartOfKeybind(event.key) && (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f3_screen" || SDK::getCurrentScreen() == "zoom_screen")) reQ();
}
