#include "SpotifyCommand.hpp"

#include "../../CommandManager.hpp"

std::vector<std::string> argss = {};

void SpotifyCommand::execute(const std::vector<std::string>& args) {
    if (!spotify.SetupSuccess()) {
        addCommandMessage("Spotify Credentials are not setup");
        return;
    }
    argss = args; // ???
    std::thread thread([&]() {
        std::string msg = "";
        const std::string command = String::toLower(argss[0]);

        if (command == "resume") {
            spotify.play();
            msg = ("Started Playing.");
        }
        else if (command == "pause" || command == "stop") {
            spotify.pause();
            msg = ("Stopped Playing.");
        }
        else if (command == "next" || command == "skip") {
            spotify.next_track();
            msg = ("Skipped to next track.");
        }
        else if (command == "previous" || command == "back") {
            spotify.previous_track();
            msg = ("Playing the previous track.");
        }
        else if (command == "volume") {
            spotify.set_volume(argss[1]);
            msg = ("Volume set to " + argss[1] + "%");
        }
        else if (command == "play") {
            if (argss.size() <= 2) {
                msg = "Please provide the name of the song you want to play.";
                addCommandMessage(msg);
                return;
            }
            std::ostringstream oss;
            for (size_t i = 1; i < argss.size(); ++i) {
                if (i > 1) oss << " ";
                oss << argss[i];
            }

            std::string output;
            for (char c : (oss.str() + "     ")) {
                if (c == ' ') {
                    output += "%20";
                }
                else {
                    output += c;
                }
            }
            spotify.play_song_by_name(output);

            std::this_thread::sleep_for(std::chrono::seconds(1)); //needed or else it will print previous song name
            std::string songName = spotify.get_song_name();
            if (songName.empty()) {
                msg = "Started playing, but could not retrieve song info.";
            } else {
                msg = "Started playing " + songName;
            }
        }
        else if (command == "name") msg = ("Playing " + spotify.get_song_name());
        else msg = ("Invalid Command. (Use .spotify help to get a list of commands!)");

        addCommandMessage(msg);
    });

    thread.detach();
}
