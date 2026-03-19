#include "BedrockLogHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <cstdio>

static const char* getAreaName(int area) {
    switch (area) {
        case 0:  return "All";
        case 1:  return "Platform";
        case 2:  return "Entity";
        case 3:  return "Database";
        case 4:  return "GUI";
        case 5:  return "System";
        case 6:  return "Network";
        case 7:  return "Render";
        case 8:  return "Memory";
        case 9:  return "Animation";
        case 10: return "Input";
        case 11: return "Level";
        case 12: return "Server";
        case 13: return "DLC";
        case 14: return "Physics";
        case 15: return "File";
        case 16: return "Storage";
        case 17: return "Realms";
        case 18: return "RealmsAPI";
        case 19: return "XboxLive";
        case 20: return "UserManager";
        case 21: return "XSAPI";
        case 22: return "Perf";
        case 23: return "Telemetry";
        case 24: return "Blocks";
        case 25: return "RakNet";
        case 26: return "GameFace";
        case 27: return "Sound";
        case 28: return "Interactive";
        case 29: return "Scripting";
        case 30: return "PlayFab";
        case 31: return "Automation";
        case 32: return "Persona";
        case 33: return "Texture";
        case 34: return "AssetPackages";
        case 35: return "Items";
        case 36: return "Services";
        case 37: return "Volumes";
        case 38: return "LootTable";
        case 39: return "Sidebar";
        case 40: return "Localization";
        case 41: return "Movement";
        case 42: return "LiveEvents";
        case 43: return "Editor";
        case 44: return "LevelTransition";
        case 45: return "Unknown";
        case 46: return "Store";
        case 47: return "World";
        case 48: return "Messaging";
        case 49: return "Nethernet";
        case 50: return "Serialization";
        case 51: return "DisconnectionScreen";
        default: return "???";
    }
}

static const char* getPriorityName(unsigned int priority) {
    switch (priority) {
        case 1:  return "Verbose";
        case 2:  return "Info";
        case 4:  return "Warning";
        case 8:  return "Error";
        default: return "Log";
    }
}

static fmt::text_style getPriorityStyle(unsigned int priority) {
    switch (priority) {
        case 1:  return fmt::fg(fmt::color::gray);
        case 2:  return fmt::fg(fmt::color::light_green);
        case 4:  return fmt::fg(fmt::color::orange);
        case 8:  return fmt::fg(fmt::color::crimson);
        default: return fmt::fg(fmt::color::light_green);
    }
}

int BedrockLogHook::callback(void* logDetails, int area, unsigned int priority,
                             const char* function, int line, int messageId,
                             const char* pszFormat, va_list argptr) {

    if (pszFormat) {
        // va_copy so we don't consume the original argptr before passing it on
        va_list argsCopy;
        va_copy(argsCopy, argptr);

        char buf[4096];
        int len = vsnprintf(buf, sizeof(buf), pszFormat, argsCopy);
        va_end(argsCopy);

        if (len > 0) {
            // Trim trailing newlines
            while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
                buf[--len] = '\0';

            if (len > 0)
                Logger::custom(
                    getPriorityStyle(priority),
                    fmt::format("MC {} | {}", getAreaName(area), getPriorityName(priority)),
                    "{} (src: {}:{}, id:{})",
                    buf, function ? function : "anonymous", line, messageId
                );
        }
    }

    return funcOriginal(logDetails, area, priority, function, line, messageId, pszFormat, argptr);
}

BedrockLogHook::BedrockLogHook() : Hook("BedrockLogHook", GET_SIG_ADDRESS("BedrockLog::LogDetails::_log_va")) {}

void BedrockLogHook::enableHook() {
    this->autoHook((void*)callback, (void**)&funcOriginal);
}
