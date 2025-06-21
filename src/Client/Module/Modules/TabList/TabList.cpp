#include "TabList.hpp"

#include <wrl/client.h>

#include "Command/Commands/SkinStealCommand.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

TabList::TabList(): Module("Tab List", "Java-like tab list.\nLists the current online players on the server.", IDR_LIST_PNG, "TAB") {
}

void TabList::onEnable() {
    Listen(this, RenderEvent, &TabList::onRender)
    Listen(this, KeyEvent, &TabList::onKey)
    Module::onEnable();
}

void TabList::onDisable() {
    Deafen(this, RenderEvent, &TabList::onRender)
    Deafen(this, KeyEvent, &TabList::onKey)
    Module::onDisable();
}

void TabList::defaultConfig() {
    setDef("enabled", true);
    setDef("uiscale", 0.65f);
    setDef("playerCount", true);
    setDef("serverIP", true);
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    Module::defaultConfig("colors");
    setDef("alphaOrder", true);
    setDef("flarialFirst", true);
}

void TabList::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Tab List");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Misc");
    addToggle("Player Count", "", "playerCount");
    addToggle("Server IP", "", "serverIP");
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

int TabList::getRolePriority(const std::string &name) {
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

std::vector<const std::pair<const mcUUID, PlayerListEntry> *> TabList::sortByFlarialHierarchy(
    const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap) {
    std::vector<const std::pair<const mcUUID, PlayerListEntry> *> players;
    players.reserve(sourceMap.size());

    for (const auto &pair: sourceMap) {
        players.push_back(&pair);
    }

    std::sort(players.begin(), players.end(), [](const auto *a, const auto *b) {
        int priorityA = getRolePriority(a->second.name);
        int priorityB = getRolePriority(b->second.name);
        if (priorityA != priorityB) return priorityA < priorityB;

        return std::lexicographical_compare(
            a->second.name.begin(), a->second.name.end(),
            b->second.name.begin(), b->second.name.end(),
            [](char c1, char c2) {
                return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
            });
    });

    return players;
}

std::vector<const std::pair<const mcUUID, PlayerListEntry> *> TabList::copyMapInAlphabeticalOrder(
    const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap, bool flarialFirst) {
    std::vector<const std::pair<const mcUUID, PlayerListEntry> *> flarialEntries;
    std::vector<const std::pair<const mcUUID, PlayerListEntry> *> nonFlarialEntries;

    for (const auto &pair: sourceMap) {
        const std::string &name = pair.second.name;
        if (name.empty()) continue;

        std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
        if (clearedName.empty()) clearedName = name;

        auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
        if (flarialFirst && it != APIUtils::onlineUsers.end()) {
            flarialEntries.push_back(&pair);
        } else {
            nonFlarialEntries.push_back(&pair);
        }
    }

    auto sortByRoleAndName = [](const auto *a, const auto *b) {
        int priorityA = getRolePriority(a->second.name);
        int priorityB = getRolePriority(b->second.name);
        if (priorityA != priorityB) return priorityA < priorityB;
        return std::lexicographical_compare(
            a->second.name.begin(), a->second.name.end(),
            b->second.name.begin(), b->second.name.end(),
            [](char c1, char c2) {
                return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
            });
    };

    auto sortByName = [](const auto *a, const auto *b) {
        return std::lexicographical_compare(
            a->second.name.begin(), a->second.name.end(),
            b->second.name.begin(), b->second.name.end(),
            [](char c1, char c2) {
                return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2));
            });
    };

    std::vector<const std::pair<const mcUUID, PlayerListEntry> *> result;

    if (flarialFirst) {
        std::sort(flarialEntries.begin(), flarialEntries.end(), sortByRoleAndName);
        std::sort(nonFlarialEntries.begin(), nonFlarialEntries.end(), sortByName);
        result.insert(result.end(), flarialEntries.begin(), flarialEntries.end());
        result.insert(result.end(), nonFlarialEntries.begin(), nonFlarialEntries.end());
    } else {
        result = flarialEntries;
        result.insert(result.end(), nonFlarialEntries.begin(), nonFlarialEntries.end());
        std::sort(result.begin(), result.end(), sortByName);
    }

    return result;
}

ID3D11ShaderResourceView *CreateTextureFromBytes(
    ID3D11Device *device,
    const unsigned char *data,
    int width,
    int height) {
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;

    ID3D11Texture2D *texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &texture);
    if (FAILED(hr) || !texture) return nullptr;

    ID3D11ShaderResourceView *srv = nullptr;
    hr = device->CreateShaderResourceView(texture, nullptr, &srv);
    texture->Release(); // Release after SRV is created

    return srv;
}

void TabList::normalRender(int index, std::string &value) {
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced && (active || ClickGUI::editmenu)) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            float keycardSize = Constraints::RelativeConstraint(0.05f * getOps<float>("uiscale"), "height", true);

            Vec2<float> settingperc{0.0, 0.0};

            bool alphaOrder = getOps<bool>("alphaOrder");
            bool flarialFirst = getOps<bool>("flarialFirst");

            // Define role logos for reuse in both branches
            std::map<std::string, int> roleLogos = {{"Dev", IDR_CYAN_LOGO_PNG}, {"Staff", IDR_WHITE_LOGO_PNG}, {"Gamer", IDR_GAMER_LOGO_PNG}, {"Booster", IDR_BOOSTER_LOGO_PNG}, {"Supporter", IDR_SUPPORTER_LOGO_PNG}, {"Default", IDR_RED_LOGO_PNG}};
            auto module = ModuleManager::getModule("Nick");

            std::vector<const std::pair<const mcUUID, PlayerListEntry> *> vecmap = alphaOrder
                                                                                       ? copyMapInAlphabeticalOrder(SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap(), flarialFirst)
                                                                                       : (flarialFirst
                                                                                              ? sortByFlarialHierarchy(SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap())
                                                                                              : [] {
                                                                                                  const auto &map = SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap();
                                                                                                  std::vector<const std::pair<const mcUUID, PlayerListEntry> *> v;
                                                                                                  v.reserve(map.size());
                                                                                                  for (const auto &pair: map) v.push_back(&pair);
                                                                                                  return v;
                                                                                              }());

            float totalWidth = keycardSize * 0.4f;
            float totalHeight = keycardSize * 0.5f;

            float fontSize = Constraints::SpacingConstraint(3, keycardSize);
            std::vector<float> columnx = {};
            float curMax = 0;
            size_t validPlayers = 0;

            for (size_t i = 0; i < vecmap.size(); i++) {
                if (vecmap[i]->second.name.empty()) continue;

                std::string name = String::removeColorCodes(vecmap[i]->second.name);
                if (name.empty()) continue;

                std::string clearedName = String::removeNonAlphanumeric(name);
                if (clearedName.empty()) clearedName = name;

                if (module && module->isEnabled() && !NickModule::backupOri.empty() && clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                    name = module->getOps<std::string>("nick");
                    if (name.empty()) name = clearedName;
                }

                auto textMetric = FlarialGUI::getFlarialTextSize(String::StrToWStr(name).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);

                auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
                if (it != APIUtils::onlineUsers.end()) textMetric.x += Constraints::SpacingConstraint(0.5, keycardSize);

                if (textMetric.x > curMax) curMax = textMetric.x;
                if (i < 10) totalHeight += keycardSize * 0.7f;

                validPlayers++;

                if ((i + 1) % 10 == 0 || i == vecmap.size() - 1) {
                    totalWidth += curMax + keycardSize * 0.4f;
                    columnx.push_back(curMax + keycardSize * 0.4f);
                    curMax = 0;
                }
            }

            std::string curPlayer;
            std::string countTxt;
            ImVec2 curPlayerMetrics;

            if (getOps<bool>("serverIP")) {
                ImVec2 serverIpMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(SDK::getServerIP()).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                if (totalWidth < serverIpMetrics.x + keycardSize) totalWidth = serverIpMetrics.x + keycardSize;

                totalHeight += keycardSize * 1.25f;
            }

            if (getOps<bool>("playerCount")) {
                countTxt = std::to_string(validPlayers) + " player(s) online";
                curPlayer = module && module->isEnabled() && !NickModule::backupOri.empty() ? module->getOps<std::string>("nick") : SDK::clientInstance->getLocalPlayer()->getPlayerName();
                ImVec2 countTxtMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(countTxt).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                curPlayerMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide("Player:__" + curPlayer).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                curPlayerMetrics.x += Constraints::SpacingConstraint(0.5, keycardSize);

                if (totalWidth < countTxtMetrics.x + keycardSize) totalWidth = countTxtMetrics.x + keycardSize;
                if (totalWidth < curPlayerMetrics.x + keycardSize) totalWidth = curPlayerMetrics.x + keycardSize;

                totalHeight += keycardSize * 2;
            }


            Vec2<float> realcenter;
            if (settingperc.x != 0 || settingperc.y != 0) realcenter = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
            else realcenter = Constraints::CenterConstraint(totalWidth, totalHeight, "y", 0.0f, -0.85f);

            float fakex = realcenter.x;
            Vec2<float> vec2 = realcenter;

            Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));

            D2D1_COLOR_F disabledColor = getColor("bg");
            D2D1_COLOR_F textColor = getColor("text");
            D2D1_COLOR_F borderColor = getColor("border");

            disabledColor.a = getOps<float>("bgOpacity");
            textColor.a = getOps<float>("textOpacity");
            borderColor.a = getOps<float>("borderOpacity");

            if (getOps<bool>("glow")) FlarialGUI::ShadowRect(Vec2<float>(fakex, realcenter.y), Vec2<float>(totalWidth, totalHeight), getColor("glow"), rounde.x, (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));
            if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(fakex, realcenter.y, fakex + totalWidth, realcenter.y + totalHeight), rounde.x, rounde.x));
            if (getOps<bool>("border")) FlarialGUI::RoundedHollowRect(fakex, realcenter.y, Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true), borderColor, totalWidth, totalHeight, rounde.x, rounde.x);

            FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, totalHeight, rounde.x, rounde.x);

            if (getOps<bool>("serverIP")) {
                FlarialGUI::FlarialTextWithFont(MC::windowSize.x / 2.f, realcenter.y, FlarialGUI::to_wide(SDK::getServerIP()).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                realcenter.y += keycardSize * 1.25f;
            }

            for (size_t i = 0; i < vecmap.size(); i++) {
                if (vecmap[i]->second.name.empty()) continue;

                std::string name = String::removeColorCodes(vecmap[i]->second.name);
                if (name.empty()) continue;

                std::string clearedName = String::removeNonAlphanumeric(name);
                if (clearedName.empty()) clearedName = name;

                if (module && module->isEnabled() && !NickModule::backupOri.empty() && clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
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

                    int imageResource = roleLogos["Default"];
                    for (const auto &[role, resource]: roleLogos) {
                        if (APIUtils::hasRole(role, clearedName)) {
                            imageResource = resource;
                            break;
                        }
                    }

                    FlarialGUI::image(imageResource, D2D1::RectF(
                                          fakex + Constraints::SpacingConstraint(p1, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                          realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                          fakex + Constraints::SpacingConstraint(p3, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                                          realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize)));

                    xx = Constraints::SpacingConstraint(0.5, keycardSize);
                }
                // PLAYER HEAD START

                Logger::debug("{}x{}", vecmap[i]->second.playerSkin.mSkinImage.mWidth, vecmap[i]->second.playerSkin.mSkinImage.mHeight);
                std::cout << static_cast<int>(vecmap[i]->second.playerSkin.mSkinImage.imageFormat) << std::endl;

                if (static_cast<int>(vecmap[i]->second.playerSkin.mSkinImage.imageFormat) == 4) {
                    std::vector<unsigned char> head = SkinStealCommand::cropHead(vecmap[i]->second.playerSkin.mSkinImage);

                    // Microsoft::WRL::ComPtr<ID3D11Device> d3d11Device;

                    // if (SwapchainHook::queue != nullptr) {
                    //     Logger::debug("using dx12");
                    //     SwapchainHook::d3d11On12Device->QueryInterface(IID_PPV_ARGS(&d3d11Device));
                    // }

                    if (SwapchainHook::d3d11Device != nullptr) {
                        Logger::debug("d3d11device not null");
                        ID3D11ShaderResourceView *srv = CreateTextureFromBytes(
                            SwapchainHook::d3d11Device,
                            head.data(),
                            8, 8
                        );

                        if (srv) {
                            Logger::debug("resource loaded!");
                            ImDrawList *drawList = ImGui::GetForegroundDrawList();
                            if (drawList != nullptr) {
                                Logger::debug("drawing!");
                                drawList->AddImage(
                                    reinterpret_cast<ImTextureID>(srv),
                                    ImVec2(fakex + keycardSize / 2.f, realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize)), // Top-left corner (x, y)
                                    ImVec2((fakex + keycardSize / 2.f) + 8, (realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize)) + 8) // Bottom-right corner (x + width, y + height)
                                );
                            }

                            srv->Release();
                            Logger::debug("released");
                        }
                    }
                }

                // PLAYER HEAD END

                FlarialGUI::FlarialTextWithFont(fakex + xx + Constraints::SpacingConstraint(0.5, keycardSize), realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize), String::StrToWStr(name).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                if ((i + 1) % 10 == 0) {
                    realcenter.y -= Constraints::SpacingConstraint(0.70, keycardSize) * 10;
                    fakex += columnx[i / 10];
                }
            }

            if (getOps<bool>("playerCount")) {
                float curY = vec2.y + totalHeight - 1.75f * keycardSize;

                static float p1 = 0.175;
                static float p2 = 0.196;
                static float p3 = 0.7;
                static float p4 = 0.77;

                int imageResource = roleLogos["Default"];
                for (const auto &[role, resource]: roleLogos) {
                    if (APIUtils::hasRole(role, curPlayer)) {
                        imageResource = resource;
                        break;
                    }
                }

                FlarialGUI::FlarialTextWithFont((MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f), curY, L"Player:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                ImVec2 part1Metrics = FlarialGUI::getFlarialTextSize(L"Player:_", keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                FlarialGUI::image(imageResource, D2D1::RectF(
                                      (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x + Constraints::SpacingConstraint(p1, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p2, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize),
                                      ((MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x) + Constraints::SpacingConstraint(p3, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      (curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p4, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize))));
                FlarialGUI::FlarialTextWithFont((MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f) + part1Metrics.x + Constraints::SpacingConstraint(0.5, keycardSize), curY, FlarialGUI::to_wide(curPlayer).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                curY += Constraints::SpacingConstraint(0.70, keycardSize);
                FlarialGUI::FlarialTextWithFont(MC::windowSize.x / 2.f, curY, FlarialGUI::to_wide(countTxt).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }
        }
    }
}

void TabList::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    std::string text;
    this->normalRender(20, text);
}

void TabList::onKey(const KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        keybindActions[0]({});
    }

    if (!this->isKeybind(event.keys)) this->active = false;
}
