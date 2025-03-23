#include "SpotifyCommand.hpp"

#include "../../CommandManager.hpp"

std::vector<std::string> argss = {};

void SpotifyCommand::execute(const std::vector<std::string>& args) {
    if (!spotify.SetupSuccess()) {
        addCommandMessage("Spotify Credentials are not setup");
    }
    argss = args;
    std::thread thread([&]() {
        std::string msg = "";
        const std::string command = String::toLower(argss[0]);

        if (command == "resume") {
            spotify.play();
            msg = ("Started Playing.");
        }
        else if (command == "pause") {
            spotify.pause();
            msg = ("Stopped Playing.");
        }
        else if (command == "next") {
            spotify.next_track();
            msg = ("Skipped to next track.");
        }
        else if (command == "previous") {
            spotify.previous_track();
            msg = ("Playing the previous track.");
        }
        else if (command == "volume") {
            spotify.set_volume(argss[1]);
            msg = ("Volume set to " + argss[1] + "%");
        }

        else if (command == "play") {
            if (argss.size() <= 2) {
                msg = ("Please give the name of song");
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

            msg = ("Started playing " + spotify.get_song_name());
        }

        else if (command == "name") msg = ("Playing " + spotify.get_song_name());

        else msg = ("Invalid Command.");


        SDK::clientInstance->getGuiData()->displayClientMessage("§r§l§9spotify §3§l»§r " + msg);
        });

    thread.detach();
}
