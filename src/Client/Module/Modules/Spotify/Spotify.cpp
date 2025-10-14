// TODO: 
// - Text wraparound for long names (Title, artist, album)
// - ^ or just adjust the entire module length
// - RTL alignment including the album art
// - Figure out Asian characters (?) lost cause tbh
// - Detect Spotify.json and prompt in the module instead of Logger
// - Fix album art rendering below the background

#include "Spotify.hpp"
#include "Utils/Utils.hpp"
#include "GUI/D2D.hpp"
#include "../ClickGUI/ClickGUI.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>

extern size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

void Spotify::onEnable() {
    Logger::info("Spotify module enabling...");
    
    Listen(this, RenderEvent, &Spotify::onRender)
    Listen(this, RenderUnderUIEvent, &Spotify::onRenderUnderUI)
    Listen(this, UIControlGetPositionEvent, &Spotify::onUIControlGetPosition)
    Listen(this, SetupAndRenderEvent, &Spotify::onSetupAndRender)
    
    Logger::info("Spotify module event listeners registered");
    
    loadCredentials();
    
    if (!credentialsLoaded) {
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Spotify credentials not configured. Please set up Spotify.json first.");
        Logger::warn("Spotify credentials not loaded, module will not function");
        return;
    }
    
    Logger::info("Spotify credentials loaded successfully");
    
    refreshAccessToken();
    
    shouldStop = false;
    updateThread = std::thread([this]() {
        Logger::info("Spotify update thread started");
        while (!shouldStop) {
            if (this->isEnabled()) {
                Logger::debug("Spotify updating track info...");
                updateTrackInfo();
            }
            float updateInterval = getOps<float>("updateInterval");
            int updateIntervalMs = static_cast<int>(updateInterval * 1000); // Convert seconds to milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(updateIntervalMs));
        }
        Logger::info("Spotify update thread ended");
    });
    
    Module::onEnable();
    Logger::info("Spotify module enabled successfully");
}

void Spotify::onDisable() {
    Logger::info("Spotify module disabling...");
    
    Deafen(this, RenderEvent, &Spotify::onRender)
    Deafen(this, RenderUnderUIEvent, &Spotify::onRenderUnderUI)
    Deafen(this, UIControlGetPositionEvent, &Spotify::onUIControlGetPosition)
    Deafen(this, SetupAndRenderEvent, &Spotify::onSetupAndRender)
    
    Logger::info("Spotify module event listeners removed");
    
    shouldStop = true;
    if (updateThread.joinable()) {
        Logger::info("Stopping Spotify update thread...");
        updateThread.join();
        Logger::info("Spotify update thread stopped");
    }
    
    Module::onDisable();
    Logger::info("Spotify module disabled successfully");
}

void Spotify::terminate() {
    cleanupUpdateThread();
    Module::terminate();
}

void Spotify::defaultConfig() {
    setDef("showAlbumArt", true);
    setDef("showProgress", true);
    setDef("showTime", true);
    
    setDef("progressColor", (std::string)"1db954", 1.0f, false);
    setDef("progressBackgroundColor", (std::string)"333333", 1.0f, false);
    setDef("updateInterval", 5.0f);
    
    setDef("uiscale", 0.65f);
    setDef("percentageX", 0.0f);
    setDef("percentageY", 0.0f);
    setDef("rounding", 32.0f);
    setDef("showBg", true);
    setDef("rectShadow", false);
    setDef("rectShadowOffset", 0.003f);
    setDef("border", false);
    setDef("borderWidth", 1.0f);
    setDef("glow", false);
    setDef("glowAmount", 30.0f);
    setDef("BlurEffect", false);
    
    setDef("text", (std::string)"fafafa", 1.f, false);
    setDef("bg", (std::string)"000000", 0.55f, false);
    setDef("rectShadow", (std::string)"00000", 0.55f, false);
    setDef("border", (std::string)"000000", 1.f, false);
    setDef("glow", (std::string)"F0F0F0", 1.f, false);
    
    Module::defaultConfig();
}

void Spotify::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Spotify Settings");
    addToggle("Show Album Art", "Display the album artwork", "showAlbumArt");
    addToggle("Show Progress Bar", "Display the progress bar", "showProgress");
    addToggle("Show Time", "Display elapsed and remaining time", "showTime");
    addSlider("Update Interval", "How often to update track info (seconds)", "updateInterval", 10.0f, 0.5f);
    
    addHeader("Spotify Colors");
    addColorPicker("Progress Color", "Color of the progress bar", "progressColor");
    addColorPicker("Progress Background", "Background color of the progress bar", "progressBackgroundColor");

    addHeader("Main");
    addSlider("UI Scale", "", "uiscale", 2.0f);
    addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", "BlurEffect");
    addSlider("Rounding", "Rounding of the rectangle", "rounding");
    addToggle("Background", "", "showBg");
    addConditionalToggle(getOps<bool>("showBg"), "Background Shadow", "Displays a shadow under the background", "rectShadow");
    addConditionalSlider(getOps<bool>("showBg") && getOps<bool>("rectShadow"), "Shadow Offset", "How far the shadow will be.", "rectShadowOffset", 0.02f, 0.001f);
    addToggle("Border", "", "border");
    addConditionalSlider(getOps<bool>("border"), "Border Thickness", "", "borderWidth", 4.f);
    addToggle("Glow", "", "glow");
    addConditionalSlider(getOps<bool>("glow"), "Glow Amount", "", "glowAmount", 100.f);

    addHeader("Colors");
    addColorPicker("Text Color", "", "text");
    addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
    addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
    addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
    addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Spotify::loadCredentials() {
    Logger::info("Loading Spotify credentials...");
    std::string configPath = Utils::getClientPath() + "\\Spotify.json";
    Logger::debug("Looking for Spotify config at: {}", configPath);
    
    std::ifstream file(configPath);
    if (!file) {
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Spotify.json not found.");
        Logger::warn("Spotify.json not found at: {}", configPath);
        credentialsLoaded = false;
        return;
    }

    try {
        nlohmann::json j;
        file >> j;
        client_id = j["id"];
        client_secret = j["secret"];
        refresh_token = j["refresh_token"];
        credentialsLoaded = true;
        Logger::info("Spotify credentials loaded successfully");
        Logger::debug("Client ID: {}, Refresh token: {}", client_id, refresh_token.substr(0, 10) + "...");
    } catch (const std::exception& e) {
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Failed to parse credentials: {}", e.what());
        Logger::error("Error parsing Spotify.json: {}", e.what());
        credentialsLoaded = false;
    }
}

void Spotify::refreshAccessToken() {
    if (!credentialsLoaded) {
        Logger::debug("Cannot refresh access token - credentials not loaded");
        return;
    }
    
    Logger::info("Refreshing Spotify access token...");
    Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Refreshing access token...");

    std::string post_data =
        "grant_type=refresh_token&refresh_token=" + refresh_token +
        "&client_id=" + client_id +
        "&client_secret=" + client_secret;

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    Logger::debug("Making token refresh request...");
    auto [responseCode, responseBody] = APIUtils::Request("https://accounts.spotify.com/api/token", "POST", post_data, headers);
    Logger::debug("Token refresh response code: {}, body length: {}", responseCode, responseBody.length());

    if (responseCode == 200) {
        try {
            nlohmann::json jsonData = nlohmann::json::parse(responseBody);
            if (jsonData.contains("access_token")) {
                access_token = jsonData["access_token"].get<std::string>();
                Logger::info("Access token refreshed successfully");
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Access token refreshed successfully!");
            }
            else {
                Logger::error("No access_token in response: {}", responseBody);
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Failed to refresh token: {}", jsonData.dump(4));
            }
        } catch (const std::exception& e) {
            Logger::error("Failed to parse token response: {}", e.what());
        }
    }
    else {
        Logger::error("Token refresh failed with response code: {}, body: {}", responseCode, responseBody);
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Token refresh failed with response code: {}", responseCode);
    }
}

void Spotify::updateTrackInfo() {
    if (isUpdating.load() || !credentialsLoaded) {
        Logger::debug("Skipping track update - updating: {}, credentials: {}", isUpdating.load(), credentialsLoaded);
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    float updateInterval = getOps<float>("updateInterval");
    int updateIntervalMs = static_cast<int>(updateInterval * 1000); // Convert seconds to milliseconds
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastApiCall).count() < updateIntervalMs) {
        Logger::debug("Skipping track update - too soon since last call");
        return;
    }
    
    Logger::debug("Updating track info...");
    isUpdating.store(true);
    lastApiCall = now;
    
    try {
        std::string api_url = "https://api.spotify.com/v1/me/player/currently-playing";
        
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        Logger::debug("Making API request to: {}", api_url);
        auto [responseCode, responseBody] = APIUtils::Request(api_url, "GET", "", headers);
        Logger::debug("API response code: {}, body length: {}", responseCode, responseBody.length());
        
        if (responseCode == 200) {
            Logger::debug("Successfully received track data from API");
            nlohmann::json jsonData = nlohmann::json::parse(responseBody);
            
            if (jsonData.contains("item") && !jsonData["item"].is_null()) {
                auto item = jsonData["item"];
                
                currentTrack.name = item.contains("name") ? item["name"].get<std::string>() : "Unknown";
                currentTrack.album = item.contains("album") && item["album"].contains("name") ? 
                                   item["album"]["name"].get<std::string>() : "Unknown Album";
                
                if (item.contains("artists") && item["artists"].is_array() && !item["artists"].empty()) {
                    currentTrack.artist = item["artists"][0]["name"].get<std::string>();
                } else {
                    currentTrack.artist = "Unknown Artist";
                }
                
                if (item.contains("album") && item["album"].contains("images") && 
                    item["album"]["images"].is_array() && !item["album"]["images"].empty()) {
                    currentTrack.albumArtUrl = item["album"]["images"][0]["url"].get<std::string>();
                }
                
                currentTrack.duration_ms = item.contains("duration_ms") ? item["duration_ms"].get<int>() : 0;
                currentTrack.progress_ms = jsonData.contains("progress_ms") ? jsonData["progress_ms"].get<int>() : 0;
                currentTrack.is_playing = jsonData.contains("is_playing") ? jsonData["is_playing"].get<bool>() : false;
                currentTrack.last_update = now;
                currentTrack.valid = true;
                
                Logger::info("Track updated: {} by {} ({}ms/{}ms, playing: {})", 
                        currentTrack.name, currentTrack.artist, currentTrack.progress_ms, 
                        currentTrack.duration_ms, currentTrack.is_playing);
                
                // Download album art if URL changed
                if (!currentTrack.albumArtUrl.empty() && currentTrack.albumArtUrl != cachedAlbumArtPath) {
                    Logger::debug("Downloading album art from: {}", currentTrack.albumArtUrl);
                    downloadAlbumArt(currentTrack.albumArtUrl);
                }
            } else {
                Logger::debug("No track item in API response");
                currentTrack.valid = false;
            }
        } else if (responseCode == 204) {
            Logger::debug("No content playing (204 response)");
            currentTrack.valid = false;
        } else {
            Logger::error("API call failed with code: {}, body: {}", responseCode, responseBody);
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "API call failed with code: {}", responseCode);
            currentTrack.valid = false;
        }
    } catch (const std::exception& e) {
        Logger::error("Error updating track info: {}", e.what());
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Error updating track info: {}", e.what());
        currentTrack.valid = false;
    }
    
    isUpdating.store(false);
    Logger::debug("Track update completed");
}

void Spotify::downloadAlbumArt(const std::string& url) {
    if (url.empty()) {
        Logger::debug("Skipping album art download - empty URL");
        return;
    }
    
    Logger::debug("Starting album art download from: {}", url);
    std::thread([this, url]() {
        try {
            CURL* curl = curl_easy_init();
            if (!curl) return;
            
            std::string imageData;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            
            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            
            if (res == CURLE_OK && !imageData.empty()) {
                // Image cache
                std::string cacheDir = Utils::getRoamingPath() + "\\Flarial\\SpotifyCache";
                std::filesystem::create_directories(cacheDir);
                
                std::string filename = std::to_string(std::hash<std::string>{}(url)) + ".jpg";
                std::string filepath = cacheDir + "\\" + filename;
                
                std::ofstream file(filepath, std::ios::binary);
                if (file.is_open()) {
                    file.write(imageData.c_str(), imageData.size());
                    file.close();
                    
                    currentTrack.albumArtPath = "Flarial\\SpotifyCache\\" + filename;
                    cachedAlbumArtPath = url;
                    albumArtDownloaded = true;
                    
                    Logger::info("Album art downloaded and cached: {}", currentTrack.albumArtPath);
                }
            }
        } catch (const std::exception& e) {
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Error downloading album art: {}", e.what());
        }
    }).detach();
}

std::string Spotify::formatTime(int milliseconds) {
    int totalSeconds = milliseconds / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    
    std::ostringstream oss;
    oss << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

void Spotify::cleanupUpdateThread() {
    shouldStop = true;
    if (updateThread.joinable()) {
        updateThread.join();
    }
}

void Spotify::onRender(RenderEvent& event) {
    if (!this->isEnabled() || !credentialsLoaded || SDK::getCurrentScreen() != "hud_screen") {
        return;
    }
    
    // Update local progress if music is playing
    if (currentTrack.valid && currentTrack.is_playing) {
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTrack.last_update).count();
        
        // Only update if enough time has passed (we're not updating this every frame)
        if (timeSinceLastUpdate >= 1000) { // Update every second
            currentTrack.progress_ms += timeSinceLastUpdate;
            currentTrack.last_update = now;
            
            // Don't exceed song duration
            if (currentTrack.progress_ms > currentTrack.duration_ms) {
                currentTrack.progress_ms = currentTrack.duration_ms;
            }
        }
    }
    
    // Music player dimensions
    float uiScale = getOps<float>("uiscale");
    float playerWidth = Constraints::RelativeConstraint(0.4f * uiScale);  // 40% of screen width * UI scale
    float playerHeight = Constraints::RelativeConstraint(0.12f * uiScale); // 12% of screen height * UI scale
    float albumArtSize = Constraints::RelativeConstraint(0.08f * uiScale); // 8% of screen height (square) * UI scale
    
    // Update current size
    currentSize = Vec2<float>(playerWidth, playerHeight);
    
    // Get module pos
    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
    Vec2<float> topleft;
    
    if (settingperc.x != 0) {
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x - playerWidth), settingperc.y * (MC::windowSize.y - playerHeight));
        currentPos = topleft;
    } else {
        // Center the music player
        topleft = Vec2<float>(
            (MC::windowSize.x - playerWidth) / 2.0f,
            (MC::windowSize.y - playerHeight) / 2.0f
        );
        currentPos = topleft;
    }
    
    // Handle dragging in ClickGUI edit mode
    // TODO: This ain't right, I'm pretty sure
    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(topleft.x, topleft.y, playerWidth, playerHeight, 30, this->name);
        checkForRightClickAndOpenSettings(topleft.x, topleft.y, playerWidth, playerHeight);
        
        if (currentPos.x != -120.0f) {
            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 30, playerWidth, playerHeight);
            
            currentPos.x = vec2.x;
            currentPos.y = vec2.y;
            topleft = currentPos;
            
            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, playerWidth, playerHeight);
            
            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);
        }
        
        FlarialGUI::UnsetWindowRect();
    }
    
    // Get colors from settings
    D2D1_COLOR_F textColor = getColor("text");  // Use base module text color
    D2D1_COLOR_F progressColor = getColor("progressColor");
    D2D1_COLOR_F progressBgColor = getColor("progressBackgroundColor");
    
    // Draw music player background using the base module system (proper layering)
    // Calculate proper rounding with UI scale
    Vec2<float> rounde = Constraints::RoundingConstraint(
        getOps<float>("rounding") * getOps<float>("uiscale"), 
        getOps<float>("rounding") * getOps<float>("uiscale")
    );
    
    // Apply all base module background effects in the correct order
    if (getOps<bool>("glow")) {
        FlarialGUI::ShadowRect(
            Vec2<float>(topleft.x, topleft.y),
            Vec2<float>(playerWidth, playerHeight),
            getColor("glow"), 
            rounde.x,
            (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top")
        );
    }
    
    // Blur effect
    float blur = Client::settings.getSettingByName<float>("blurintensity")->value;
    if (getOps<bool>("BlurEffect")) {
        FlarialGUI::BlurRect(D2D1::RoundedRect(
            D2D1::RectF(topleft.x, topleft.y, topleft.x + playerWidth, topleft.y + playerHeight), 
            rounde.x, 
            rounde.x
        ));
    }
    
    // Background shadow
    if (getOps<bool>("rectShadow")) {
        FlarialGUI::RoundedRect(
            topleft.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            topleft.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            getColor("rectShadow"),
            playerWidth,
            playerHeight,
            rounde.x,
            rounde.x
        );
    }
    
    // Main background
    if (getOps<bool>("showBg")) {
        FlarialGUI::RoundedRect(
            topleft.x,
            topleft.y,
            getColor("bg"), 
            playerWidth,
            playerHeight,
            rounde.x,
            rounde.x
        );
    }
    
    // Border
    if (getOps<bool>("border")) {
        FlarialGUI::RoundedRect(
            topleft.x,
            topleft.y,
            getColor("border"),
            playerWidth,
            playerHeight,
            rounde.x,
            rounde.x,
            getOps<float>("borderWidth")
        );
    }
    
    // Draw album art on the left (only if music is playing and album art is available)
    if (currentTrack.valid && getOps<bool>("showAlbumArt") && !currentTrack.albumArtPath.empty()) {
        std::string albumArtKey = "spotify_album_" + currentTrack.albumArtPath;
        
        // Load album art if not already loaded
        if (ImagesClass::eimages[albumArtKey] == nullptr) {
            std::string fullPath = Utils::getRoamingPath() + "\\" + currentTrack.albumArtPath;
            FlarialGUI::LoadImageFromFile(FlarialGUI::to_wide(fullPath).c_str(), &ImagesClass::eimages[albumArtKey]);
        }
        
        // Draw album art
        // TODO: Fix album art rendering below the background
        if (ImagesClass::eimages[albumArtKey] != nullptr) {
            float albumX = topleft.x + Constraints::RelativeConstraint(0.01f * uiScale); // Scaled margin from left
            float albumY = topleft.y + (playerHeight - albumArtSize) / 2.0f; // Center vertically
            D2D1_RECT_F imageRect = D2D1::RectF(albumX, albumY, albumX + albumArtSize, albumY + albumArtSize);
            D2D::context->DrawBitmap(ImagesClass::eimages[albumArtKey], imageRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR);
        }
    }
    
    // Draw track information on the right (following base module text pattern)
    float albumArtMargin = Constraints::RelativeConstraint(0.01f * uiScale); // Same margin as album art
    float textX = topleft.x + albumArtSize + albumArtMargin + Constraints::RelativeConstraint(0.01f * uiScale); // Start after album art + margins
    float textY = topleft.y + Constraints::RelativeConstraint(0.015f * uiScale); // Top margin
    float textWidth = playerWidth - albumArtSize - (albumArtMargin * 2) - Constraints::RelativeConstraint(0.01f * uiScale); // Available width for text
    float lineHeight = Constraints::RelativeConstraint(0.025f * uiScale); // Height between lines (following base module pattern)
    
    if (currentTrack.valid) {
        // Music is playing - show track info
        // Song title (largest) - following base module text sizing pattern
        float titleTextSize = Constraints::SpacingConstraint(5.0f, lineHeight);
        FlarialGUI::FlarialTextWithFont(
            textX, textY, FlarialGUI::to_wide(currentTrack.name).c_str(),
            textWidth, lineHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            titleTextSize,
            DWRITE_FONT_WEIGHT_BOLD,
            textColor
        );
        
        // Artist - following base module text sizing pattern
        if (!currentTrack.artist.empty()) {
            float artistTextSize = Constraints::SpacingConstraint(4.5f, lineHeight);
            FlarialGUI::FlarialTextWithFont(
                textX, textY + lineHeight, FlarialGUI::to_wide(currentTrack.artist).c_str(),
                textWidth, lineHeight,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                artistTextSize,
                DWRITE_FONT_WEIGHT_NORMAL,
                textColor
            );
        }
        
        // Album - following base module text sizing pattern
        if (!currentTrack.album.empty()) {
            float albumTextSize = Constraints::SpacingConstraint(4.0f, lineHeight);
            FlarialGUI::FlarialTextWithFont(
                textX, textY + lineHeight * 2, FlarialGUI::to_wide(currentTrack.album).c_str(),
                textWidth, lineHeight,
                DWRITE_TEXT_ALIGNMENT_LEADING,
                albumTextSize,
                DWRITE_FONT_WEIGHT_NORMAL,
                textColor
            );
        }
        
        // Progress bar and time
        if (getOps<bool>("showProgress") && currentTrack.duration_ms > 0) {
            float progressY = topleft.y + playerHeight - Constraints::RelativeConstraint(0.025f * uiScale); // Bottom margin
            float progressWidth = textWidth;
            float progressHeight = Constraints::RelativeConstraint(0.005f * uiScale); // Progress bar height
            
            // Progress bar background using base module system
            FlarialGUI::RoundedRect(
                textX, progressY,
                progressBgColor,
                progressWidth, progressHeight,
                2.0f, 2.0f
            );
            
            // Progress bar fill
            float progress = (float)currentTrack.progress_ms / (float)currentTrack.duration_ms;
            progress = std::max(0.0f, std::min(1.0f, progress));
            float fillWidth = progressWidth * progress;
            
            if (fillWidth > 0) {
                FlarialGUI::RoundedRect(
                    textX, progressY,
                    progressColor,
                    fillWidth, progressHeight,
                    2.0f, 2.0f
                );
            }
            
            // Time labels
            if (getOps<bool>("showTime")) {
                float timeHeight = Constraints::RelativeConstraint(0.018f * uiScale); // Time text height
                float timeY = progressY + Constraints::RelativeConstraint(0.008f * uiScale); // Margin above progress bar
                float timeTextSize = Constraints::SpacingConstraint(3.0f, timeHeight);
                
                // Elapsed time (left)
                FlarialGUI::FlarialTextWithFont(
                    textX, timeY, FlarialGUI::to_wide(formatTime(currentTrack.progress_ms)).c_str(),
                    textWidth, timeHeight,
                    DWRITE_TEXT_ALIGNMENT_LEADING,
                    timeTextSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor
                );
                
                // Total time (right)
                FlarialGUI::FlarialTextWithFont(
                    textX, timeY, FlarialGUI::to_wide(formatTime(currentTrack.duration_ms)).c_str(),
                    textWidth, timeHeight,
                    DWRITE_TEXT_ALIGNMENT_TRAILING,
                    timeTextSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    textColor
                );
            }
        }
    } else {
        // No music playing - show fallback message in song title area
        float noMusicTextSize = Constraints::SpacingConstraint(5.0f, lineHeight);
        FlarialGUI::FlarialTextWithFont(
            textX, textY, FlarialGUI::to_wide("No music playing").c_str(),
            textWidth, lineHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            noMusicTextSize,
            DWRITE_FONT_WEIGHT_BOLD,
            textColor
        );
    }
}

void Spotify::onRenderUnderUI(RenderUnderUIEvent& event) {
}

void Spotify::onUIControlGetPosition(UIControlGetPositionEvent& event) {
}

void Spotify::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!this->isEnabled() && !delayDisable) return;
    update();
}

void Spotify::update() {
    if (!this->isEnabled() && !delayDisable) return;
    
    float uiScale = getOps<float>("uiscale");
    float playerWidth = Constraints::RelativeConstraint(0.4f * uiScale, "width");
    float playerHeight = Constraints::RelativeConstraint(0.12f * uiScale, "height");
    
    currentSize = Vec2<float>(playerWidth, playerHeight);
    
    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
    
    if (settingperc.x != 0) {
        currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - playerWidth), settingperc.y * (MC::windowSize.y - playerHeight));
    }
}

void Spotify::updatePosition(UIControl* control) {
}

