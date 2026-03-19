#include "ClickGUI.hpp"

#include <random>
#include <algorithm>
#include <Scripting/ScriptManager.hpp>
#include <Scripting/ModuleScript.hpp>

#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp"
#include "Utils/PlatformUtils.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

std::chrono::time_point<std::chrono::high_resolution_clock> ClickGUI::favoriteStart;

ClickGUI::HudFadeGuard::HudFadeGuard()
    : vtxStart(ImGui::GetBackgroundDrawList()->VtxBuffer.Size) {}

ClickGUI::HudFadeGuard::~HudFadeGuard() {
    if (hudOpacity >= 1.0f) return;
    auto* drawList = ImGui::GetBackgroundDrawList();
    for (int i = vtxStart; i < drawList->VtxBuffer.Size; i++) {
        ImU32& col = drawList->VtxBuffer[i].col;
        ImU32 a = (col >> IM_COL32_A_SHIFT) & 0xFF;
        a = static_cast<ImU32>(a * hudOpacity);
        col = (col & ~(static_cast<ImU32>(0xFF) << IM_COL32_A_SHIFT)) | (a << IM_COL32_A_SHIFT);
    }
}

namespace {
    constexpr size_t FRAME_COUNT = 7;
    size_t currentFrame = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate = std::chrono::high_resolution_clock::now();

    // Color mapping: base color -> {primary, secondary}
    struct ColorPair {
        char primary;
        char secondary;
    };

    const std::map<char, ColorPair> colorMap = {
        {'b', {'3', 'b'}}, // Dev: §b -> §3 and §b
        {'f', {'7', 'f'}}, // Staff: §f -> §7 and §f
        {'u', {'d', 'u'}}, // Gamer: §u -> §d and §u (using magenta/light purple)
        {'a', {'2', 'a'}}, // Media: §a -> §2 and §a
        {'d', {'5', 'd'}}, // Booster: §d -> §5 and §d
        {'5', {'d', '5'}}, // Supporter: §5 -> §d and §5
        {'4', {'c', '4'}}  // Regular: §4 -> §c and §4
    };

    // Generate animation frame based on primary and secondary color codes
    std::string getAnimationFrame(size_t frame, char primaryColor, char secondaryColor) {
        std::string frames[7] = {
            std::string("§") + primaryColor + "FLA§" + secondaryColor + "RIAL",
            std::string("§") + secondaryColor + "F§" + primaryColor + "LAR§" + secondaryColor + "IAL",
            std::string("§") + secondaryColor + "FL§" + primaryColor + "ARI§" + secondaryColor + "AL",
            std::string("§") + secondaryColor + "FLA§" + primaryColor + "RIA§" + secondaryColor + "L",
            std::string("§") + secondaryColor + "FLAR§" + primaryColor + "IAL",
            std::string("§") + primaryColor + "F§" + secondaryColor + "LARI§" + primaryColor + "AL",
            std::string("§") + primaryColor + "FL§" + secondaryColor + "ARIA§" + primaryColor + "L"
        };
        return frames[frame];
    }
}

// Moved static method implementations from header for better compilation performance
D2D_COLOR_F ClickGUI::getColor(const std::string& text) {
    if(!clickgui->settings.getSettingByName<bool>(text + "RGB")) return D2D1::ColorF(D2D1::ColorF::White);
    D2D_COLOR_F col = clickgui->settings.getSettingByName<bool>(text + "RGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>(text + "Col")->value);
    col.a = clickgui->settings.getSettingByName<float>(text + "Opacity")->value;
    return col;
}

D2D_COLOR_F ClickGUI::getFormatColor(const std::string& colorName) {
    // Convert to lowercase for consistent lookup
    std::string lowerName = colorName;
    for (char& c : lowerName) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    std::string settingName = "formatColor_" + lowerName;

    // Check if the setting exists
    auto* colSetting = clickgui->settings.getSettingByName<std::string>(settingName + "Col");
    if (!colSetting) {
        // Return white as fallback if setting doesn't exist
        return D2D1::ColorF(D2D1::ColorF::White);
    }

    // Get color similar to getColor but for format colors
    bool useRGB = false;
    auto* rgbSetting = clickgui->settings.getSettingByName<bool>(settingName + "RGB");
    if (rgbSetting) {
        useRGB = rgbSetting->value;
    }

    D2D_COLOR_F col = useRGB ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(colSetting->value);

    auto* opacitySetting = clickgui->settings.getSettingByName<float>(settingName + "Opacity");
    if (opacitySetting) {
        col.a = opacitySetting->value;
    }

    return col;
}

// Moved constructor implementation from header
ClickGUI::ClickGUI() : Module("ClickGUI", "What do you think it is?",
    IDR_CLICKGUI_PNG, "K", false, {"theme", "key", "promotions", "watermark", "logo", "spam"}
) {
    this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
    scrollInfo["modules"] = {0, 0};
    scrollInfo["scripting"] = {0, 0};
    scrollInfo["settings"] = {0, 0};

    Listen(this, MouseEvent, &ClickGUI::onMouse)
    //Listen(this, FOVEvent, &ClickGUI::fov)
    Listen(this, KeyEvent, &ClickGUI::onKey)
    ListenOrdered(this, PacketEvent, &ClickGUI::onPacketReceive, EventOrder::IMMEDIATE)
    ListenOrdered(this, RenderEvent, &ClickGUI::onRender, EventOrder::IMMEDIATE)
    Listen(this, SetupAndRenderEvent, &ClickGUI::onSetupAndRender)
    Listen(this, DrawTextEvent, &ClickGUI::onDrawText)
    //Module::onEnable();
}

void ClickGUI::onDrawText(DrawTextEvent &event) {
    std::string* text = event.getText();

    // Look for "FLARIAL" preceded by a color code
    size_t pos = text->find("FLARIAL");
    if (pos != std::string::npos && pos >= 3) {
        // Check if there's a color code before "FLARIAL" (§ is 2 bytes + 1 byte for color code)
        // § symbol is at pos-3 and pos-2, color character is at pos-1
        if ((*text)[pos - 3] == '\xC2' && (*text)[pos - 2] == '\xA7') {
            char colorCode = (*text)[pos - 1]; // Get the color character (the one right before "FLARIAL")

            // Look up the color mapping
            auto it = colorMap.find(colorCode);
            if (it == colorMap.end()) {
                return; // Don't animate if not a supported color
            }

            char primaryColor = it->second.primary;
            char secondaryColor = it->second.secondary;

            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(now - lastUpdate).count();

            if (elapsed > 0.15) {
                currentFrame = (currentFrame + 1) % FRAME_COUNT;
                lastUpdate = now;
            }

            // Find the start of the color-coded FLARIAL (including the § symbol)
            size_t startPos = pos - 3; // Go back to the § character
            size_t length = 10; // Length of "§XFLARIAL" (2 bytes for §, 1 for color, 7 for FLARIAL)

            // Replace with the animated frame
            text->replace(startPos, length, getAnimationFrame(currentFrame, primaryColor, secondaryColor));
        }
    }
}


//random clickgui fov animation i found cool
//i guess worth noting, i made this in like 10 minutes

/*float rFov = 70.0f;
float cFov = 70.0f;
float maxFov = 110.0f;
bool firsttime = true;

void ClickGUI::fov(FOVEvent& event) {
	rFov = event.getFOV();
	auto player = SDK::clientInstance->getLocalPlayer();
	if (!player) return;

	if (ModuleManager::getModule("Java Dynamic FOV").get()->isEnabled()) {
		if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
			rFov = ModuleManager::getModule("Java Dynamic FOV").get()->getOps<float>("fov_target");
		}
	}


	if (this->active) {
		if (firsttime) { cFov = rFov; firsttime = false; maxFov = rFov + 10.0f; }

		FlarialGUI::lerp(cFov, maxFov, 0.20f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
	} else {
		firsttime = true;
		FlarialGUI::lerp(cFov, rFov, 0.20f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
	}
	event.setFOV(cFov);
};*/

size_t ClickGUI::sanitizedToRawIndex(std::string_view raw, size_t sanIdx) {
    size_t rawIdx = 0, visible = 0;

    while ((rawIdx < raw.length()) && (visible < sanIdx)) {
        const auto b0 = static_cast<uint8_t>(raw[rawIdx]);
        if (
            ((rawIdx + 2) < raw.length()) &&
            (b0 == section1stPart) &&
            (static_cast<uint8_t>(raw[rawIdx + 1]) == section2ndPart)
            ) {
            rawIdx += 3; // skip section symbol (2 bytes) + 1 code byte
            continue;
        }
        ++rawIdx;
        ++visible;
    }

    return rawIdx; // raw insertion point corresponding to sanitized index
}

/*
std::string& ClickGUI::getMutableTextForWatermark(TextPacket& pkt) {
    return ((pkt.type == TextPacketType::CHAT) && !pkt.name.empty()) ? pkt.name : pkt.message;
}
*/

// ============================================================================
// Chat [FLARIAL] watermark - prefixes Flarial usernames with a colored tag
// ============================================================================
void ClickGUI::onPacketReceive(PacketEvent& event) {
    try {
        if (!SDK::clientInstance) return;
        auto* player = SDK::clientInstance->getLocalPlayer();
        if (!player || !player->getLevel()) return;
        if (event.getPacket()->getId() != MinecraftPacketIds::Text) return;

        // Read-only view for initial validation
        auto pktView = getTextPacket(event.getPacket());
        if (!pktView) return;

        if (pktView->message == "\uE250 \uE251") {
            event.cancel(); // remove onix promotion on zeqa
            return;
        }

        if (Client::settings.getSettingByName<bool>("nochaticon")->value) return;

        // ----------------------------------------------------------------
        // Get mutable pointers directly into the original packet's fields.
        // Modifying via insert() avoids copy + write-back entirely.
        // ----------------------------------------------------------------
        std::string* nameField = nullptr;
        std::string* msgField  = nullptr;

        if (VersionUtils::checkAboveOrEqual(21, 130)) {
            auto* p = static_cast<TextPacket_v21130*>(static_cast<void*>(event.getPacket()));
            if (std::holds_alternative<std::string>(p->data)) {
                nameField = &p->str;
                msgField  = &std::get<std::string>(p->data);
            } else {
                msgField = &p->str;
            }
        } else {
            auto* p = static_cast<TextPacket*>(event.getPacket());
            nameField = &p->name;
            msgField  = &p->message;
        }

        if (!msgField) return;

        // Lambda: build the colored [FLARIAL] prefix for a given username
        auto getPrefix = [](const std::string& name) -> std::optional<std::string> {
            for (const auto& [role, color] : roleColors)
                if (APIUtils::hasRole(role, name))
                    return std::format("{}{}{}",  "§r§f[", color, "FLARIAL§f]§r ");
            return std::nullopt;
        };

        // Build set of currently online players for validation
        std::unordered_set<std::string> onlinePlayers;
        for (auto& pair : player->getLevel()->getPlayerMap()) {
            if (!pair.second.name.empty())
                onlinePlayers.insert(String::removeColorCodes(pair.second.name));
        }

        // ------------------------------------------------------------------
        // 1) Name field: prefix the sender's name if they are a Flarial user
        //    BUT only if their name does NOT also appear in the message body
        //    (the message-body scan below would handle it, causing a dupe).
        // ------------------------------------------------------------------
        if (nameField && !nameField->empty()) {
            const std::string cleanName = String::removeColorCodes(*nameField);
            bool isFlarialUser = std::ranges::any_of(APIUtils::onlineUsers,
                [&](const std::string& u) { return u == cleanName; });

            if (isFlarialUser) {
                const std::string cleanMsg = String::removeColorCodes(*msgField);
                if (cleanMsg.find(cleanName) == std::string::npos) {
                    auto prefix = getPrefix(cleanName);
                    if (prefix) nameField->insert(0, *prefix);
                }
            }
        }

        // ------------------------------------------------------------------
        // 2) Message body: scan for any Flarial usernames and insert tags.
        //    Save the original message before modification for dedup check.
        // ------------------------------------------------------------------
        const std::string originalMsg = *msgField;
        if (lastmesg != originalMsg) {
            const std::string sanitized = String::removeColorCodes(msgField->c_str());

            // Collect all username occurrences in the sanitized message
            std::vector<std::pair<std::string, size_t>> hits;
            bool singleOnly = Client::settings.getSettingByName<bool>("singlewatermark")->value;
            bool breakloop = false;

            for (const auto& user : APIUtils::onlineUsers) {
                if (breakloop) break;
                size_t pos = 0;
                while ((pos = sanitized.find(user, pos)) != std::string::npos) {
                    const size_t matchPos = pos;

                    // Check for @mention prefix and adjust hit position
                    size_t hitPos = pos;
                    if (pos >= 2 && sanitized[pos - 1] == '"' && sanitized[pos - 2] == '@')
                        hitPos = pos - 2;
                    else if (pos >= 1 && (sanitized[pos - 1] == '@' || sanitized[pos - 1] == '"'))
                        hitPos = pos - 1;

                    // Skip if this specific occurrence is already tagged.
                    // Our tag sanitizes to "[FLARIAL] " (10 chars) placed exactly at hitPos,
                    // so on re-entry the 10 chars immediately before hitPos are the tag.
                    if (hitPos >= 10 && sanitized.substr(hitPos - 10, 10) == "[FLARIAL] ") {
                        pos = matchPos + user.length();
                        continue;
                    }

                    hits.emplace_back(user, hitPos);

                    if (singleOnly) { breakloop = true; break; }
                    pos = matchPos + user.length();
                }
            }

            if (!hits.empty()) {
                // Extract formatting codes (§X sequences) preceding a raw index
                auto extractFormat = [&](size_t rawIndex) {
                    std::string fmt;
                    const size_t limit = std::min(rawIndex, msgField->size());
                    for (size_t i = 0; i + 2 <= limit; ++i) {
                        if (static_cast<uint8_t>((*msgField)[i]) == section1stPart &&
                            static_cast<uint8_t>((*msgField)[i + 1]) == section2ndPart) {
                            if (i + 2 < msgField->size()) {
                                fmt.append(*msgField, i, 3);
                                i += 2;
                            }
                        }
                    }
                    return fmt;
                };

                bool allowDupes = Client::settings.getSettingByName<bool>("watermarkduplicates")->value;
                std::unordered_set<std::string> seen;
                std::vector<std::pair<size_t, std::string>> insertions;

                for (auto& [name, sanitizedIdx] : hits) {
                    if (!allowDupes && seen.contains(name)) continue;
                    if (!onlinePlayers.contains(name)) continue;

                    auto prefix = getPrefix(name);
                    if (!prefix) continue;

                    const size_t rawIdx = sanitizedToRawIndex(*msgField, sanitizedIdx);
                    insertions.emplace_back(rawIdx, *prefix + extractFormat(rawIdx));
                    seen.insert(name);
                }

                if (!insertions.empty()) {
                    // Sort right-to-left so earlier insertions don't shift later positions
                    std::ranges::sort(insertions, [](auto& a, auto& b) {
                        return a.first > b.first;
                    });

                    for (auto& [insertPos, text] : insertions) {
                        msgField->insert(std::min(insertPos, msgField->size()), text);
                    }

                    lastmesg = originalMsg;
                }
            }
        }
    }
    catch (...) {
        // Invalid packet data (can happen when exiting world)
    }
}


void ClickGUI::onRender(RenderEvent &event) {
    // Handle deferred close from the key handler thread safely on the render thread.
    // All shared state modifications (strings, maps, etc.) happen here on the render
    // thread to avoid data races with concurrent reads during rendering.
    if (pendingClose.exchange(false)) {
        // Mouse grab + active=false already done immediately in the key handler.
        // Only the unsafe state cleanup (strings, maps, ResetUIState) is deferred here.
        MC::lastMouseScroll = MouseAction::Release;
        page.type = "normal";
        curr = "modules";
        if (pendingSaveScrollPos) {
            saved_acumilatedPos = pendingSavedAccumPos;
            saved_acumilatedBarPos = pendingSavedAccumBarPos;
        }
        FlarialGUI::ResetUIState();
        searchBarString = pendingSearchString;
        // Don't write TextBoxes[0].text here — the GUI is closed, so no widget
        // is reading it. The text will sync naturally when the SearchBar renders
        // again on reopen (via TextBox's inactive-sync path).
        // Writing to TextBoxes while the input thread may be reading was a race.

        // Clear ImGui's active widget and input queue so stale text input
        // doesn't carry over to the next frame's NewFrame() processing.
        if (ImGui::GetCurrentContext()) {
            ImGui::ClearActiveID();
            ImGui::GetIO().ClearInputKeys();
        }
    }

    float watermarkWidth = Constraints::RelativeConstraint(0.65);
    float watermarkHeight = Constraints::RelativeConstraint(0.25);

    if (editmenu) {
        D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::Black);
        c.a = 0.75f;
        FlarialGUI::RoundedRect(0, 0, c, MC::windowSize.x, MC::windowSize.y, 0, 0);
        FlarialGUI::FlarialTextWithFont(MC::windowSize.x / 2.f, 0.01f * MC::windowSize.y,
                                        L"Use the scroll wheel to resize modules by hovering over them.",
                                        0, 0, DWRITE_TEXT_ALIGNMENT_CENTER,
                                        Constraints::RelativeConstraint(0.128, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL, false);
        FlarialGUI::FlarialTextWithFont(MC::windowSize.x / 2.f, 0.035f * MC::windowSize.y,
                                        FlarialGUI::to_wide(
                                            "To disable this menu press ESC or " + getOps<std::string>("editmenubind")).
                                        c_str(),
                                        0, 0, DWRITE_TEXT_ALIGNMENT_CENTER,
                                        Constraints::RelativeConstraint(0.128, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL, false);
    }
    Vec2<float> watermarkPos = Constraints::CenterConstraint(watermarkWidth, watermarkHeight, "y", 1.175, 1.175);
    // TODO: add inventory screen to onRender?
    // watermark
    if (SDK::getCurrentScreen() == "inventory_screen" || SDK::getCurrentScreen().contains("chest"))
        if (Client::settings.getSettingByName<bool>("watermark")->value)
            FlarialGUI::image(IDR_FLARIAL_TITLE_PNG, D2D1::RectF(watermarkPos.x, watermarkPos.y, watermarkPos.x + watermarkWidth,
                                                                 watermarkPos.y + watermarkHeight));


    if (FlarialGUI::scrollposmodifier == 0) {
        FlarialGUI::scrollposmodifier = Constraints::RelativeConstraint(0.1f);
    }

    // When viewing a module's settings page, lerp blur out so HUD modules
    // smoothly reappear and sliders can be previewed in real time.
    const bool onSettingsPage = (page.type != "normal");

    if (this->active) {
        setEnabled(true);
        float openFactor = floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f;
        FlarialGUI::lerp(baseHeightActual, 0.64f, 0.18f * openFactor);

        if (onSettingsPage) {
            // Fade blur out when entering a settings page
            FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.18f * openFactor);
        } else {
            FlarialGUI::lerp(realBlurAmount, Client::settings.getSettingByName<float>("blurintensity")->value,
                             0.18f * openFactor);
        }
    } else {
        float closeFactor = floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f;
        FlarialGUI::lerp(baseHeightReal, 0.0001f, 0.22f * closeFactor);
        FlarialGUI::lerp(baseHeightActual, 0.00001f, 0.30f * closeFactor);
        FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.30f * closeFactor);

        for (auto &box: FlarialGUI::TextBoxes) box.second.isActive = false;
    }

    // Render blur when there's any meaningful blur amount.
    bool shouldRenderBlur = !editmenu && realBlurAmount > 0.05f;
    if (shouldRenderBlur)
        Blur::RenderBlur(event.RTV, 3, realBlurAmount);

    // Compute HUD fade opacity — inverse of blur progress.
    // As blur fades in, HUD elements smoothly fade out and vice versa.
    float maxBlur = Client::settings.getSettingByName<float>("blurintensity")->value;
    if (!editmenu && maxBlur > 0.001f) {
        hudOpacity = 1.0f - std::clamp(realBlurAmount / maxBlur, 0.0f, 1.0f);
    } else {
        hudOpacity = 1.0f;
    }
    blurActive = hudOpacity <= 0.01f;

    if (SwapchainHook::init && baseHeightActual > 0.1f) {
        /* Base Rectangle Start */

        float baseWidth = Constraints::RelativeConstraint(0.81);

        if (this->active) {
            FlarialGUI::lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
        }


        float baseHeight = Constraints::RelativeConstraint(baseHeightReal);

        Vec2<float> center = Constraints::CenterConstraint(baseWidth,
                                                           Constraints::RelativeConstraint(baseHeightReal), "r",
                                                           1, 1);
        Vec2<float> round = Constraints::RoundingConstraint(43, 43);
        Vec2<float> baseRound = round;

        D2D1_COLOR_F basebaseRectangleColor = ClickGUI::getColor("secondary3");

        FlarialGUI::RoundedRect(center.x, center.y,
                                basebaseRectangleColor, baseWidth,
                                Constraints::RelativeConstraint(baseHeightReal), round.x, round.x);


        FlarialGUI::PushSize(center.x, center.y, baseWidth, Constraints::RelativeConstraint(baseHeightActual));

        /* Base Rectangle End */


        /* Nav Bar Start */

        float navigationBarWidth = Constraints::RelativeConstraint(1.235f);
        float navigationBarHeight = Constraints::RelativeConstraint(0.124f);
        float navx = Constraints::PercentageConstraint(0.013f, "left");
        float navy = Constraints::PercentageConstraint(0.019f, "top");
        round = Constraints::RoundingConstraint(28, 28);

        D2D1_COLOR_F navColor = ClickGUI::getColor("secondary2");

        FlarialGUI::RoundedRect(navx, navy, navColor,
                                navigationBarWidth, navigationBarHeight, round.x, round.x);


        FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

        /* Nav Bar End */

        /* Logo Start */


        float logoWidth = Constraints::RelativeConstraint(1.21);
        float logoWidthButReal = Constraints::RelativeConstraint(0.5);

        float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidthButReal) + (logoWidthButReal * 0.6);
        float logoY = (navy + navigationBarHeight / 2.0f - logoWidthButReal / 2.0f);

        D2D1_COLOR_F fLARIALlOGO = ClickGUI::getColor("flariallogo");

        if (!Client::settings.getSettingByName<bool>("noicons")->value) {
            if (ModuleManager::getModule("Lewis")->getOps<bool>("lewislogo"))
                FlarialGUI::image(
                    IDR_LEWIS_PNG, D2D1::RectF(logoX - (logoWidthButReal * 0.2), logoY - (logoWidthButReal * 0.2),
                                               logoX + (logoWidthButReal * 1.4), logoY + (logoWidthButReal * 1.4)),
                    "PNG");
            else
                FlarialGUI::image(
                    IDR_WHITE_LOGO_PNG, D2D1::RectF(logoX, logoY, logoX + logoWidthButReal, logoY + logoWidthButReal),
                    "PNG", true, FlarialGUI::D2DColorToImColor(fLARIALlOGO));
        }
        FlarialGUI::Tooltip("easter egg", logoX, logoY, "Never gonna give you up", logoWidthButReal, logoWidthButReal);

        /* Logo End */

        /* tab buttons start */

        D2D1_COLOR_F RadioButtonEnabled = ClickGUI::getColor("enabledRadioButton");
        D2D1_COLOR_F RadioButtonDisabled = ClickGUI::getColor("disabledRadioButton");

        float tabBaseWidth = Constraints::RelativeConstraint(0.448f);

        float RadioButtonWidth = Constraints::RelativeConstraint(0.134, "width");
        float RadioButtonHeight = tabBaseWidth;

        if (curr == "modules") FlarialGUI::lerp(width1, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(width1, tabBaseWidth, 0.15f * FlarialGUI::frameFactor);

        if (curr == "settings") FlarialGUI::lerp(width2, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(width2, tabBaseWidth, 0.15f * FlarialGUI::frameFactor);

        if (curr == "scripting") FlarialGUI::lerp(width3, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(width3, tabBaseWidth, 0.15f * FlarialGUI::frameFactor);

        float radioX = navx - Constraints::SpacingConstraint(-0.85, logoWidth);
        float radioY(navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

        round = Constraints::RoundingConstraint(17.5f, 17.5f);

        D2D1_COLOR_F modTextCol = ClickGUI::getColor("globalText");

        D2D1_COLOR_F mod6Col = ClickGUI::getColor("secondary6");
        static D2D1_COLOR_F tabBgCol = ClickGUI::getColor("secondary6");
        static D2D1_COLOR_F tabBgCol2 = ClickGUI::getColor("secondary6");
        static D2D1_COLOR_F tabBgCol3 = ClickGUI::getColor("secondary6");
        static D2D1_COLOR_F tabBgCol4 = ClickGUI::getColor("secondary6");

        D2D_COLOR_F secondary8 = ClickGUI::getColor("secondary8");
        D2D_COLOR_F secondary6 = ClickGUI::getColor("secondary6");

        //radiobutton of modules

        if (curr != "modules") tabBgCol = FlarialGUI::LerpColor(tabBgCol, secondary8, 0.15f * FlarialGUI::frameFactor);
        else tabBgCol = FlarialGUI::LerpColor(tabBgCol, secondary6, 0.15f * FlarialGUI::frameFactor);

        FlarialGUI::ShadowRect(Vec2(radioX, radioY + Constraints::SpacingConstraint(0.015f, logoWidth)),
                               Vec2{width1, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)},
                               D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

        if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(1, radioX, radioY, tabBgCol,
                modTextCol, L"Modules",
                width1,
                RadioButtonHeight, round.x,
                round.x, "modules", curr)) {
            auto &_scrollData = scrollInfo[curr];
            _scrollData.scrollpos = FlarialGUI::scrollpos;
            _scrollData.barscrollpos = FlarialGUI::barscrollpos;

            curr = "modules";
            page.type = "normal";

            // Save search string before ResetUIState clears TextBoxes
            std::string savedSearch = searchBarString;
            FlarialGUI::ResetUIState();
            // Restore the search state so filtering continues to work
            searchBarString = savedSearch;
            FlarialGUI::TextBoxes[0].text = savedSearch;

            auto &scrollData = scrollInfo[curr];

            FlarialGUI::scrollpos = scrollData.scrollpos;
            FlarialGUI::barscrollpos = scrollData.barscrollpos;
            accumilatedPos = scrollData.scrollpos;
            accumilatedBarPos = scrollData.barscrollpos;

            if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                accumilatedPos = saved_acumilatedPos;
                accumilatedBarPos = saved_acumilatedBarPos;
            }
        }


        logoWidth = tabBaseWidth * 0.625f;

        radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
        radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

        if (!Client::settings.getSettingByName<bool>("noicons")->value) {
            if (curr == "modules")
                FlarialGUI::image(
                    IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true,
                    FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
            else
                FlarialGUI::image(
                    IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true,
                    FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
        }

        // radiobutton of settings

        radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
        radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

        if (curr != "settings")
            tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, secondary8,
                                              0.15f * FlarialGUI::frameFactor);
        else tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, secondary6, 0.15f * FlarialGUI::frameFactor);


        radioX += Constraints::SpacingConstraint(3.24f, logoWidth);
        round = Constraints::RoundingConstraint(17.5f, 17.5f);

        radioPushAmount1 = Constraints::SpacingConstraint(0.9f, logoWidth) + width1;
        radioX += radioPushAmount1;

        FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)},
                               Vec2{width2, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)},
                               D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

        if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(0, radioX, radioY,
                tabBgCol2,
                modTextCol, L"Settings",
                width2,
                RadioButtonHeight, round.x,
                round.x, "settings",
                curr)) {
            auto &_scrollData = scrollInfo[curr];
            _scrollData.scrollpos = FlarialGUI::scrollpos;
            _scrollData.barscrollpos = FlarialGUI::barscrollpos;

            FlarialGUI::TextBoxes[0].isActive = false;
            curr = "settings";

            saved_acumilatedPos = accumilatedPos;
            saved_acumilatedBarPos = accumilatedBarPos;

            auto &scrollData = scrollInfo[curr];

            FlarialGUI::scrollpos = scrollData.scrollpos;
            FlarialGUI::barscrollpos = scrollData.barscrollpos;
            accumilatedPos = scrollData.scrollpos;
            accumilatedBarPos = scrollData.barscrollpos;
        }

        radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
        radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

        if (!Client::settings.getSettingByName<bool>("noicons")->value) {
            if (curr == "settings")
                FlarialGUI::image(
                    IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
            else
                FlarialGUI::image(
                    IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
        }

        const float h = Constraints::RelativeConstraint(0.42, "height");
        const float searchBarY = (navy + navigationBarHeight / 2.0f - h / 2.0f);
        ClickGUIElements::SearchBar(0, searchBarString, 12, Constraints::PercentageConstraint(0.022, "right"),
                                    searchBarY);


        // radiobutton of scripting

        radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
        radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

        radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 5.69f, logoWidth) + width1 + width2;
        radioX += radioPushAmount2;
        round = Constraints::RoundingConstraint(17.5f, 17.5f);

        if (curr != "scripting") {
            tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, secondary8, 0.15f * FlarialGUI::frameFactor);
        } else {
            tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, secondary6, 0.15f * FlarialGUI::frameFactor);
        }

        FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)},
                               Vec2{width3, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)},
                               D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
        if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(3, radioX, radioY,
                tabBgCol4,
                modTextCol, L"Scripts",
                width3,
                RadioButtonHeight, round.x,
                round.x, "scripting",
                curr)) {
            auto &_scrollData = scrollInfo[curr];
            _scrollData.scrollpos = FlarialGUI::scrollpos;
            _scrollData.barscrollpos = FlarialGUI::barscrollpos;

            curr = "scripting";
            page.type = "normal";

            // Save search string before ResetUIState clears TextBoxes
            std::string savedSearch = searchBarString;
            FlarialGUI::ResetUIState();
            // Restore the search state so filtering continues to work
            searchBarString = savedSearch;
            FlarialGUI::TextBoxes[0].text = savedSearch;

            auto &scrollData = scrollInfo[curr];

            FlarialGUI::scrollpos = scrollData.scrollpos;
            FlarialGUI::barscrollpos = scrollData.barscrollpos;
            accumilatedPos = scrollData.scrollpos;
            accumilatedBarPos = scrollData.barscrollpos;
        }

        radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
        radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

        if (!Client::settings.getSettingByName<bool>("noicons")->value) {
            if (curr == "scripting")
                FlarialGUI::image(
                    IDR_SCRIPTING_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true, FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
            else
                FlarialGUI::image(
                    IDR_SCRIPTING_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true, FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
        }

        // radiobutton of editmenu

        radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
        radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

        radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 6.45f, logoWidth) + width1 + width2 + width3;
        radioX += radioPushAmount2;
        round = Constraints::RoundingConstraint(17.5f, 17.5f);

        if (curr != "editmenu") {
            tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, secondary8, 0.15f * FlarialGUI::frameFactor);
        } else {
            tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, secondary6, 0.15f * FlarialGUI::frameFactor);
        }

        FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)},
                               Vec2{tabBaseWidth, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)},
                               D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
        if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(2, radioX, radioY,
                tabBgCol3,
                modTextCol, L"",
                tabBaseWidth,
                RadioButtonHeight, round.x,
                round.x, "editmenu",
                curr)) {
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            this->active = false;
            Client::SaveSettings();
            // FlarialGUI::Notify("Right click a module to directly go to their settings page.");
            // FlarialGUI::Notify("To disable this menu press ESC or " + getOps<std::string>("editmenubind"));

            editmenu = true;
        }

        radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
        radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

        if (!Client::settings.getSettingByName<bool>("noicons")->value) {
            if (curr == "editmenu")
                FlarialGUI::image(
                    IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true,
                    FlarialGUI::D2DColorToImColor(RadioButtonDisabled));
            else
                FlarialGUI::image(
                    IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth), "PNG",
                    true,
                    FlarialGUI::D2DColorToImColor(RadioButtonEnabled));
        }

        /* tab buttons end */

        FlarialGUI::PopSize(); // Pops nav bar
        /* Mod Card Start */

        std::string e = curr;

        if (page.type == "normal" && e == "modules") {
            FlarialGUI::lerp(modcardOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(modcardOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
        } else {
            FlarialGUI::lerp(modcardOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(modcardOffset, -Constraints::RelativeConstraint(2.f, "height", true),
                             0.08f * FlarialGUI::frameFactor);
        }

        if (page.type == "normal" && e == "scripting") {
            FlarialGUI::lerp(scriptingOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(scriptingOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
        } else {
            FlarialGUI::lerp(scriptingOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(scriptingOffset, -Constraints::RelativeConstraint(4.f, "height", true),
                             0.08f * FlarialGUI::frameFactor);
        }


        if (e == "settings" || page.type == "settings") {
            FlarialGUI::lerp(settingsOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(settingsOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
        } else {
            FlarialGUI::lerp(settingsOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
            FlarialGUI::lerp(settingsOffset, Constraints::RelativeConstraint(2.f, "height", true),
                             0.08f * FlarialGUI::frameFactor);
        }


        if (page.type == "normal") {
            /* MODCARD RENDER */

            if (modcardOpacity > 0.05f) {
                float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.58,
                                                                      -0.52);

                FlarialGUI::PushSize(center.x, center.y, baseWidth,
                                     Constraints::RelativeConstraint(baseHeightReal, "height", true));

                float scrollWidth = Constraints::RelativeConstraint(1.12);
                float scrollHeight = Constraints::RelativeConstraint(0.84);
                Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0,
                                                                         1);

                FlarialGUI::PopSize();

                if (ModuleManager::cguiRefresh && ScriptManager::initialized && ModuleManager::initialized) {
                    ModuleManager::updateModulesVector();
                    ModuleManager::cguiRefresh = false;
                }
                auto modules = ModuleManager::modulesVector;

                // Count how many modules will actually be displayed (accounting for search filter)
                int visibleModuleCount = 0;
                if (!searchBarString.empty()) {
                    std::string search = searchBarString;
                    for (char &c: search) {
                        c = (char) std::tolower(c);
                    }
                    for (const auto &pModule: modules) {
                        std::string name = pModule->name;
                        for (char &c: name) {
                            c = (char) std::tolower(c);
                        }
                        bool showMod = name.starts_with(search) || name.find(search) != std::string::npos;
                        if (!showMod && !pModule->aliases.empty()) {
                            for (const std::string& alias : pModule->aliases) {
                                if (String::toLower(alias).starts_with(search) ||
                                    String::toLower(alias).find(search) != std::string::npos) {
                                    showMod = true;
                                    break;
                                }
                            }
                        }
                        if (showMod) visibleModuleCount++;
                    }
                } else {
                    visibleModuleCount = static_cast<int>(modules.size());
                }

                // Calculate content height based on visible modules (3 per row)
                int i3 = 0;
                float i2 = 0;
                for (int idx = 0; idx < visibleModuleCount; idx++) {
                    if ((++i3 % 3) == 0) {
                        i2 += Constraints::SpacingConstraint(0.8, modWidth);
                    }
                }

                FlarialGUI::ScrollBar(120, scrollcenter.y, 10, modHeight + i2, 2);

                FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                float xModifier = 0.0f;
                float yModifier = 0.0f;

                int i = 0;

                for (const auto &pModule: modules) {
                    bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                   (modcenter.y + yModifier + FlarialGUI::scrollpos - 300) <
                                   center.y + Constraints::RelativeConstraint(baseHeightReal);

                    if (!searchBarString.empty()) {
                        std::string name = pModule->name;

                        for (char &c: name) {
                            c = (char) std::tolower(c);
                        }

                        std::string search = searchBarString;

                        for (char &c: search) {
                            c = (char) std::tolower(c);
                        }

                        bool showMod = false;

                        if (
                            name.starts_with(search) ||
                            name.find(search) != std::string::npos
                        ) showMod = true;

                        if (!showMod && !pModule->aliases.empty()) {
                            for (const std::string& alias : pModule->aliases) {
                                if (
                                    String::toLower(alias).starts_with(search) ||
                                    String::toLower(alias).find(search) != std::string::npos
                                ) showMod = true;
                            }
                        }

                        if (showMod) {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier,
                                                      pModule.get(),
                                                      pModule->icon, i, visible);
                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }
                        }
                    } else {
                        ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier,
                                                  pModule.get(),
                                                  pModule->icon, i, visible);

                        xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                        if ((++i % 3) == 0) {
                            yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                            xModifier = 0.0f;
                        }
                    }
                }

                FlarialGUI::UnsetScrollView();

                D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                shadowCol.a = modcardOpacity;
                //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f,
                                                       center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                FlarialGUI::ShadowRect(
                    Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight},
                    Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                FlarialGUI::PopImClipRect();

                //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                FlarialGUI::PushImClipRect(D2D1::RectF(
                    center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f,
                    center.x + baseWidth,
                    center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) +
                    Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                FlarialGUI::ShadowRect(
                    Vec2{
                        center.x + Constraints::SpacingConstraint(0.15f, baseWidth),
                        center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)
                    }, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50,
                    100);
                FlarialGUI::PopImClipRect();

                //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
            }

            /* MODCARD RENDER END */

            /* SETTINGS RENDER */

            if (settingsOpacity > 0.05f) {
                FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
                float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
                float rectY = Constraints::PercentageConstraint(0.167, "top");
                float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                float rectHeight = Constraints::RelativeConstraint(0.85);
                round = Constraints::RoundingConstraint(38.f, 38.5f);

                float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                D2D1_COLOR_F colorThing = ClickGUI::getColor("secondary2");

                D2D1_RECT_F rect = {rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
                FlarialGUI::PushImClipRect(rect);

                FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                        anotherRectWidth, anotherRectHeight, round.x, round.x);

                FlarialGUI::PopImClipRect();

                D2D1_COLOR_F bruv = ClickGUI::getColor("secondary1");
                D2D1_COLOR_F textCol = ClickGUI::getColor("globalText");

                FlarialGUI::PopSize();

                FlarialGUI::PushSize(rectXNoOff + Constraints::SpacingConstraint(0.0085, rectWidth),
                                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                     rectHeight);


                float toggleX = Constraints::PercentageConstraint(0.019, "left");
                float toggleY = Constraints::PercentageConstraint(0.10, "top");

                const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


                FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(20.5, scrollviewWidth), 2);
                FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                          Constraints::RelativeConstraint(1.0, "width"),
                                          Constraints::RelativeConstraint(0.88f, "height"));

                FlarialGUI::PopSize();

                FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                     rectHeight);

                if (Client::activeConfig.empty())
                    Client::activeConfig = Client::settings.getSettingByName<std::string>(
                        "currentConfig")->value;

                if (Client::activeConfig != Client::settings.getSettingByName<std::string>("currentConfig")->value &&
                    Client::activeConfig != "") {
                    Client::switchConfig(Client::settings.getSettingByName<std::string>("currentConfig")->value);
                    ModuleManager::restartModules = true;
                    FlarialGUI::UnsetScrollView();
                    FlarialGUI::PopSize();
                    FlarialGUI::PopSize();
                    return;
                }
                Module *c = this->ghostMainModule;
                c->addHeader("Config Manager");
                c->addDropdown("Selected Config", "", Client::availableConfigs,
                               Client::settings.getSettingByName<std::string>("currentConfig")->value);
                c->addButton("Add a new config", "", "ADD", []() {
                    std::random_device rd;
                    std::mt19937 gen(rd());

                    std::uniform_int_distribution<> distrib(1000, 9000);

                    int random_number = distrib(gen);
                    std::string configname = "config-" + std::to_string(random_number) + ".json";
                    Client::createConfig(configname);
                    FlarialGUI::Notify("Created & loaded: " + configname);
                    ScriptMarketplace::reloadAllConfigs();
                });
                c->addButton("Remove selected config", "DELETES YOUR CURRENT CONFIG", "DELETE", []() {
                    if (Client::settings.getSettingByName<std::string>("currentConfig")->value != "default.json") {
                        std::string to = Client::settings.getSettingByName<std::string>("currentConfig")->value;
                        Client::deleteConfig(to);
                        ScriptMarketplace::reloadAllConfigs();
                        FlarialGUI::Notify("Deleted " + to);
                    } else {
                        FlarialGUI::Notify("Cannot delete default config.");
                    }
                });
                c->addButton("Reload Configs", "Reloads the configs of all modules.", "RELOAD", []() {
                    ScriptMarketplace::reloadAllConfigs();
                    std::string to = Client::settings.getSettingByName<std::string>("currentConfig")->value;
                    FlarialGUI::Notify("Reloaded " + to);
                });
                c->extraPadding();

                c->addHeader("Keybinds");
                c->addKeybind("Eject Keybind", "",
                              Client::settings.getSettingByName<std::string>("ejectKeybind")->value);

                c->extraPadding();

                c->addHeader("Fonts");
                c->addToggle("Pixelate Fonts (Enable For MC Font)", "Reload Minecraft to take full effect",
                             Client::settings.getSettingByName<bool>("pixelateFonts")->value);
                c->addTextBox("ClickGUI", "", Client::settings.getSettingByName<std::string>("fontname")->value, 48);
                c->addSlider("Universal Font Scale", "",
                             Client::settings.getSettingByName<float>("gui_font_scale")->value, 2.f, 0.5f, true);
                c->addTextBox("Modules", "", Client::settings.getSettingByName<std::string>("mod_fontname")->value, 48);
                c->addSlider("Universal Font Scale", "",
                             Client::settings.getSettingByName<float>("modules_font_scale")->value, 2.f, 0.5f, true);
                c->addToggle("Override Font Weight", "",
                             Client::settings.getSettingByName<bool>("overrideFontWeight")->value);
                c->addDropdown("Font Weight", "Bold, Thin, etc.",
                               {"Bold", "Normal", "SemiBold", "ExtraBold", "Medium", "Light", "ExtraLight"},
                               Client::settings.getSettingByName<std::string>("fontWeight")->value);
                c->extraPadding();


                c->addHeader("Rendering");

                if (PlatformUtils::isUWP()) {
                    c->addButton("Force Reload Minecraft", "Forces the swapchain to recreate.", "RELOAD", []() {
                        SwapchainHook::recreate = true;
                    });
                }


                c->addButton("Reload Scripts", "", "RELOAD", [&]() {
                    ModuleManager::restartModules = true;
                });

                c->addElementText("Following Requires Restart");
                c->extraPadding();

                c->addToggle("Better Frames", "RTX Disabled, Restart Required.",
                             Client::settings.getSettingByName<bool>("killdx")->value);

                // VSync Disabler works on both UWP and GDK
                c->addToggle("V-SYNC Disabler", "Restart Required.",
                             Client::settings.getSettingByName<bool>("vsync")->value);

                if (PlatformUtils::isUWP())
                {
                    c->addToggle("Recreate Swapchain At Start",
                                 "May help with Better RenderDragon",
                                 Client::settings.getSettingByName<bool>("recreateAtStart")->value);
                }
                c->extraPadding();

                c->addElementText("Following Does Not Require Restart");
                c->extraPadding();

                c->addToggle("Disable Animations", "", Client::settings.getSettingByName<bool>("disableanims")->value);
                c->addSlider("UI Blur Intensity", "", Client::settings.getSettingByName<float>("blurintensity")->value,
                             25.f);
                c->addSlider("Chroma / RGB Speed", "", Client::settings.getSettingByName<float>("rgb_speed")->value,
                             10.f);
                c->addSlider("Chroma / RGB Saturation", "",
                             Client::settings.getSettingByName<float>("rgb_saturation")->value, 1.f);
                c->addSlider("Chroma / RGB Value", "", Client::settings.getSettingByName<float>("rgb_value")->value,
                             1.f);
                c->addToggle("Inventory Watermark", "", Client::settings.getSettingByName<bool>("watermark")->value);
                c->addToggle("Enabled Modules on Top", "",
                             Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value);
                c->addToggle("No UI Icons / Images", "Flarial only",
                             Client::settings.getSettingByName<bool>("noicons")->value);
                c->addToggle("Disable UI Shadows", "Flarial only",
                             Client::settings.getSettingByName<bool>("noshadows")->value);

                c->extraPadding();

                c->addHeader("Misc");
                c->addToggle("Hide Modules when F1",
                             "Hide Hud will also Hide Modules",
                             Client::settings.getSettingByName<bool>("hideHudAndMods")->value);
                c->addToggle("Save Scroll Position",
                             "Save scroll position in ClickGUI",
                             Client::settings.getSettingByName<bool>("saveScrollPos")->value);
                c->addSlider("Page Up/Down Scroll Speed",
                             "How much to scroll when pressing Page Up or Page Down keys",
                             Client::settings.getSettingByName<float>("pageScrollMultiplier")->value, 15.0f, 1.0f);
                c->addToggle("Auto Search ClickGUI",
                             "Start searching for modules already when you press a key in ClickGUI",
                             Client::settings.getSettingByName<bool>("autosearch")->value);
                c->addToggle("Resettable Settings",
                             "Allows settings to be reset by left clicking and then right clicking",
                             Client::settings.getSettingByName<bool>("resettableSettings")->value);
                c->addToggle("Snapping Lines", "Y'know, those pink lines in edit mode.",
                             Client::settings.getSettingByName<bool>("snappinglines")->value);
                c->addToggle("Center Cursor", "Centers your cursor everytime you open your inventory, etc.",
                             Client::settings.getSettingByName<bool>("centreCursor")->value);
                c->addToggle("Anonymous on API", "Stay anonymous on Flarial's API.",
                             Client::settings.getSettingByName<bool>("anonymousApi")->value);
                c->addToggle("API Usage", "Might disable some features when turned off.",
                             Client::settings.getSettingByName<bool>("apiusage")->value);
                c->addToggle("Promotions", "To donate & join discord. (pls keep on)",
                             Client::settings.getSettingByName<bool>("promotions")->value);
                c->addToggle("No Flarial Logo", "No Logo next to Nametag </3",
                             Client::settings.getSettingByName<bool>("nologoicon")->value);
                c->addToggle("No Flarial Chat Icon", "No [FLARIAL] in chat </3",
                             Client::settings.getSettingByName<bool>("nochaticon")->value);
                c->addConditionalToggle(
                        !Client::settings.getSettingByName<bool>("nochaticon")->value,
                        "Single watermark",
                        "The [FLARIAL] tag will only apply to the first user found in each message.\nThis will be the player sending the messages in a local world.",
                        Client::settings.getSettingByName<bool>("singlewatermark")->value
                );
                c->addConditionalToggle(
                        !Client::settings.getSettingByName<bool>("nochaticon")->value &&
                        !Client::settings.getSettingByName<bool>("singlewatermark")->value,
                        "Watermark duplicate usernames",
                        "Turning this off watermarks only the first instance of each username.",
                        Client::settings.getSettingByName<bool>("watermarkduplicates")->value
                );
                c->addToggle("Clear Text Box When Clicked", "",
                             Client::settings.getSettingByName<bool>("clearTextBoxWhenClicked")->value);

                c->resetPadding();

                FlarialGUI::UnsetScrollView();
                FlarialGUI::PopSize();
            }

            if (scriptingOpacity > 0.05f) {
                float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.58,
                                                                      -0.52);

                FlarialGUI::PushSize(center.x, center.y, baseWidth,
                                     Constraints::RelativeConstraint(baseHeightReal, "height", true));

                float scrollWidth = Constraints::RelativeConstraint(1.12);
                float scrollHeight = Constraints::RelativeConstraint(0.84);
                Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0,
                                                                         1);

                FlarialGUI::PopSize();

                auto scriptModules = ScriptManager::getLoadedModules();

                // Count how many script modules will actually be displayed (accounting for search filter)
                int visibleScriptCount = 0;
                if (!searchBarString.empty()) {
                    std::string search = searchBarString;
                    for (char &c: search) {
                        c = (char) std::tolower(c);
                    }
                    for (const auto &pModule: scriptModules) {
                        std::string name = pModule->name;
                        for (char &c: name) {
                            c = (char) std::tolower(c);
                        }
                        if (name.starts_with(search) || name.find(search) != std::string::npos) {
                            visibleScriptCount++;
                        }
                    }
                } else {
                    visibleScriptCount = static_cast<int>(scriptModules.size());
                }

                // Calculate content height based on visible script modules (3 per row)
                int i3 = 0;
                float i2 = 0;
                for (int idx = 0; idx < visibleScriptCount; idx++) {
                    if ((++i3 % 3) == 0) {
                        i2 += Constraints::SpacingConstraint(0.8, modWidth);
                    }
                }

                FlarialGUI::ScrollBar(120, scrollcenter.y, 10, modHeight + i2, 2);

                FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                float xModifier = 0.0f;
                float yModifier = 0.0f;

                int i = 0;

                for (const auto &pModule: scriptModules) {
                    bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                   (modcenter.y + yModifier + FlarialGUI::scrollpos - 300) <
                                   center.y + Constraints::RelativeConstraint(baseHeightReal);

                    if (!searchBarString.empty()) {
                        std::string name = pModule->name;

                        for (char &c: name) {
                            c = (char) std::tolower(c);
                        }

                        std::string search = searchBarString;

                        for (char &c: search) {
                            c = (char) std::tolower(c);
                        }

                        if (name.starts_with(search) ||
                            name.find(search) != std::string::npos) {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset,
                                                      modcenter.y + yModifier, pModule.get(),
                                                      pModule->icon, i, visible, ClickGUI::scriptingOpacity);
                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }
                        }
                    } else {
                        ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset, modcenter.y + yModifier,
                                                  pModule.get(),
                                                  pModule->icon, i, visible, ClickGUI::scriptingOpacity);

                        xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                        if ((++i % 3) == 0) {
                            yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                            xModifier = 0.0f;
                        }
                    }
                }

                FlarialGUI::UnsetScrollView();

                D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                shadowCol.a = scriptingOpacity;
                //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f,
                                                       center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                FlarialGUI::ShadowRect(
                    Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight},
                    Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                FlarialGUI::PopImClipRect();

                //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                FlarialGUI::PushImClipRect(D2D1::RectF(
                    center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f,
                    center.x + baseWidth,
                    center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) +
                    Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                FlarialGUI::ShadowRect(
                    Vec2{
                        center.x + Constraints::SpacingConstraint(0.15f, baseWidth),
                        center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)
                    }, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50,
                    100);
                FlarialGUI::PopImClipRect();

                //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
            }
            /* SETTINGS RENDER END */
            /* Mod Card End */
        }

        std::shared_ptr<Module> settingMod = ModuleManager::getModule(page.module);
        if (!settingMod) {
            settingMod = ScriptManager::getModuleByName(ScriptManager::getLoadedModules(), page.module);
        }


        if (settingMod && page.type != "normal" && settingsOpacity > 0.05f) {
            FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

            float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
            float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
            float rectY = Constraints::PercentageConstraint(0.167, "top");
            float rectWidth = Constraints::RelativeConstraint(0.965, "width");
            float rectHeight = Constraints::RelativeConstraint(0.85);
            round = Constraints::RoundingConstraint(38.f, 38.5f);

            float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
            float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

            D2D1_COLOR_F colorThing = ClickGUI::getColor("secondary2");

            D2D1_RECT_F rect = {rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
            FlarialGUI::PushImClipRect(rect);

            FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                    anotherRectWidth, anotherRectHeight, round.x, round.x);

            FlarialGUI::PopImClipRect();

            D2D1_COLOR_F bruv = ClickGUI::getColor("secondary1");
            D2D1_COLOR_F textCol = ClickGUI::getColor("globalText");

            FlarialGUI::PopSize();

            FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                 rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                 rectHeight);

            // Module's settingsRender handles its own scroll view via initSettingsPage()
            // No need to set up an outer scroll view here - it would conflict
            settingMod->settingsRender(settingsOffset);

            FlarialGUI::PopSize();

            if (!FlarialGUI::activeColorPickerWindows) {
                float childHeight = Constraints::SpacingConstraint(0.3, rectHeight);
                float childWidth = Constraints::SpacingConstraint(0.45, rectWidth);
                std::pair<float, float> centered = centerChildRectangle(rectWidth, rectHeight, childWidth,
                                                                        childHeight);

                round = Constraints::RoundingConstraint(25, 25);

                float buttonWidth = Constraints::RelativeConstraint(0.19f, "width");
                float buttonHeight = Constraints::RelativeConstraint(0.1f, "height");
                float spacingX = Constraints::RelativeConstraint(0.03);

                std::pair<float, float> thingYes = centerChildRectangle(childWidth, childHeight, buttonWidth,
                                                                        buttonHeight);


                textCol.a = settingsOpacity;
                if (FlarialGUI::RoundedButton(0, spacingX + centered.first + rectX - settingsOffset,
                                              thingYes.second + rectHeight + rectY -
                                              Constraints::RelativeConstraint(0.06f), colorThing, textCol, L"RESET ALL",
                                              buttonWidth, buttonHeight, round.x, round.x)) {
                    auto currentModule = ModuleManager::getModule(page.module);
                    if (currentModule != nullptr) {
                        bool wasEnabled = currentModule->isEnabled();
                        currentModule->enabledState = false; // THIS MIGHT BE WITH DELAY !!!
                        currentModule->active = false;
                        currentModule->settings.reset();
                        currentModule->postLoad(true);
                        FlarialGUI::ResetUIState();
                        if (wasEnabled) {
                            currentModule->settings.setValue("enabled", true);
                            currentModule->enabledState = true;
                        }
                        Client::SaveSettings();
                        Client::LoadSettings();
                        FlarialGUI::Notify("Reset all settings of " + currentModule->name);
                    }
                }


                if (FlarialGUI::RoundedButton(
                    1, -spacingX + centered.first + rectX + childWidth - buttonWidth - settingsOffset,
                    thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol,
                    L"RESET POS", buttonWidth, buttonHeight, round.x, round.x)) {
                    auto currentModule = ModuleManager::getModule(page.module);
                    if (currentModule != nullptr) {
                        currentModule->settings.deleteSetting("percentageX");
                        currentModule->settings.deleteSetting("percentageY");
                        currentModule->defaultConfig();
                        FlarialGUI::ResetUIState();
                    }
                }
            }
        }
        FlarialGUI::PopSize(); // Pops base rect
    }

    if (realBlurAmount < 0.1f) setEnabled(false);

    FlarialGUI::inMenu = this->enabledState;

    FlarialGUI::displayToolTips();
    FlarialGUI::NotifyHeartbeat();
}
