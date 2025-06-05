#include "TabList.hpp"

#include "Modules/ClickGUI/ClickGUI.hpp"

TabList::TabList(): Module("Tab List", "Java-like tab list.\nLists the current online players on the server.",
                           IDR_LIST_PNG, "TAB")
{
    Module::setup();
}

void TabList::onEnable()
{
    Listen(this, RenderEvent, &TabList::onRender)
    Listen(this, KeyEvent, &TabList::onKey)
    Module::onEnable();
}

void TabList::onDisable()
{
    Deafen(this, RenderEvent, &TabList::onRender)
    Deafen(this, KeyEvent, &TabList::onKey)
    Module::onDisable();
}

void TabList::defaultConfig()
{
    setDef("enabled", true);
    setDef("uiscale", 0.65f);
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    Module::defaultConfig("colors");
    setDef("alphaOrder", true);
    setDef("flarialFirst", true);
}

void TabList::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Tab List");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Misc");
    addToggle("Alphabetical Order", "", "alphaOrder");
    addToggle("Flarial First", "Prioritize Flarial users (Dev > Gamer > Booster > Supporter > Default) at the top", "flarialFirst");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    extraPadding();

    addHeader("Colors");
    addColorPicker("Background Color", "", "bg");
    addColorPicker("Text Color", "", "text");
    addColorPicker("Border Color", "", "border");
    addColorPicker("Glow Color", "", "glow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

int TabList::getRolePriority(const std::string& name)
{
    std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
    if (clearedName.empty()) return 5; // Lowest priority for invalid names

    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
    if (it == APIUtils::onlineUsers.end()) return 5; // Non-Flarial users

    // Check roles in order of priority using ApiUtils
    if (APIUtils::hasRole("Dev", clearedName)) return 0;
    if (APIUtils::hasRole("Gamer", clearedName)) return 1;
    if (APIUtils::hasRole("Booster", clearedName)) return 2;
    if (APIUtils::hasRole("Supporter", clearedName)) return 3;
    return 4; // Default Flarial user (in onlineUsers but no specific role)
}

std::vector<std::pair<mcUUID, PlayerListEntry>> TabList::sortByFlarialHierarchy(
    const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap)
{
    std::vector<std::pair<mcUUID, PlayerListEntry>> players(sourceMap.begin(), sourceMap.end());
    std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) {
        int priorityA = getRolePriority(a.second.name);
        int priorityB = getRolePriority(b.second.name);
        if (priorityA != priorityB) return priorityA < priorityB;
        // If priorities are equal, sort alphabetically
        return std::lexicographical_compare(
            a.second.name.begin(), a.second.name.end(),
            b.second.name.begin(), b.second.name.end(),
            [](char c1, char c2) {
                return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
            });
    });
    return players;
}

std::vector<std::string> TabList::copyMapInAlphabeticalOrder(
    const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap, bool flarialFirst)
{
    std::vector<std::string> names;
    std::vector<std::string> flarialNames;
    std::vector<std::string> nonFlarialNames;

    // Split players into Flarial and non-Flarial groups
    for (const auto& pair : sourceMap) {
        std::string name = pair.second.name;
        if (name.empty()) continue;

        std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
        if (clearedName.empty()) clearedName = name;

        auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
        if (flarialFirst && it != APIUtils::onlineUsers.end()) {
            flarialNames.push_back(name);
        }
        else {
            nonFlarialNames.push_back(name);
        }
    }

    if (flarialFirst) {
        // Sort Flarial users by hierarchy using ApiUtils
        std::sort(flarialNames.begin(), flarialNames.end(), [](const auto& a, const auto& b) {
            int priorityA = getRolePriority(a);
            int priorityB = getRolePriority(b);
            if (priorityA != priorityB) return priorityA < priorityB;
            // If priorities are equal, sort alphabetically
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](char c1, char c2) {
                    return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
                });
        });

        // Sort non-Flarial users alphabetically
        std::sort(nonFlarialNames.begin(), nonFlarialNames.end(), [](const auto& a, const auto& b) {
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](char c1, char c2) {
                    return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
                });
        });

        // Combine: Flarial users first, then non-Flarial
        names.insert(names.end(), flarialNames.begin(), flarialNames.end());
        names.insert(names.end(), nonFlarialNames.begin(), nonFlarialNames.end());
    }
    else {
        // Standard alphabetical sort for all players
        names = nonFlarialNames;
        names.insert(names.end(), flarialNames.begin(), flarialNames.end());
        std::sort(names.begin(), names.end(), [](const auto& a, const auto& b) {
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](char c1, char c2) {
                    return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
                });
        });
    }

    return names;
}

void TabList::normalRender(int index, std::string& value)
{
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced && (active || ClickGUI::editmenu)) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            float keycardSize = Constraints::RelativeConstraint(
                0.05f * getOps<float>("uiscale"), "height", true);

            Vec2<float> settingperc{ getOps<float>("percentageX"), getOps<float>("percentageY") };

            int i3 = 0;
            float i2 = 0;

            for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {
                i3++;
                if (i3 % 10 == 1) {
                    i2 += 4.85;
                }
            }

            Vec2<float> realcenter;
            if (settingperc.x != 0 || settingperc.y != 0) realcenter = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
            else realcenter = Constraints::CenterConstraint(i2 * keycardSize, 7.5f * keycardSize, "y", 0.0f, -0.85f);

            i3 = 0;
            float fakex = realcenter.x;
            Vec2<float> vec2 = realcenter;

            Vec2<float> rounde = Constraints::RoundingConstraint(
                getOps<float>("rounding") * getOps<float>("uiscale"),
                getOps<float>("rounding") * getOps<float>("uiscale"));

            float totalWidth = i2 * keycardSize;
            float fontSize = Constraints::SpacingConstraint(3, keycardSize);

            D2D1_COLOR_F disabledColor = getColor("bg");
            D2D1_COLOR_F textColor = getColor("text");
            D2D1_COLOR_F borderColor = getColor("border");

            disabledColor.a = getOps<float>("bgOpacity");
            textColor.a = getOps<float>("textOpacity");
            borderColor.a = getOps<float>("borderOpacity");

            if (getOps<bool>("glow")) 
                FlarialGUI::ShadowRect(Vec2<float>(fakex, realcenter.y), Vec2<float>(totalWidth, 7.5f * keycardSize), getColor("glow"), rounde.x, (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));
            

            if (getOps<bool>("BlurEffect")) {
                FlarialGUI::BlurRect(D2D1::RoundedRect(
                    D2D1::RectF(fakex, realcenter.y, fakex + totalWidth, realcenter.y + (7.5f * keycardSize)),
                    rounde.x, rounde.x));
            }

            if (getOps<bool>("border")) {
                FlarialGUI::RoundedHollowRect(
                    fakex, realcenter.y,
                    Constraints::RelativeConstraint(
                        (getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true),
                    borderColor, totalWidth, 7.5f * keycardSize, rounde.x, rounde.x);
            }

            FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, 7.5f * keycardSize, rounde.x, rounde.x);

            int i = 0;
            bool alphaOrder = getOps<bool>("alphaOrder");
            bool flarialFirst = getOps<bool>("flarialFirst");

            // Define role logos for reuse in both branches
            std::map<std::string, int> roleLogos = {
                {"Dev", IDR_CYAN_LOGO_PNG},
                {"Staff", IDR_WHITE_LOGO_PNG},
                {"Gamer", IDR_GAMER_LOGO_PNG},
                {"Booster", IDR_BOOSTER_LOGO_PNG},
                {"Supporter", IDR_SUPPORTER_LOGO_PNG},
                {"Default", IDR_RED_LOGO_PNG}
            };

            if (alphaOrder) {
                auto vecmap = copyMapInAlphabeticalOrder(SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap(), flarialFirst);

                for (const auto& rawName : vecmap) {
                    i++;
                    if (rawName.empty()) continue;

                    std::string name = String::removeColorCodes(rawName);
                    if (name.empty()) continue;

                    std::string clearedName = String::removeNonAlphanumeric(name);
                    if (clearedName.empty()) clearedName = name;

                    auto module = ModuleManager::getModule("Nick");
                    if (module && module->isEnabled() && !NickModule::backupOri.empty() &&
                        clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        name = module->getOps<std::string>("nick");
                        if (name.empty()) name = clearedName;
                    }

                    float xx = 0;

                    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
                    if (it != APIUtils::onlineUsers.end()) {
                        static float p1 = 0.175;
                        static float p2 = 0.196;
                        static float p3 = 0.7;
                        static float p4 = 0.77;
                        float width = Constraints::SpacingConstraint(p2, keycardSize);

                        int imageResource = roleLogos["Default"];
                        for (const auto& [role, resource] : roleLogos) {
                            if (APIUtils::hasRole(role, clearedName)) {
                                imageResource = resource;
                                break;
                            }
                        }

                        FlarialGUI::image(
                            imageResource,
                            D2D1::RectF(
                                fakex + Constraints::SpacingConstraint(p1, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                realcenter.y + width + Constraints::SpacingConstraint(0.17f, keycardSize),
                                fakex + Constraints::SpacingConstraint(p3, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize)
                            )
                        );

                        xx = Constraints::SpacingConstraint(0.5, keycardSize);
                    }

                    FlarialGUI::FlarialTextWithFont(
                        fakex + xx + Constraints::SpacingConstraint(0.5, keycardSize),
                        realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize),
                        String::StrToWStr(name).c_str(),
                        keycardSize * 5, keycardSize,
                        DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize),
                        DWRITE_FONT_WEIGHT_NORMAL, textColor, true
                    );

                    realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                    if (i % 10 == 0) {
                        realcenter.y = vec2.y;
                        fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                    }
                }
            }
            else {
                // Use Flarial hierarchy sorting if flarialFirst is enabled
                auto players = flarialFirst ? sortByFlarialHierarchy(SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap())
                                   : std::vector<std::pair<mcUUID, PlayerListEntry>>(SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap().begin(),
                                       SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap().end());

                for (const auto& pair : players) {
                    i++;
                    std::string name = pair.second.name;
                    if (name.empty()) continue;

                    std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
                    if (clearedName.empty()) clearedName = name;

                    auto module = ModuleManager::getModule("Nick");
                    if (module && module->isEnabled() && !NickModule::backupOri.empty() &&
                        clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        name = module->getOps<std::string>("nick");
                        if (name.empty()) name = clearedName;
                    }

                    float xx = 0;

                    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
                    if (it != APIUtils::onlineUsers.end()) {
                        static float p1 = 0.175;
                        static float p2 = 0.196;
                        static float p3 = 0.7;
                        static float p4 = 0.77;
                        float width = Constraints::SpacingConstraint(p2, keycardSize);

                        int imageResource = roleLogos["Default"];
                        for (const auto& [role, resource] : roleLogos) {
                            if (APIUtils::hasRole(role, clearedName)) {
                                imageResource = resource;
                                break;
                            }
                        }

                        FlarialGUI::image(
                            imageResource,
                            D2D1::RectF(
                                fakex + Constraints::SpacingConstraint(p1, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                realcenter.y + width + Constraints::SpacingConstraint(0.17f, keycardSize),
                                fakex + Constraints::SpacingConstraint(p3, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize)
                            )
                        );

                        xx = Constraints::SpacingConstraint(0.5, keycardSize);
                    }

                    FlarialGUI::FlarialTextWithFont(
                        fakex + xx + Constraints::SpacingConstraint(0.5, keycardSize),
                        realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize),
                        String::StrToWStr(name).c_str(),
                        keycardSize * 5, keycardSize,
                        DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize),
                        DWRITE_FONT_WEIGHT_NORMAL, textColor, true
                    );

                    realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                    if (i % 10 == 0) {
                        realcenter.y = vec2.y;
                        fakex += Constraints::SpacingConstraint(5.0, keycardSize);
                    }
                }
            }
        }
    }
}

void TabList::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    std::string text;
    this->normalRender(20, text);
}

void TabList::onKey(const KeyEvent& event)
{
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        keybindActions[0]({});
    }

    if (!this->isKeybind(event.keys)) this->active = false;
}