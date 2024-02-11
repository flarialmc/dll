#pragma once

#include "SDK.hpp"

ClientInstance* SDK::clientInstance = nullptr;
ScreenView* SDK::screenView = nullptr;
std::string SDK::CurrentScreen;
bool SDK::hasInstanced = false;

std::shared_ptr<Packet> SDK::createPacket(int id) {
 
    static uintptr_t Address;

    if (Address == NULL) {
        Address = Memory::findSig("40 53 48 83 EC 30 45 33 C0 48 8B D9 FF CA 81 FA ? ? ? ? 0F 87 ? ? ? ? 48 63 C2 48 8D 0D ? ? ? ? 0F B6 84 01 ? ? ? ? 8B 94 81 ? ? ? ? 48 03 D1");
    }

    auto pFunction = reinterpret_cast<std::shared_ptr<Packet>(__fastcall*)(int)>(Address);
    return pFunction(id);
}

void SDK::setCI() {
    struct bruh {
        struct bruh2 {
            struct bruh3 {
                char pad[0x58];
                struct bruh4 {
                    ClientInstance* ci;
                } *bruh4;
            } *bruh3;
        } *bruh2;
    };

    static bruh* off;

    if (off == NULL) {
        uintptr_t addr = Memory::findSig("48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 83 39 ? 74 ? 48 8B 05 ? ? ? ? 48 85 C0 74 ? F0 FF 40 ? 48 8B 05 ? ? ? ? 48 8B 0D ? ? ? ? 48 89 43 ? 48 8B C3 48 89 3B C6 43 ? ? 48 89 4B ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 33 C0 48 8B CF 48 89 03 88 43 ? 48 89 43 ? 48 89 43 ? E8 ? ? ? ? 48 8B C3 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC 48 89 5C 24 ? 48 89 74 24 ? 48 89 4C 24 ? 57 48 83 EC ? 48 8B F9 33 F6 89 31 48 89 71 ? 48 89 71 ? 48 83 C1 ? E8 ? ? ? ? 90 48 8D 4F ? 48 89 31 48 89 71 ? 48 89 71 ? 48 C7 47 ? ? ? ? ? 48 C7 47 ? ? ? ? ? C7 07 ? ? ? ? 4C 8B 47 ? 8D 56 ? E8 ? ? ? ? 90 48 8B C7 48 8B 5C 24 ? 48 8B 74 24 ? 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC ? 48 8B D9 48 8B 49 ? 48 85 C9 74 ? 48 8B 43 ? 48 8B 53");
        off = reinterpret_cast<bruh*>(Memory::offsetFromSig(addr, 3));
    }

    clientInstance = off->bruh2->bruh3->bruh4->ci;
}
