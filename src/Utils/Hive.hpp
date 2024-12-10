#pragma once

#include <string>
#include <windows.h>
#include <wininet.h>
#include <ImGui/imgui.h>
#include <lib/json/json.hpp>
#include <Utils/Logger/Logger.hpp>

#pragma comment(lib, "wininet.lib")

const std::map<int, int> xpToLevelBedWars = {
        {0, 0},
        {150, 1},
        {450, 2},
        {900, 3},
        {1500, 4},
        {2250, 5},
        {3150, 6},
        {4200, 7},
        {5400, 8},
        {6750, 9},
        {8250, 10},
        {9900, 11},
        {11700, 12},
        {13650, 13},
        {15750, 14},
        {18000, 15},
        {20400, 16},
        {22950, 17},
        {25650, 18},
        {28500, 19},
        {31500, 20},
        {34650, 21},
        {37950, 22},
        {41400, 23},
        {45000, 24},
        {48750, 25},
        {52650, 26},
        {56700, 27},
        {60900, 28},
        {65250, 29},
        {69750, 30},
        {74400, 31},
        {79200, 32},
        {84150, 33},
        {89250, 34},
        {94500, 35},
        {99900, 36},
        {105450, 37},
        {111150, 38},
        {117000, 39},
        {123000, 40},
        {129150, 41},
        {135450, 42},
        {141900, 43},
        {148500, 44},
        {155250, 45},
        {162150, 46},
        {169200, 47},
        {176400, 48},
        {183750, 49}
};


const std::map<int, int> xpToLevelSkyWars = {
        {0, 0},
        {150, 1},
        {450, 2},
        {900, 3},
        {1500, 4},
        {2250, 5},
        {3150, 6},
        {4200, 7},
        {5400, 8},
        {6750, 9},
        {8250, 10},
        {9900, 11},
        {11700, 12},
        {13650, 13},
        {15750, 14},
        {18000, 15},
        {20400, 16},
        {22950, 17},
        {25650, 18},
        {28500, 19},
        {31500, 20},
        {34650, 21},
        {37950, 22},
        {41400, 23},
        {45000, 24},
        {48750, 25},
        {52650, 26},
        {56700, 27},
        {60900, 28},
        {65250, 29},
        {69750, 30},
        {74400, 31},
        {79200, 32},
        {84150, 33},
        {89250, 34},
        {94500, 35},
        {99900, 36},
        {105450, 37},
        {111150, 38},
        {117000, 39},
        {123000, 40},
        {129150, 41},
        {135450, 42},
        {141900, 43},
        {148500, 44},
        {155250, 45},
        {162150, 46},
        {169200, 47},
        {176400, 48},
        {183750, 49},
        {191250, 50},
        {198900, 51},
        {206550, 52},
        {214200, 53},
        {221850, 54},
        {229500, 55},
        {237150, 56},
        {244800, 57},
        {252450, 58},
        {260100, 59},
        {267750, 60},
        {275400, 61},
        {283050, 62},
        {290700, 63},
        {298350, 64},
        {306000, 65},
        {313650, 66},
        {321300, 67},
        {328950, 68},
        {336600, 69},
        {344250, 70},
        {351900, 71},
        {359550, 72},
        {367200, 73},
        {374850, 74},
        {382500, 75},
        {390150, 76},
        {397800, 77},
        {405450, 78},
        {413100, 79},
        {420750, 80},
        {428400, 81},
        {436050, 82},
        {443700, 83},
        {451350, 84},
        {459000, 85},
        {466650, 86},
        {474300, 87},
        {481950, 88},
        {489600, 89},
        {497250, 90},
        {504900, 91},
        {512550, 92},
        {520200, 93},
        {527850, 94},
        {535500, 95},
        {543150, 96},
        {550800, 97},
        {558450, 98},
        {566100, 99}
};


const std::map<int, int> xpToLevelMurderMystery = {
        {0, 0},
        {100, 1},
        {300, 2},
        {600, 3},
        {1000, 4},
        {1500, 5},
        {2100, 6},
        {2800, 7},
        {3600, 8},
        {4500, 9},
        {5500, 10},
        {6600, 11},
        {7800, 12},
        {9100, 13},
        {10500, 14},
        {12000, 15},
        {13600, 16},
        {15300, 17},
        {17100, 18},
        {19000, 19},
        {21000, 20},
        {23100, 21},
        {25300, 22},
        {27600, 23},
        {30000, 24},
        {32500, 25},
        {35100, 26},
        {37800, 27},
        {40600, 28},
        {43500, 29},
        {46500, 30},
        {49600, 31},
        {52800, 32},
        {56100, 33},
        {59500, 34},
        {63000, 35},
        {66600, 36},
        {70300, 37},
        {74100, 38},
        {78000, 39},
        {82000, 40},
        {86100, 41},
        {90300, 42},
        {94600, 43},
        {99000, 44},
        {103500, 45},
        {108100, 46},
        {112800, 47},
        {117600, 48},
        {122500, 49},
        {127500, 50},
        {132600, 51},
        {137800, 52},
        {143100, 53},
        {148500, 54},
        {154000, 55},
        {159600, 56},
        {165300, 57},
        {171100, 58},
        {177000, 59},
        {183000, 60},
        {189100, 61},
        {195300, 62},
        {201600, 63},
        {208000, 64},
        {214500, 65},
        {221100, 66},
        {227800, 67},
        {234600, 68},
        {241500, 69},
        {248500, 70},
        {255600, 71},
        {262800, 72},
        {270100, 73},
        {277500, 74},
        {285000, 75},
        {292600, 76},
        {300300, 77},
        {308100, 78},
        {316000, 79},
        {324000, 80},
        {332100, 81},
        {340200, 82},
        {348300, 83},
        {356400, 84},
        {364500, 85},
        {372600, 86},
        {380700, 87},
        {388800, 88},
        {396900, 89},
        {405000, 90},
        {413100, 91},
        {421200, 92},
        {429300, 93},
        {437400, 94},
        {445500, 95},
        {453600, 96},
        {461700, 97},
        {469800, 98},
        {477900, 99}
};

const std::map<int, int> xpToLevelCTF = {
        {0, 0},
        {150, 1},
        {450, 2},
        {900, 3},
        {1500, 4},
        {2250, 5},
        {3150, 6},
        {4200, 7},
        {5400, 8},
        {6750, 9},
        {8250, 10},
        {9900, 11},
        {11700, 12},
        {13650, 13},
        {15750, 14},
        {18000, 15},
        {20400, 16},
        {22950, 17},
        {25650, 18},
        {28500, 19},
        {31500, 20},
        {34650, 21},
        {37950, 22},
        {41400, 23},
        {45000, 24},
        {48750, 25},
        {52650, 26},
        {56700, 27},
        {60900, 28},
        {65250, 29},
        {69750, 30},
        {74400, 31},
        {79200, 32},
        {84150, 33},
        {89250, 34},
        {94500, 35},
        {99900, 36},
        {105450, 37},
        {111150, 38},
        {117000, 39},
        {123000, 40},
        {129150, 41},
        {135450, 42},
        {141900, 43},
        {148500, 44},
        {155250, 45},
        {162150, 46},
        {169200, 47},
        {176400, 48},
        {183750, 49}
};




namespace Hive {

    class PlayerStats {
    private:
        float fkdr;
        float kd;
        float winRate;
        int level;
        int code = 0;

    public:
        PlayerStats(float fkdr = 0.0f, float kd = 0.0f, float winRate = 0.0f)
                : fkdr(fkdr), kd(kd), winRate(winRate) {}

        void setFKDR(double fkdrValue) { fkdr = fkdrValue; }
        void setKD(double kdValue) { kd = kdValue; }
        void setWinRate(double winRateValue) { winRate = winRateValue; }
        void setCode(int codeValue){ code = codeValue; }
        void setLevel(int levelValue){ level = levelValue; }

        float getFKDR() const { return fkdr; }
        float getKD() const { return kd; }
        float getWinRate() const { return winRate; }
        int getLevel() const { return level; }
        int getCode() const { return code; };
    };

    double roundToSecond(float value) {
        return std::round(value * 100.0f) / 100.0f;
    }



    std::string GetString(const std::string &URL) {
        HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
        HINTERNET urlFile;
        std::string rtn;

        if (interwebs) {
            urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
            if (urlFile) {
                char buffer[2000];
                DWORD bytesRead;
                do {
                    InternetReadFile(urlFile, buffer, 2000, &bytesRead);
                    rtn.append(buffer, bytesRead);
                    memset(buffer, 0, 2000);
                } while (bytesRead);
                InternetCloseHandle(urlFile);
            }
            InternetCloseHandle(interwebs);
        }

        return rtn;
    }

    std::string replaceAll(std::string subject, const std::string &search, const std::string &replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    ImVec4 calculateWinRateColor(float winrate){
        if(winrate < 25.0f ){
            return ImVec4(255.0f, 72.0f, 59.0f, 1.0f);
        }
        else if(winrate < 50.0f ){
            return ImVec4(251.0f, 255.0f, 38.0f, 1.0f);
        }else {
            return ImVec4(61.0f, 255.0f, 43.0f, 1.0f);
        }

    }

    int getLevelForXP(int xp, const std::map<int, int>& xpToLevel) {
        auto it = xpToLevel.lower_bound(xp);

        if (it == xpToLevel.end()) {
            return (--it)->second + 1;
        }

        if (it == xpToLevel.begin()) {
            return it->second + 1;
        }

        if (it->first == xp) {
            return it->second + 1;
        }

        auto prev = std::prev(it);
        return prev->second + 1;
    }

    Hive::PlayerStats GetStats(const std::string &gameId, const std::string &username) {

        PlayerStats stats;

        try {
            std::string url = "https://api.playhive.com/v0/game/all/" + gameId + "/" + username;
            std::string jsonResponse = GetString(url);


            nlohmann::json jsonData = nlohmann::json::parse(jsonResponse);

            Logger::info(jsonResponse);
            if(!jsonData.contains("first_played")){
                stats.setCode(1);
                return stats;
            }

            if(gameId == "bed") {
                if(!jsonData.contains("final_kills") || !jsonData.contains("kills") || !jsonData.contains("deaths") || !jsonData.contains("victories") || !jsonData.contains("played") || !jsonData.contains("xp") || !jsonData.contains("beds_destroyed")){
                    stats.setCode(2);
                    return stats;
                }
                int finalKills = jsonData["final_kills"].get<int>();
                int xp = jsonData["xp"].get<int>();
                int kills = jsonData["kills"].get<int>();
                int deaths = jsonData["deaths"].get<int>();
                int victories = jsonData["victories"].get<int>();
                int played = jsonData["played"].get<int>();

                int finalDeaths = played - victories;


                stats.setFKDR(roundToSecond((float) finalKills / (float) finalDeaths));
                stats.setKD(roundToSecond((float) kills / (float) deaths));
                stats.setWinRate(std::round(((float) victories / (float) played) * 100.0f));
                stats.setLevel(getLevelForXP(xp, xpToLevelBedWars));
            }

            if(gameId == "sky") {


                if(!jsonData.contains("kills") || !jsonData.contains("deaths") || !jsonData.contains("victories") || !jsonData.contains("played") || !jsonData.contains("xp")){
                    stats.setCode(2);
                    return stats;
                }

                int kills = jsonData["kills"].get<int>();
                int deaths = jsonData["deaths"].get<int>();
                int victories = jsonData["victories"].get<int>();
                int played = jsonData["played"].get<int>();
                int xp = jsonData["xp"].get<int>();


                stats.setFKDR(roundToSecond((float) -1.f));
                stats.setKD(roundToSecond((float) kills / (float) deaths));
                stats.setWinRate(std::round(((float) victories / (float) played) * 100.0f));
                stats.setLevel(getLevelForXP(xp, xpToLevelSkyWars));
            }
            if(gameId == "murder") {
                if(!jsonData.contains("murders") || !jsonData.contains("murderer_eliminations") || !jsonData.contains("deaths") || !jsonData.contains("victories") || !jsonData.contains("played") || !jsonData.contains("xp")){
                    stats.setCode(2);
                    return stats;
                }

                int murders = jsonData["murders"].get<int>();
                int eliminations = jsonData["murderer_eliminations"].get<int>();
                int deaths = jsonData["deaths"].get<int>();
                int victories = jsonData["victories"].get<int>();
                int played = jsonData["played"].get<int>();
                int xp = jsonData["xp"].get<int>();

                stats.setFKDR(roundToSecond(-1.0f));

                stats.setKD(roundToSecond((float) murders / (float) deaths));

                stats.setWinRate(std::round(((float) victories / (float) played) * 100.0f));

                stats.setLevel(getLevelForXP(xp, xpToLevelMurderMystery));
            }


            if(gameId == "ctf") {
                if(!jsonData.contains("kills") || !jsonData.contains("deaths") || !jsonData.contains("victories") || !jsonData.contains("played") || !jsonData.contains("xp") || !jsonData.contains("flags_captured") || !jsonData.contains("flags_returned")){
                    stats.setCode(2);
                    return stats;
                }

                int kills = jsonData["kills"].get<int>();
                int deaths = jsonData["deaths"].get<int>();
                int victories = jsonData["victories"].get<int>();
                int played = jsonData["played"].get<int>();
                int xp = jsonData["xp"].get<int>();
                int flagsCaptured = jsonData["flags_captured"].get<int>();
                int flagsReturned = jsonData["flags_returned"].get<int>();
                int assists = jsonData["assists"].get<int>();

                stats.setKD(roundToSecond((float) kills / (float) deaths));

                stats.setFKDR(roundToSecond(-1.0f));

                stats.setWinRate(std::round(((float) victories / (float) played) * 100.0f));

                stats.setLevel(getLevelForXP(xp, xpToLevelCTF));
            }


            return stats;
        } catch (...) {
            return stats;
        }
    }

}