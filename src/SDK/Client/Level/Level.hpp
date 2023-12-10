#pragma once
#include <string>
#include <unordered_map>

class mcUUID {
public:
    uint64_t mostSig, leastSig;
};

class PlayerListEntry {
public:
    char pad_0000[24]; //0x0000
    std::string name; //0x0018
    char pad_0020[328]; //0x0020
    void *N0000007C; //0x0168
};

class Level {
public:
    char pad_0000[0x1EA8]; //0x0000
    std::unordered_map<mcUUID, PlayerListEntry> playermap; //0x1EA8
};