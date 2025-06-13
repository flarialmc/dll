#pragma once

#include "../Render/TextureGroup.hpp"

class ThirdPartyServer
{
public:
    std::string mCreatorName;
    std::string mProductId;
    std::string mCreatorId;
    std::string mTitle;
private:
    char pad[0x18];
public:
    std::string mDescription;
private:
    char pad2[0x18];
public:
    std::string mNews;
    std::string mNewsTitle;
    std::string mAllowListUrl;
private:
    char pad3[0x20];
public:
    std::string mServerUrl;
    int mServerPort;
    bool mRequireXBL;
    bool mIsImageFinished;
};

class ThirdPartyInfo
{
    char pad[0x88];
};

class ThirdPartyServerRepository
{
public:
    std::unordered_map<std::string,std::pair<ThirdPartyInfo,std::vector<std::shared_ptr<ThirdPartyServer>>>> mTestThirdPartyServers;
    std::unordered_map<std::string,std::pair<ThirdPartyInfo,std::vector<std::shared_ptr<ThirdPartyServer>>>> mThirdPartyServers;
};

class MinecraftGame {
public:
    //BUILD_ACCESS(this, bool, mouseGrabbed, GET_OFFSET("MinecraftGame::mouseGrabbed"));
    BUILD_ACCESS(this, std::shared_ptr<TextureGroup>, textureGroup, GET_OFFSET("MinecraftGame::textureGroup"));
    void _onResumeWaitReloadActors()
    {
        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeWaitReloadActors");
        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
        fn(this);
    }

    Bedrock::NonOwnerPointer<ThirdPartyServerRepository> getThirdPartyServerRepository() {
        if (!this) return nullptr;
        auto vtable = *reinterpret_cast<void***>(this);
        if (!vtable || !vtable[37]) return nullptr;

        Bedrock::NonOwnerPointer<ThirdPartyServerRepository> var {};
         return Memory::CallVFuncI<Bedrock::NonOwnerPointer<ThirdPartyServerRepository>>(37,this, &var);
        //return reinterpret_cast<Fn>(vtable[37])(this, var);
    }

//    void _onResumeWaitPreviousResourceLoads() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeWaitPreviousResourceLoads");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
//    void _onResumeQueueReloadTextures() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeQueueReloadTextures");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
//    void _onResumeReloadTextures() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeReloadTextures");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
};
