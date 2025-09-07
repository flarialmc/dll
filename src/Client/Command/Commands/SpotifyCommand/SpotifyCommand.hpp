#pragma once

#include <Command/Command.hpp>

#include <HTTPRequest/HTTPRequest.hpp>
#include <json/json.hpp>
#include <curl/curl/curl.h>
#include "flarial/include/api.hpp"

using json = nlohmann::json;

class SpotifyController {
    bool isFine = false;
    std::string client_id;
    std::string client_secret;
    std::string refresh_token;
    std::string access_token;
    const std::string token_url = "https://accounts.spotify.com/api/token";
    const std::string api_url = "https://api.spotify.com/v1/me/player/";

    void send_request(const std::string& url, const std::string& method = "PUT", const std::string& data = "") {
        /*struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        auto [responseCode, responseBody] = api::request(url, method, data, headers);
        if (responseCode < 200 && responseCode >= 300) {
            LOG_ERROR("Spotify request failed ({}) : {}", responseCode, responseBody);
        }*/
    }


public:
    SpotifyController() {
        std::ifstream file(Utils::getClientPath() + "\\Spotify.json");
        if (!file) {
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Spotify.json not found.");
            return;
        }

        try {
            json j;
            file >> j;
            client_id = j["id"];
            client_secret = j["secret"];
            refresh_token = j["refresh_token"];
        } catch (const std::exception& e) {
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Failed to parse credentials: {}", e.what());
            return;
        }
        
        refresh_access_token();

        isFine = true;
    }

    bool SetupSuccess(){ return isFine; } // ???

    void refresh_access_token() {
        Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Refreshing access token...");

        std::string post_data =
            "grant_type=refresh_token&refresh_token=" + refresh_token +
            "&client_id=" + client_id +
            "&client_secret=" + client_secret;

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        /*auto [responseCode, responseBody] = api::request("https://accounts.spotify.com/api/token", "POST", post_data, headers);

        if (responseCode == 200) {
            json jsonData = json::parse(responseBody);
            if (jsonData.contains("access_token")) {
                access_token = jsonData["access_token"].get<std::string>();
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Access token refreshed successfully!");
            }
            else {
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Failed to refresh token: {}", jsonData.dump(4));
            }
        }
        else {
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Token refresh failed with response code: {}", responseCode);
        }*/
    }

    void play() { send_request(api_url + "play"); }
    void pause() { send_request(api_url + "pause"); }
    void next_track() { send_request(api_url + "next", "POST"); }
    void previous_track() { send_request(api_url + "previous", "POST"); }
    void set_volume(std::string volume) { send_request(api_url + "volume?volume_percent=" + volume); }
    void set_shuffle(bool enable) { send_request(api_url + "shuffle?state=" + (enable ? "true" : "false")); }
    void seek(int position_ms) { send_request(api_url + "seek?position_ms=" + std::to_string(position_ms)); }
    void set_repeat(const std::string& mode) { send_request(api_url + "repeat?state=" + mode); }

    std::string get_song_name() {
        std::string search_url = "https://api.spotify.com/v1/me/player/currently-playing";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        /*auto [responseCode, responseBody] = api::request(search_url, "GET", "", headers);

        if (responseCode >= 200 && responseCode < 300) {
            try {
                json jsonData = json::parse(responseBody);
                if (jsonData.contains("item")) {
                    std::string songName = jsonData["item"]["name"].get<std::string>();
                    std::string artist = jsonData["item"]["artists"][0]["name"].get<std::string>();

                    return songName + " by " + artist;
                }
            } catch (const json::parse_error& e) {
                LOG_ERROR("JSON Parse Error: " + std::string(e.what()));
            } catch (const json::type_error& e) {
                LOG_ERROR("JSON Type Error: " + std::string(e.what()));
            }
        }*/

        return "";
    }

    void play_song_by_name(const std::string& song_name) {
        std::string search_url = "https://api.spotify.com/v1/search?q=" + song_name + "&type=track&limit=1";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        /*auto [responseCode, responseBody] = api::request(search_url, "GET", "", headers);

        if (responseCode >= 200 && responseCode < 300) {
            json jsonData = json::parse(responseBody);
            if (jsonData.contains("tracks") && !jsonData["tracks"]["items"].empty()) {
                std::string track_uri = jsonData["tracks"]["items"][0]["uri"].get<std::string>();
                json play_data = { {"uris", {track_uri}} };

                send_request(api_url + "play", "PUT", play_data.dump());
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Playing Song: {}", song_name);
            }
            else {
                Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "No track found for: {}", song_name);
            }
        }
        else {
            Logger::custom(fg(fmt::color::light_green), "SPOTIFY", "Search failed with response code: {}", responseCode);
        }*/
    }
};

class SpotifyCommand : public Command {
public:
    SpotifyController spotify;
    SpotifyCommand() : Command("spotify", "Controls spotify", {"s"}) {};
    void execute(const std::vector<std::string>& args) override;
};