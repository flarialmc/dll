#pragma once

#include <string>
#include <unordered_map>

#include "../Actor/Actor.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "libhat/Access.hpp"
#include "HitResult.hpp"
#include "../../../Utils/Versions/WinrtUtils.hpp"

class Actor;

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
    char filler[0x164];
    //SerializedSkin skin;
    bool isTeacher, isHost;
};

class Level {
public:
    std::unordered_map<mcUUID, PlayerListEntry> &getPlayerMap() {
        return hat::member_at<std::unordered_map<mcUUID, PlayerListEntry>>(this, GET_OFFSET("Level::getPlayerMap"));
    }

    HitResult &getHitResult() {
        static int offset = GET_OFFSET("Level::hitResult");

        if (WinrtUtils::check(20, 60))
            return *hat::member_at<std::shared_ptr<HitResult>>(this, offset);

        return hat::member_at<HitResult>(this, offset);
    }

    std::vector<Actor *> getRuntimeActorList() {
        // TODO prevent crashing !!!
        static uintptr_t sig = GET_SIG_ADDRESS("Level::getRuntimeActorList");
        static auto getRuntimeActorList = *(decltype(&Level::getRuntimeActorList) *) &sig;
        return (this->*getRuntimeActorList)();
    }
};