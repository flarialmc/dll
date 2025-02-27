#pragma once

#include <lua.hpp>

class FlarialScript {
public:
    FlarialScript(std::string filePath, std::string code);

    bool compile();

    [[nodiscard]] const std::string& getName() const { return mName; }
    [[nodiscard]] const std::string& getDescription() const { return mDescription; }
    [[nodiscard]] const std::string& getAuthor() const { return mAuthor; }
    [[nodiscard]] lua_State* getState() const { return mState; }

    ~FlarialScript() {
        if (mState) {
            lua_close(mState);
            mState = nullptr;
        }
    }

private:
    std::string mFilePath;
    std::string mCode;
    lua_State* mState = nullptr;

    // Values extracted from script
    std::string mName;
    std::string mDescription;
    std::string mAuthor = "Unknown";
};