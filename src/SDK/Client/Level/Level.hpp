#pragma once

#include <string>
#include <unordered_map>

#include "libhat/Access.hpp"
#include "HitResult.hpp"

#include <Utils/VersionUtils.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <SDK/Client/Util/mce.hpp>

class mcUUID {
public:
    uint64_t mostSig, leastSig;
};

enum PlayerListPacketType : int8_t {
    AddPlayerEntry = 0x0,  // The names are just add and removed but i dont want to cause issues
    RemovePlayerEntry = 0x1,
};

class PlayerListEntry {
public:
    uint64_t id; // This is the ActorUniqueID
    mcUUID UUID;
    std::string name, XUID, platformOnlineId;
    int buildPlatform;
    PlayerSkin playerSkin;
    bool isTeacher, isHost, isSubClient;
    PlayerListEntry& operator=(const PlayerListEntry& other) {
        return *this;
    }
};

class Level {
public:
    std::unordered_map<mcUUID, PlayerListEntry> &getPlayerMap() {
        if (VersionUtils::checkAboveOrEqual(21, 40)) {
            return *hat::member_at<std::unordered_map<mcUUID, PlayerListEntry>*>(this, GET_OFFSET("Level::getPlayerMap"));
        } else {
            return hat::member_at<std::unordered_map<mcUUID, PlayerListEntry>>(this, GET_OFFSET("Level::getPlayerMap"));
        }
    }

    HitResult &getHitResult() {
        static int offset = GET_OFFSET("Level::hitResult");

        if (VersionUtils::checkAboveOrEqual(20, 60))
            return *hat::member_at<std::shared_ptr<HitResult>>(this, offset);

        return hat::member_at<HitResult>(this, offset);
    }

    std::string getWorldFolderName() { return hat::member_at<std::string>(this, GET_OFFSET("Level::worldFolderName")); }

    std::vector<Actor *> getRuntimeActorList();
};