#pragma once

#include <string>
#include <utility>

class TextureData {
private:
    char pad_0x0[0x18];
    struct PathStruct {
        char pad_0x0[0x18];
        std::string filePath;
    } *ptrToPath;
    char pad_0x0038[0x20];

public:
    [[nodiscard]] std::string GetFilePath() const {
        return ptrToPath->filePath;
    }
};