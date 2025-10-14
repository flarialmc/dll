#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include "curl/curl/curl.h"
#include "json/json.hpp"
#include "../../../../Utils/APIUtils.hpp"
#include "../../../GUI/Engine/Elements/Structs/ImagesClass.hpp"
#include "../../../GUI/Engine/Constraints.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"
#include "Events/Render/UIControlGetPositionEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

struct SpotifyTrackInfo {
    std::string name;
    std::string artist;
    std::string album;
    std::string albumArtUrl;
    std::string albumArtPath;
    int duration_ms = 0;
    int progress_ms = 0;
    bool is_playing = false;
    std::chrono::steady_clock::time_point last_update;
    bool valid = false;
};

class Spotify : public Module {
private:
    SpotifyTrackInfo currentTrack;
    std::atomic<bool> isUpdating{false};
    std::thread updateThread;
    std::atomic<bool> shouldStop{false};
    
    // Movable functionality
    Vec2<float> currentPos{-120.0f, -120.0f};
    Vec2<float> currentSize{0.0f, 0.0f};
    Vec2<float> lastAppliedPos{0.0f, 0.0f};
    bool restored = false;
    
    // Spotify API credentials
    std::string client_id;
    std::string client_secret;
    std::string refresh_token;
    std::string access_token;
    bool credentialsLoaded = false;
    
    // Cached album art
    std::string cachedAlbumArtPath;
    bool albumArtDownloaded = false;
    
    // Update timing
    std::chrono::steady_clock::time_point lastApiCall;
    static constexpr int DEFAULT_UPDATE_INTERVAL_MS = 5000; // Default update every 5 seconds
    
    void loadCredentials();
    void refreshAccessToken();
    void updateTrackInfo();
    void downloadAlbumArt(const std::string& url);
    std::string formatTime(int milliseconds);
    std::string getAlbumArtPath(const std::string& albumArtUrl);
    void cleanupUpdateThread();

public:
    Spotify() : Module("Spotify", "Shows currently playing Spotify track with album art and progress", 
                       IDR_SPOTIFY_PNG, "", false, {"music", "spotify"}) {}

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;
    void terminate() override;

    void onRender(RenderEvent& event);
    void onRenderUnderUI(RenderUnderUIEvent& event);
    void onUIControlGetPosition(UIControlGetPositionEvent& event);
    void onSetupAndRender(SetupAndRenderEvent& event);
    void update();
    void updatePosition(UIControl* control);
};
