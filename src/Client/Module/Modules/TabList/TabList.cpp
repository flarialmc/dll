#include "TabList.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"
#include "Utils/APIUtils.hpp"

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include <future>
#include <queue>
#include <condition_variable>
#include <winrt/base.h>

#include "Command/Commands/SkinStealCommand.hpp"
#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Nick/NickModule.hpp"

bool logDebug = false;

// Forward declarations for functions in separate modules
enum class PlayerHeadLoadState {
    NotLoaded,
    Loading,
    Loaded,
    Failed
};

struct PlayerHeadTexture {
    winrt::com_ptr<ID3D12Resource> texture;
    D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
    UINT descriptorId = UINT_MAX;
    std::chrono::steady_clock::time_point lastUsed;
    std::atomic<PlayerHeadLoadState> loadState{PlayerHeadLoadState::NotLoaded};
    bool valid = false;
};

struct PlayerHeadTextureDX11 {
    winrt::com_ptr<ID3D11ShaderResourceView> srv;
    std::atomic<PlayerHeadLoadState> loadState{PlayerHeadLoadState::NotLoaded};
    bool valid = false;
};

// External references to variables and functions in other modules
extern std::unordered_map<std::string, PlayerHeadTextureDX11> g_playerHeadTexturesDX11;
extern std::unordered_map<std::string, PlayerHeadTexture> g_playerHeadTextures;
extern std::mutex g_playerHeadMutex;

// Function declarations for TabListTextures.cpp
void ProcessReadyPlayerHeadTextures();
void CleanupUnusedPlayerHeadDescriptors();
void InitializeAsyncLoading();
void InitializeDX12Uploader();
void CleanupPlayerHeadTextures();
PlayerHeadTexture *CreateTextureFromBytesDX12(const std::string &playerName, const unsigned char *data, int width, int height);
ID3D11ShaderResourceView *CreateTextureFromBytesDX11(const std::string &playerName, const unsigned char *data, int width, int height);
void RenderPlayerHeadWithPointFiltering(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, D2D1_RECT_F rect, const std::string &debugName);
void RenderPlayerHeadWithPointFilteringDX11(ID3D11ShaderResourceView *srv, D2D1_RECT_F rect, const std::string &debugName);
int getPingImage(int ping);

// Function declarations for TabListSorting.cpp
int getRolePriority(const std::string &name);
std::vector<PlayerListEntry> sortVecmap(const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap, bool flarialFirst, bool sort);

void TabList::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced && (active || ClickGUI::editmenu)) {
        // Process ready textures from background threads (adaptive rate based on performance)
        ProcessReadyPlayerHeadTextures();

        // Periodically clean up unused descriptors (every ~5 seconds at 60fps)
        static int cleanupCounter = 0;
        if (++cleanupCounter > 300) {
            cleanupCounter = 0;
            CleanupUnusedPlayerHeadDescriptors();
            if (logDebug)
                Logger::debug("PlayerHead usage: {} DX12 textures, {} DX11 textures",
                              g_playerHeadTextures.size(), g_playerHeadTexturesDX11.size());
        }
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            float keycardSize = Constraints::RelativeConstraint(0.05f * getOps<float>("uiscale"), "height", true);

            Vec2<float> settingperc{0.0, 0.0};

            std::string textAlignment = getOps<std::string>("textalignment");
            bool showHeads = getOps<bool>("showHeads");
            bool alphaOrder = getOps<bool>("alphaOrder");
            bool flarialFirst = getOps<bool>("flarialFirst");

            // if (SwapchainHook::isDX12) showHeads = false;
            auto module = ModuleManager::getModule("Nick");

            int maxColumn = floor(getOps<int>("maxColumn"));

            float fontSize = Constraints::SpacingConstraint(3, keycardSize);

            float totalHeight = 0;

            // do some caching bullshit
            // getPlayerMap() is expensive to run so we only check for updates every second

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPlayerMapUpdate).count() >= 1) {
                pmap_cache = SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap();
                lastPlayerMapUpdate = now;
            }

            if (vecmap.size() != pmap_cache.size() || refreshCache) {
                totalHeight = 0;
                columnx.clear();
                refreshCache = true;
                vecmap = sortVecmap(pmap_cache, flarialFirst, alphaOrder);

                totalWidth = keycardSize * (showHeads ? 1 : 0.4);

                float curMax = 0;

                validPlayers = 0;

                for (size_t i = 0; i < vecmap.size(); i++) {
                    if (vecmap[i].name.empty()) continue;

                    std::string name = String::removeColorCodes(vecmap[i].name);
                    if (name.empty()) continue;

                    std::string clearedName = String::removeNonAlphanumeric(name);
                    if (clearedName.empty()) clearedName = name;

                    if (module && module->isEnabled() && !NickModule::backupOri.empty() && clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        name = module->getOps<std::string>("nick");
                        if (name.empty()) name = clearedName;
                    }

                    auto textMetric = FlarialGUI::getFlarialTextSize(String::StrToWStr(name).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);

                    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
                    if (it != APIUtils::onlineUsers.end()) textMetric.x += Constraints::SpacingConstraint(0.6, keycardSize);

                    if (textMetric.x > curMax) curMax = textMetric.x;
                    if (i < maxColumn) totalHeight += keycardSize * 0.7f;

                    validPlayers++;

                    if ((i + 1) % maxColumn == 0 || i == vecmap.size() - 1) {
                        totalWidth += curMax + keycardSize * (showHeads ? 1 : 0.4);
                        columnx.push_back(curMax + keycardSize * (showHeads ? 1 : 0.4f));
                        curMax = 0;
                    }
                }

                baseTotalHeight = totalHeight;
            } else {
                totalHeight = baseTotalHeight;
            }

            if (getOps<bool>("worldName")) {
                std::string worldName = SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName();
                if (worldName != cache_worldName || refreshCache) {
                    worldNameMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(worldName).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    cache_worldName = worldName;
                }
                if (totalWidth < worldNameMetrics.x + keycardSize) totalWidth = worldNameMetrics.x + keycardSize;

                totalHeight += keycardSize * 0.75f;
            }

            if (getOps<bool>("serverPing")) {
                std::string serverPing = "Ping:__" + std::format("{}ms", SDK::getServerPing());
                if (serverPing != cache_serverPing || refreshCache) {
                    curPingMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(serverPing).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    curPingMetrics.x += Constraints::SpacingConstraint(0.5, keycardSize);
                    cache_serverPing = serverPing;
                }
                if (totalWidth < curPingMetrics.x + keycardSize) totalWidth = curPingMetrics.x + keycardSize;

                totalHeight += keycardSize * 0.5f;
            }

            totalHeight += keycardSize * 0.5f;

            if (getOps<bool>("playerCount")) {
                std::string playerCount = std::to_string(validPlayers) + " player(s) online";
                if (playerCount != cache_playerCount || refreshCache) {
                    playerCountMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(playerCount).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    cache_playerCount = playerCount;
                }

                std::string _curPlayer = String::removeNonAlphanumeric(String::removeColorCodes(module && module->isEnabled() && !NickModule::backupOri.empty() ? module->getOps<std::string>("nick") : SDK::clientInstance->getLocalPlayer()->getPlayerName()));
                std::string curPlayer = "Player:__" + _curPlayer;
                if (_curPlayer != cache_curPlayer || refreshCache) {
                    curPlayerMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(curPlayer).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    curPlayerMetrics.x += Constraints::SpacingConstraint(0.5, keycardSize);
                    cache_curPlayer = _curPlayer;
                }

                if (totalWidth < playerCountMetrics.x + keycardSize) totalWidth = playerCountMetrics.x + keycardSize;
                if (totalWidth < curPlayerMetrics.x + keycardSize) totalWidth = curPlayerMetrics.x + keycardSize;

                totalHeight += keycardSize * 2.f;
            }
            else totalHeight += keycardSize * 0.5f;

            Vec2<float> realcenter;
            if (settingperc.x != 0 || settingperc.y != 0) realcenter = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
            else realcenter = Constraints::CenterConstraint(totalWidth, totalHeight, "y", 0.0f, -0.85f);

            float fakex = realcenter.x;
            Vec2<float> vec2 = realcenter;

            Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));

            D2D1_COLOR_F disabledColor = getColor("bg");
            D2D1_COLOR_F textColor = getColor("text");
            D2D1_COLOR_F borderColor = getColor("border");

            if (getOps<bool>("glow")) FlarialGUI::ShadowRect(Vec2<float>(fakex, realcenter.y), Vec2<float>(totalWidth, totalHeight), getColor("glow"), rounde.x, (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));
            if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(fakex, realcenter.y, fakex + totalWidth, realcenter.y + totalHeight), rounde.x, rounde.x));
            if (getOps<bool>("border")) FlarialGUI::RoundedHollowRect(fakex, realcenter.y, Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true), borderColor, totalWidth, totalHeight, rounde.x, rounde.x);
            if (getOps<bool>("showBg")) {
                if (getOps<bool>("rectShadow")) FlarialGUI::RoundedRect(fakex + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"), realcenter.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"), getColor("rectShadow"), totalWidth, totalHeight, rounde.x, rounde.x);
                FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, totalHeight, rounde.x, rounde.x);
            }

            if (getOps<bool>("worldName")) {
                float textX = MC::windowSize.x / 2.f;
                float textY = realcenter.y;

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName()).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX, textY, FlarialGUI::to_wide(SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName()).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }
            else realcenter.y -= keycardSize * 0.75f;

            if (getOps<bool>("serverPing")) {
                static float p1 = 0.175;
                static float p2 = 0.196;
                static float p3 = 0.7;
                static float p4 = 0.77;

                realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                float textX1 = (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.2f);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX1 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        realcenter.y + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        L"Ping:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX1, realcenter.y, L"Ping:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                ImVec2 part2Metrics = FlarialGUI::getFlarialTextSize(L"Ping:_", keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                FlarialGUI::image(getPingImage(SDK::getServerPing()), D2D1::RectF(
                                      (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.f) + part2Metrics.x + Constraints::SpacingConstraint(p1, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p2, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize),
                                      ((MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.f) + part2Metrics.x) + Constraints::SpacingConstraint(p3, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      (realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p4, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize))));

                float textX2 = (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.2f) + part2Metrics.x + Constraints::SpacingConstraint(0.5, keycardSize);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX2 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        realcenter.y + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(std::format(" {}ms", SDK::getServerPing())).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX2, realcenter.y, FlarialGUI::to_wide(std::format(" {}ms", SDK::getServerPing())).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }
            else realcenter.y -= keycardSize * 0.5f;

            realcenter.y += keycardSize * 1.25f;

            for (size_t i = 0; i < vecmap.size(); i++) {
                if (!vecmap[i].name.size() || vecmap[i].name.empty()) continue;

                std::string pname = String::removeColorCodes(vecmap[i].name);
                if (pname.empty()) continue;

                if (refreshCache) vectab[i].clearedName = String::removeNonAlphanumeric(pname);
                if (vectab[i].clearedName.empty()) vectab[i].clearedName = pname;

                if (refreshCache) {
                    if (module && module->isEnabled() && !NickModule::backupOri.empty() && vectab[i].clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        if (std::string nick = module->getOps<std::string>("nick"); !nick.empty()) vectab[i].clearedName = nick;
                    }
                }

                float xx = 0;

                if (showHeads) {
                    // PLAYER HEAD START
                    const auto &skinImage = vecmap[i].playerSkin.mSkinImage;

                    if (logDebug)
                        Logger::debug("Player {} skin: format={}, size={}x{}, data_size={}",
                                      vecmap[i].name,
                                      static_cast<int>(skinImage.imageFormat),
                                      skinImage.mWidth, skinImage.mHeight,
                                      skinImage.mImageBytes.size());

                    if (static_cast<int>(skinImage.imageFormat) == 4 && // RGBA8Unorm format
                        skinImage.mWidth == skinImage.mHeight && // Square texture
                        skinImage.mWidth == 64 || skinImage.mWidth == 128 && // 64x64 or 128x128
                        (skinImage.mWidth & (skinImage.mWidth - 1)) == 0) {
                        // Power of 2

                        // Determine skin format based on width (0 = classic 64x64, 1 = slim 64x64+)
                        int skinFormat = (skinImage.mWidth == 64) ? 0 : 1;

                        std::string uniqueTextureKey = vecmap[i].playerSkin.mId;

                        bool alrExists = false;

                        if (SwapchainHook::isDX12 && g_playerHeadTextures.contains(uniqueTextureKey)) alrExists = true;
                        else if (g_playerHeadTexturesDX11.contains(uniqueTextureKey)) alrExists = true;

                        std::vector<unsigned char> head = SkinStealCommand::cropHead(skinImage, skinFormat);
                        std::vector<unsigned char> head2 = SkinStealCommand::cropHead(skinImage, skinFormat, true);

                        if (head.empty() && head2.empty()) {
                            if (logDebug) Logger::debug("Empty head data for player {}", vecmap[i].name);
                            continue;
                        }
                        // Dynamic head size detection and scaling for crisp pixels
                        // Support any skin resolution (64x64, 128x128, 256x256, etc.)
                        const int headSize = skinImage.mWidth / 8; // Head is always 1/8th of skin width
                        const int scaledSize = std::max(64, headSize * 2); // At least 64x64, or 2x the original head size
                        const int scale = scaledSize / headSize;
                        std::vector<unsigned char> scaledHead(scaledSize * scaledSize * 4);
                        std::vector<unsigned char> scaledHead2(scaledSize * scaledSize * 4);

                        if (!alrExists) {
                            for (int y = 0; y < scaledSize; y++) {
                                for (int x = 0; x < scaledSize; x++) {
                                    int srcX = x / scale;
                                    int srcY = y / scale;
                                    int srcIndex = (srcY * headSize + srcX) * 4;
                                    int dstIndex = (y * scaledSize + x) * 4;

                                    scaledHead[dstIndex + 0] = head[srcIndex + 0]; // R
                                    scaledHead[dstIndex + 1] = head[srcIndex + 1]; // G
                                    scaledHead[dstIndex + 2] = head[srcIndex + 2]; // B
                                    scaledHead[dstIndex + 3] = head[srcIndex + 3]; // A

                                    scaledHead2[dstIndex + 0] = head2[srcIndex + 0]; // R
                                    scaledHead2[dstIndex + 1] = head2[srcIndex + 1]; // G
                                    scaledHead2[dstIndex + 2] = head2[srcIndex + 2]; // B
                                    scaledHead2[dstIndex + 3] = head2[srcIndex + 3]; // A
                                }
                            }
                        }

                        std::string playerName = String::removeNonAlphanumeric(String::removeColorCodes(vecmap[i].name));
                        if (logDebug) {
                            // Debug: Check if the scaled head has any non-transparent pixels
                            bool hasVisiblePixels = false;
                            for (size_t j = 3; j < scaledHead.size(); j += 4) {
                                if (scaledHead[j] > 0) {
                                    // Alpha channel
                                    hasVisiblePixels = true;
                                    break;
                                }
                                if (scaledHead2[j] > 0) {
                                    // Alpha channel
                                    hasVisiblePixels = true;
                                    break;
                                }
                            }

                            Logger::debug("Player {} scaled head: size={}, visible_pixels={}", playerName, scaledHead.size(), hasVisiblePixels);
                        }

                        if (refreshCache) {
                            headDisplaySize = Constraints::SpacingConstraint(0.6, keycardSize);
                            headDisplaySize2 = Constraints::SpacingConstraint(0.68, keycardSize);
                            diff = headDisplaySize2 - headDisplaySize;

                            // Position for the head (left of the player name)
                            vectab[i].headPos = ImVec2(fakex + Constraints::SpacingConstraint(0.45, keycardSize), realcenter.y + Constraints::SpacingConstraint(0.3, keycardSize));
                            vectab[i].headPos2 = ImVec2(
                                fakex + Constraints::SpacingConstraint(0.45, keycardSize) - diff / 2.f,
                                realcenter.y + Constraints::SpacingConstraint(0.3, keycardSize) - diff / 2.f);
                            vectab[i].headSize2D = ImVec2(headDisplaySize, headDisplaySize);
                            vectab[i].headSize22D = ImVec2(headDisplaySize2, headDisplaySize2);
                        }

                        if (SwapchainHook::isDX12) {
                            // DX12 path
                            PlayerHeadTexture *playerTex = CreateTextureFromBytesDX12(uniqueTextureKey, scaledHead.data(), scaledSize, scaledSize);
                            PlayerHeadTexture *playerTex2 = CreateTextureFromBytesDX12("_" + uniqueTextureKey, scaledHead2.data(), scaledSize, scaledSize);

                            if (playerTex && playerTex->valid && playerTex->loadState == PlayerHeadLoadState::Loaded) {
                                // Update last used time when rendering
                                {
                                    std::lock_guard<std::mutex> lock(g_playerHeadMutex);
                                    playerTex->lastUsed = std::chrono::steady_clock::now();
                                }

                                ImDrawList *drawList = ImGui::GetForegroundDrawList();
                                if (drawList) {
                                    if (logDebug) Logger::debug("DX12 GPU handle for {}: 0x{:x}", playerName, playerTex->srvGpuHandle.ptr);

                                    // Render with point filtering for crisp pixels
                                    RenderPlayerHeadWithPointFiltering(playerTex->srvGpuHandle, D2D1::RectF(vectab[i].headPos.x, vectab[i].headPos.y, vectab[i].headPos.x + vectab[i].headSize2D.x, vectab[i].headPos.y + vectab[i].headSize2D.y), playerName);
                                    RenderPlayerHeadWithPointFiltering(playerTex2->srvGpuHandle, D2D1::RectF(vectab[i].headPos2.x, vectab[i].headPos2.y, vectab[i].headPos2.x + vectab[i].headSize22D.x, vectab[i].headPos2.y + vectab[i].headSize22D.y), "_" + playerName);

                                    if (logDebug) Logger::debug("Rendered DX12 head for player {} at ({}, {})", playerName, vectab[i].headPos.x, vectab[i].headPos.y);
                                }
                            } else {
                                if (logDebug) Logger::debug("Failed to create/get DX12 texture for player {}", playerName);
                            }
                        } else if (SwapchainHook::d3d11Device != nullptr) {
                            // DX11 path
                            ID3D11ShaderResourceView *srv = CreateTextureFromBytesDX11(uniqueTextureKey, scaledHead.data(), scaledSize, scaledSize);
                            ID3D11ShaderResourceView *srv2 = CreateTextureFromBytesDX11("_" + uniqueTextureKey, scaledHead2.data(), scaledSize, scaledSize);

                            if (srv) {
                                ImDrawList *drawList = ImGui::GetForegroundDrawList();
                                if (drawList) {
                                    if (logDebug) Logger::debug("DX11 SRV for {}: 0x{:x}", playerName, reinterpret_cast<uintptr_t>(srv));

                                    // Render with point filtering for crisp pixels
                                    RenderPlayerHeadWithPointFilteringDX11(srv, D2D1::RectF(vectab[i].headPos.x, vectab[i].headPos.y, vectab[i].headPos.x + vectab[i].headSize2D.x, vectab[i].headPos.y + vectab[i].headSize2D.y), playerName);
                                    RenderPlayerHeadWithPointFilteringDX11(srv2, D2D1::RectF(vectab[i].headPos2.x, vectab[i].headPos2.y, vectab[i].headPos2.x + vectab[i].headSize22D.x, vectab[i].headPos2.y + vectab[i].headSize22D.y), "_" + playerName);

                                    if (logDebug) Logger::debug("Rendered DX11 head for player {} at ({}, {})", playerName, vectab[i].headPos.x, vectab[i].headPos.y);
                                }
                                // Don't release - it's managed by the cache now
                            } else {
                                if (logDebug) Logger::debug("Failed to create/get DX11 texture for player {}", playerName);
                            }
                        } else {
                            if (logDebug) Logger::debug("No DirectX device available for player {}", playerName);
                        }
                    }
                    // PLAYER HEAD END
                }

                auto pit = std::ranges::find(APIUtils::onlineUsers, vectab[i].clearedName);
                if (refreshCache) {
                    vectab[i].pNameMetrics = FlarialGUI::getFlarialTextSize(String::StrToWStr(vectab[i].clearedName).c_str(), columnx[i / maxColumn] - (0.825 * keycardSize), keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    vectab[i].textWidth = columnx[i / maxColumn] - (0.825 * keycardSize);
                }

                if (pit != APIUtils::onlineUsers.end()) {
                    // FLARIAL TAG START
                    static float p1 = 0.175;
                    static float p2 = 0.196;
                    static float p3 = 0.7;
                    static float p4 = 0.77;

                    int imageResource = roleLogos["Default"];
                    for (const auto &[role, resource]: roleLogos) {
                        if (APIUtils::hasRole(role, vectab[i].clearedName)) {
                            imageResource = resource;
                            break;
                        }
                    }

                    if (refreshCache) {
                        float lol = columnx[i / maxColumn] - vectab[i].pNameMetrics.x;
                        float trollOffset = keycardSize * (showHeads ? 1 : 0.3f);

                        if (!showHeads && textAlignment == "Center") trollOffset -= keycardSize * 1.75f;

                        vectab[i].imageRect = D2D1::RectF(
                            fakex + Constraints::SpacingConstraint(p1, keycardSize) + trollOffset + (textAlignment == "Center" ? (lol / 2.f) - trollOffset * 0.75f : 0.f),
                            realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                            fakex + Constraints::SpacingConstraint(p3, keycardSize) + trollOffset + (textAlignment == "Center" ? (lol / 2.f) - trollOffset * 0.75f : 0.f),
                            realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize));
                    }

                    FlarialGUI::image(imageResource, vectab[i].imageRect);

                    xx += Constraints::SpacingConstraint(0.6, keycardSize);

                    // FLARIAL TAG END

                    if (refreshCache) {
                        vectab[i].textX = fakex + (textAlignment == "Center" ? xx / 2.f : textAlignment == "Right" ? 0 : xx) + keycardSize * (showHeads ? 1.2f : 0.5f);
                        vectab[i].textY = realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize);
                    }
                    if (getOps<bool>("textShadow")) {
                        if (refreshCache) {
                            vectab[i].textShadowX = vectab[i].textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                            vectab[i].textShadowY = vectab[i].textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                        }

                        FlarialGUI::FlarialTextWithFont(
                            vectab[i].textShadowX,
                            vectab[i].textShadowY,
                            String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);
                    }

                    FlarialGUI::FlarialTextWithFont(vectab[i].textX, vectab[i].textY, String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                } else {
                    if (getOps<bool>("textShadow")) {
                        if (refreshCache) {
                            vectab[i].textShadowY2 = (realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize)) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                            vectab[i].textShadowX2 = (fakex + xx + keycardSize * (showHeads ? 1.2f : 0.5f)) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                        }
                        FlarialGUI::FlarialTextWithFont(
                            vectab[i].textShadowX2,
                            vectab[i].textShadowY2,
                            String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);
                    }

                    if (refreshCache) {
                        vectab[i].nfTextX = fakex + xx + keycardSize * (showHeads ? 1.2f : 0.5f);
                        vectab[i].nfTextY = realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize);
                    }

                    FlarialGUI::FlarialTextWithFont(vectab[i].nfTextX, vectab[i].nfTextY, String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                }
                realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                if ((i + 1) % maxColumn == 0) {
                    realcenter.y -= Constraints::SpacingConstraint(0.70, keycardSize) * maxColumn;
                    fakex += columnx[i / maxColumn];
                }
            }

            if (getOps<bool>("playerCount")) {
                float curY = vec2.y + totalHeight;

                if (getOps<bool>("serverPing") == getOps<bool>("worldName")) curY -= keycardSize * 1.75f;
                else if (!getOps<bool>("serverPing")) curY -= keycardSize * 1.75f;
                else if (!getOps<bool>("worldName")) curY -= keycardSize * 1.5f;

                static float p1 = 0.175;
                static float p2 = 0.196;
                static float p3 = 0.7;
                static float p4 = 0.77;

                int imageResource = roleLogos["Default"];
                for (const auto &[role, resource]: roleLogos) {
                    if (APIUtils::hasRole(role, cache_curPlayer)) {
                        imageResource = resource;
                        break;
                    }
                }

                if (refreshCache) textX1 = (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX1 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        L"Player:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX1, curY, L"Player:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                if (refreshCache) part1Metrics = FlarialGUI::getFlarialTextSize(L"Player:_", keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);

                FlarialGUI::image(imageResource, D2D1::RectF(
                                      (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x + Constraints::SpacingConstraint(p1, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p2, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize),
                                      ((MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x) + Constraints::SpacingConstraint(p3, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      (curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p4, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize))));

                if (refreshCache) textX2 = (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f) + part1Metrics.x + Constraints::SpacingConstraint(0.5, keycardSize);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX2 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(cache_curPlayer).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX2, curY, FlarialGUI::to_wide(cache_curPlayer).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                curY += Constraints::SpacingConstraint(0.70, keycardSize);

                if (refreshCache) textX3 = MC::windowSize.x / 2.f;

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX3 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(cache_playerCount).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX3, curY, FlarialGUI::to_wide(cache_playerCount).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }

            refreshCache = false;
        }
    }
}