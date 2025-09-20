#pragma once

#include <string>
#include "../../../Utils/path.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

enum ResourceFileSystem : int32_t {
    UserPackage = 0x0000,
    AppPackage = 0x0001,
    Raw = 0x0002,
    RawPersistent = 0x0003,
    SettingsDir = 0x0004,
    ExternalDir = 0x0005,
    ServerPackage = 0x0006,
    DataDir = 0x0007,
    UserDir = 0x0008,
    ScreenshotsDir = 0x0009,
    StoreCache = 0x000a,
    Invalid = 0x000b,
};

class ResourceLocation {
public:
    ResourceFileSystem fileSystem;
    std::string filePath;
    uint64_t pathHash{};
    uint64_t fullHash{};

    ResourceLocation() = default;

    ResourceLocation(const std::string& filePath, bool external) {
        this->filePath = filePath;
        if (external)
            this->fileSystem = ResourceFileSystem::Raw;
        else this->fileSystem = ResourceFileSystem::UserPackage;

        _computeHashes();
    };

    bool operator==(const ResourceLocation& other) const {
        if(this->fileSystem != other.fileSystem || this->pathHash != other.pathHash) return false;
        return this->filePath == other.filePath;
    }

    bool operator<(const ResourceLocation& other) const {
        return this->filePath < other.filePath;
    }

    // Get the full path to a texture file within a resource pack
    // Usage example:
    // ResourceLocation textureLocation("textures/items/apple.png", false);
    // Core::PathBuffer<std::string> fullPath;
    // textureLocation.getFullPath(&fullPath);
    // std::string texturePath = fullPath.getContainer();
    Core::PathBuffer<std::string>* getFullPath(Core::PathBuffer<std::string>* result) {
        static auto sig = GET_SIG_ADDRESS("ResourceLocation::getFullPath");
        if (!sig) return result;

        using GetFullPathFunc = Core::PathBuffer<std::string>*(__thiscall*)(ResourceLocation*, Core::PathBuffer<std::string>*);
        static auto getFullPathFunc = reinterpret_cast<GetFullPathFunc>(sig);
        return getFullPathFunc(this, result);
    }

private:
    void _computeHashes()
    {
        const uint64_t FNV_OFFSET_BASIS = 0xCBF29CE484222325u;
        const uint64_t FNV_PRIME = 0x100000001B3u;

        uint64_t _pathHash = FNV_OFFSET_BASIS;
        if (!this->filePath.empty()) {
            for (char c : this->filePath) {
                _pathHash *= FNV_PRIME;
                _pathHash ^= c;
            }
        } else {
            _pathHash = 0;
        }

        uint64_t fileSystemHash = FNV_OFFSET_BASIS ^ static_cast<uint8_t>(this->fileSystem);
        fileSystemHash *= FNV_PRIME;

        this->pathHash = _pathHash;
        this->fullHash = _pathHash ^ fileSystemHash;
    }
};