#pragma once

#include <Command/Command.hpp>

#include <HTTPRequest/HTTPRequest.hpp>
#include <json/json.hpp>
#include <curl/curl/curl.h>
#include "../../../../Utils/APIUtils.hpp"

using json = nlohmann::json;

class SpotifyController {
private:
    std::string client_id;
    std::string client_secret;
    std::string refresh_token;
    std::string access_token;
    const std::string token_url = "https://accounts.spotify.com/api/token";
    const std::string api_url = "https://api.spotify.com/v1/me/player/";

    void send_request(const std::string& url, const std::string& method = "PUT", const std::string& data = "") {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        auto [responseCode, responseBody] = APIUtils::Request(url, method, data, headers);
        if (responseCode >= 200 && responseCode < 300) {
            std::cout << "[✅] Request successful: " << responseBody << "\n";
        }
        else {
            std::cout << "[❌] Request failed (" << responseCode << "): " << responseBody << "\n";
        }
    }


public:
    SpotifyController(const std::string& clientID, const std::string& clientSecret, const std::string& refreshToken)
        : client_id(clientID), client_secret(clientSecret), refresh_token(refreshToken) {
        refresh_access_token();
    }

    void refresh_access_token() {
        std::cout << "Refreshing token..." << std::endl;

        std::string post_data =
            "grant_type=refresh_token&refresh_token=" + refresh_token +
            "&client_id=" + client_id +
            "&client_secret=" + client_secret;

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        auto [responseCode, responseBody] = APIUtils::Request("https://accounts.spotify.com/api/token", "POST", post_data, headers);

        if (responseCode == 200) {
            json jsonData = json::parse(responseBody);
            if (jsonData.contains("access_token")) {
                access_token = jsonData["access_token"].get<std::string>();
                std::cout << "[✅] Access token refreshed successfully!\n";
            }
            else {
                std::cerr << "[❌] Failed to refresh token: " << jsonData.dump(4) << "\n";
            }
        }
        else {
            std::cerr << "[❌] Token refresh failed with response code: " << responseCode << "\n";
        }
    }

    // Spotify Controls
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
        auto [responseCode, responseBody] = APIUtils::Request(search_url, "GET", "", headers);

        if (responseCode >= 200 && responseCode < 300) {
            json jsonData = json::parse(responseBody);
            if (jsonData.contains("item")) {
                std::string SongName = jsonData["item"]["name"].get<std::string>();
                std::string Artist = jsonData["item"]["artists"][0]["name"].get<std::string>();

                return SongName + " by " + Artist;
            }
        }

        return "";
    }

    // Search and Play a Song by Name
    void play_song_by_name(const std::string& song_name) {
        std::string search_url = "https://api.spotify.com/v1/search?q=" + song_name + "&type=track&limit=1";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Search for the song
        auto [responseCode, responseBody] = APIUtils::Request(search_url, "GET", "", headers);

        if (responseCode >= 200 && responseCode < 300) {
            json jsonData = json::parse(responseBody);
            if (jsonData.contains("tracks") && !jsonData["tracks"]["items"].empty()) {
                std::string track_uri = jsonData["tracks"]["items"][0]["uri"].get<std::string>();
                json play_data = { {"uris", {track_uri}} };

                std::cout << "[🎵] Playing song: " << song_name << "\n";

                // Play the song
                send_request(api_url + "play", "PUT", play_data.dump());
            }
            else {
                std::cerr << "[❌] No track found for: " << song_name << "\n";
            }
        }
        else {
            std::cerr << "[❌] Search failed with response code: " << responseCode << "\n";
        }
    }
};

class SpotifyCommand : public Command {
public:
    SpotifyController spotify = SpotifyController("", "", "");
    SpotifyCommand() : Command("spotify", "Controlls spotify") {};
    void execute(const std::vector<std::string>& args) override;
};