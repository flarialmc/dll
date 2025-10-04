#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Client.hpp"
#include "../../SDK/Client/Network/Packet/PlayerSkinPacket.hpp"
#include "image_data.h"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class CapeManager : public Listener {
public:
    void onKey(KeyEvent& event) {
        if (SDK::clientInstance->getLocalPlayer()) {
            auto player = SDK::clientInstance->getLocalPlayer();

            auto skin = player->getSkin();
            Blob data;
            data.fromVector(image_data);
            skin->mCapeImage.mImageBytes = data;

            for (auto playerr : player->getLevel()->getPlayerMap()) {
                if (playerr.second.name == "ANSHUL MASTER") {

                    auto packet = SDK::createPacket(0x5D);
                    auto* pocket = reinterpret_cast<PlayerSkinPacket*>(packet.get());
                    pocket->mUUID = playerr.first;
                    pocket->mSkin = *skin;
                    pocket->mLocalizedOldSkinName = "balls";
                    pocket->mLocalizedNewSkinName = "NewBalls";
                    SDK::clientInstance->getPacketSender()->sendToServer(pocket);
                }
            }
        }
    }

    CapeManager() {
        Listen(this, KeyEvent, &CapeManager::onKey);
    }

    ~CapeManager() {
        Deafen(this, KeyEvent, &CapeManager::onKey);
    }
};
