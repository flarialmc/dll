#pragma once

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
class ChatScreenController {
public:
    BUILD_ACCESS(this, bool, refreshChatMessages, GET_OFFSET("ChatScreenController::refreshChatMessages"));
};
