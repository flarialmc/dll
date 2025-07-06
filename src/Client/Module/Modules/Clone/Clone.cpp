#include "Clone.hpp"

#include <utility>

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"

void Clone::onEnable() {
    Listen(this, KeyEvent, &Clone::onKey)
    Listen(this, MouseEvent, &Clone::onMouse)
    Listen(this, TickEvent, &Clone::onTick)
    Module::onEnable();
}

void Clone::onDisable() {
    Deafen(this, KeyEvent, &Clone::onKey)
    Deafen(this, MouseEvent, &Clone::onMouse)
    Deafen(this, TickEvent, &Clone::onTick)
    Module::onDisable();
}

void Clone::defaultConfig() {
    getKeybind();
    Module::defaultConfig("core");
}

void Clone::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Clone");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();

    this->resetPadding();
}

bool isValidUtf8(const std::string &s) {
    int bytes_expected = 0; // Number of continuation bytes expected

    for (size_t i = 0; i < s.length(); ++i) {
        unsigned char byte = static_cast<unsigned char>(s[i]);

        if (bytes_expected == 0) {
            // Leading byte
            if (byte <= 0x7F) {
                // ASCII (0xxxxxxx)
                // Nothing to do, it's valid
            } else if ((byte & 0xE0) == 0xC0) {
                // 2-byte sequence (110xxxxx)
                bytes_expected = 1;
            } else if ((byte & 0xF0) == 0xE0) {
                // 3-byte sequence (1110xxxx)
                bytes_expected = 2;
            } else if ((byte & 0xF8) == 0xF0) {
                // 4-byte sequence (11110xxx)
                bytes_expected = 3;
            } else {
                // Invalid leading byte
                return false;
            }
        } else {
            // Continuation byte (10xxxxxx)
            if ((byte & 0xC0) == 0x80) {
                // Is a valid continuation byte
                bytes_expected--;
            } else {
                // Not a continuation byte
                return false;
            }
        }
    }

    // After iterating through all bytes, bytes_expected should be 0 if the string is valid.
    return bytes_expected == 0;
}

void Clone::onTick(TickEvent &event) {
    if (!clone) return;
    if (!SDK::clientInstance->getLocalPlayer() || SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().type != HitResultType::Entity) return;
    std::string targetName = String::removeNonAlphanumeric(String::removeColorCodes(String::toLower(*SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().getEntity()->getNametag())));
    std::string curName = String::removeNonAlphanumeric(String::removeColorCodes(String::toLower(*SDK::clientInstance->getLocalPlayer()->getNametag())));

    Logger::debug("{} {}", curName, targetName);

    for (const auto val: SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap() | std::views::values) {
        Logger::debug("{}", val.name);
        if (
            String::removeNonAlphanumeric(String::removeColorCodes(String::toLower(val.name))) != targetName ||
            !isValidUtf8(val.playerSkin.mId)
        )
            continue;

        std::shared_ptr<Packet> packet = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
        auto *pSkinPacket = reinterpret_cast<PlayerSkinPacket *>(packet.get());

        // memcpy(&pSkinPacket->mUUID, &val.UUID, sizeof(mcUUID));
        pSkinPacket->mUUID = val.UUID;
        pSkinPacket->mLocalizedOldSkinName = (std::string) "";
        pSkinPacket->mLocalizedNewSkinName = (std::string) "";
        pSkinPacket->mSkin = val.playerSkin;
        pSkinPacket->mTrusted = (bool)val.playerSkin.mIsTrustedSkin;
        // memcpy(&pSkinPacket->mSkin, &val.playerSkin, sizeof(PlayerSkin));
        // pSkinPacket->mSkin = target.playerSkin;
        SDK::clientInstance->getPacketSender()->sendToServer(pSkinPacket);
        Logger::debug("sent skin packet");
        break;

        // if (String::toLower(pair.second.name) == targetName) {
        //     // target = pair.second;
        //     memcpy(&target, &pair.second, sizeof(PlayerListEntry));
        //     ftarget = true;
        //     if (fme) break;
        // }
        // if (String::toLower(pair.second.name) == curName) {
        //     // me = pair.second;
        //     memcpy(&me, &pair.second, sizeof(PlayerListEntry));
        //     fme = true;
        //     if (ftarget) break;
        // }
        /*
                if (String::toLower(pair.second.name) == targetName) {
                    if (false) {
                        // std::cout << pair.second.id << std::endl;
                        // std::cout << pair.second.isHost << std::endl;
                        // std::cout << pair.second.isSubClient << std::endl;
                        // std::cout << pair.second.isTeacher << std::endl;
                        // std::cout << pair.second.name << std::endl;
                        // std::cout << pair.second.platformOnlineId << std::endl;
                        // std::cout << pair.second.XUID << std::endl;
                        // std::cout << (int) pair.second.buildPlatform << std::endl;
                        // std::cout << "Skin" << std::endl;
                        // Logger::info("mId: {}", pair.second.playerSkin.mId);
                        // Logger::info("mPlayFabId: {}", pair.second.playerSkin.mPlayFabId);
                        // Logger::info("mFullId: {}", pair.second.playerSkin.mFullId);
                        // Logger::info("mResourcePatch: {}", pair.second.playerSkin.mResourcePatch);
                        // Logger::info("mDefaultGeometryName: {}", pair.second.playerSkin.mDefaultGeometryName);
                        // Logger::info("mSkinImage: {}", pair.second.playerSkin.mSkinImage.mWidth > 0 ? "exists" : "doesn't exist L");
                        // Logger::info("mCapeImage: {}", pair.second.playerSkin.mCapeImage.mWidth > 0 ? "exists" : "doesn't exist L");
                        // Logger::info("mSkinAnimatedImages (size): {}", pair.second.playerSkin.mSkinAnimatedImages.size());
                        // // Logger::info("mGeometryData: {}", pair.second.playerSkin.mGeometryData);
                        // // Logger::info("mGeometryDataMinEngineVersion: {}", pair.second.playerSkin.mGeometryDataMinEngineVersion);
                        // // Logger::info("mGeometryDataMutable: {}", pair.second.playerSkin.mGeometryDataMutable);
                        // Logger::info("mAnimationData: {}", pair.second.playerSkin.mAnimationData);
                        // Logger::info("mCapeId: {}", pair.second.playerSkin.mCapeId);
                        // Logger::info("mPersonaPieces (size): {}", pair.second.playerSkin.mPersonaPieces.size());
                        // Logger::info("mArmSizeType: {}", (int) pair.second.playerSkin.mArmSizeType);
                        // Logger::info("mPieceTintColors (size): {}", pair.second.playerSkin.mPieceTintColors.size());
                        // // Logger::info("mSkinColor R: {}", pair.second.playerSkin.mSKinColor.r);
                        // // Logger::info("mSkinColor G: {}", pair.second.playerSkin.mSKinColor.g);
                        // // Logger::info("mSkinColor B: {}", pair.second.playerSkin.mSKinColor.b);
                        // Logger::info("mIsTrustedSkin: {}", (bool) pair.second.playerSkin.mIsTrustedSkin);
                        // Logger::info("mIsPremium: {}", (bool) pair.second.playerSkin.mIsPremium);
                        // Logger::info("mIsPersona: {}", (bool) pair.second.playerSkin.mIsPersona);
                        // Logger::info("mIsPersonaCapeOnClassicSkin: {}", (bool) pair.second.playerSkin.mIsPersonaCapeOnClassicSkin);
                        // Logger::info("mIsPrimaryUser: {}", (bool) pair.second.playerSkin.mIsPrimaryUser);
                        // Logger::info("mOverridesPlayerAppearance: {}", (bool) pair.second.playerSkin.mOverridesPlayerAppearance);
                        //
                        // std::cout << pair.second.playerSkin.mId << std::endl;
                        //
                        //
                        // std::cout << "-----------------------" << std::endl;
                        // std::cout << &SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap() << std::endl;
                        // Logger::info("mId: {}", pair.second.playerSkin.mId);
                        // Logger::info("mPlayFabId: {}", pair.second.playerSkin.mPlayFabId);
                        // Logger::info("mFullId: {}", pair.second.playerSkin.mFullId);
                        // Logger::info("mResourcePatch: {}", pair.second.playerSkin.mResourcePatch);
                        // Logger::info("mDefaultGeometryName: {}", pair.second.playerSkin.mDefaultGeometryName);
                        // Logger::info("mSkinImage: {}", pair.second.playerSkin.mSkinImage.mWidth > 0 ? "exists" : "doesn't exist L");
                        // Logger::info("mCapeImage: {}", pair.second.playerSkin.mCapeImage.mWidth > 0 ? "exists" : "doesn't exist L");
                        // Logger::info("mSkinAnimatedImages (size): {}", pair.second.playerSkin.mSkinAnimatedImages.size());
                        // // Logger::info("mGeometryData: {}", pair.second.playerSkin.mGeometryData);
                        // // Logger::info("mGeometryDataMinEngineVersion: {}", pair.second.playerSkin.mGeometryDataMinEngineVersion);
                        // // Logger::info("mGeometryDataMutable: {}", pair.second.playerSkin.mGeometryDataMutable);
                        // Logger::info("mAnimationData: {}", pair.second.playerSkin.mAnimationData);
                        // Logger::info("mCapeId: {}", pair.second.playerSkin.mCapeId);
                        // Logger::info("mPersonaPieces (size): {}", pair.second.playerSkin.mPersonaPieces.size());
                        // Logger::info("mArmSizeType: {}", (int)pair.second.playerSkin.mArmSizeType);
                        // Logger::info("mPieceTintColors (size): {}", pair.second.playerSkin.mPieceTintColors.size());
                        // // Logger::info("mSkinColor: {}", pair.second.playerSkin.mSkinColor ? "exists" : "doesn't exist L");
                        // Logger::info("mIsTrustedSkin: {}", (bool)pair.second.playerSkin.mIsTrustedSkin);
                        // Logger::info("mIsPremium: {}", (bool)pair.second.playerSkin.mIsPremium);
                        // Logger::info("mIsPersona: {}", (bool)pair.second.playerSkin.mIsPersona);
                        // Logger::info("mIsPersonaCapeOnClassicSkin: {}", (bool)pair.second.playerSkin.mIsPersonaCapeOnClassicSkin);
                        // Logger::info("mIsPrimaryUser: {}", (bool)pair.second.playerSkin.mIsPrimaryUser);
                        // Logger::info("mOverridesPlayerAppearance: {}", (bool)pair.second.playerSkin.mOverridesPlayerAppearance);
                        //
                        std::shared_ptr<Packet> packet = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
                        auto *pSkinPacket = reinterpret_cast<PlayerSkinPacket *>(packet.get());

                        memcpy(&pSkinPacket->mUUID, &pair.second.UUID, sizeof(mcUUID));
                        pSkinPacket->mLocalizedOldSkinName = (std::string) "Custom";
                        pSkinPacket->mLocalizedNewSkinName = (std::string) "Custom";
                        pSkinPacket->mSkin = pair.second.playerSkin;
                        SDK::clientInstance->getPacketSender()->sendToServer(pSkinPacket);
                        Logger::debug("sent skin packet");
                    }

                    break;

                }*/
    }

    clone = false;

    // Logger::debug("{} {}", targetName, curName);
    // Logger::debug("{} {}", ftarget, fme);
    // Logger::debug("{} {}", me.playerSkin.mId, target.playerSkin.mId);

    // if (fme && ftarget && me.playerSkin.mId != target.playerSkin.mId) {
    // std::shared_ptr<Packet> packet = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
    // auto *pSkinPacket = reinterpret_cast<PlayerSkinPacket *>(packet.get());
    //
    // memcpy(&pSkinPacket->mUUID, &target.UUID, sizeof(mcUUID));
    // pSkinPacket->mLocalizedOldSkinName = (std::string) "Custom";
    // pSkinPacket->mLocalizedNewSkinName = (std::string) "Custom";
    // memcpy(&pSkinPacket->mSkin, &target.playerSkin, sizeof(PlayerSkin));
    // // pSkinPacket->mSkin = target.playerSkin;
    // SDK::clientInstance->getPacketSender()->sendToServer(pSkinPacket);
    // Logger::debug("sent skin packet");
    // }
}

void Clone::onKey(KeyEvent &event) {
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (this->isEnabled() && this->isKeyPartOfKeybind(event.key) && this->isKeybind(event.keys) && (
            SDK::getCurrentScreen() == "hud_screen" ||
            SDK::getCurrentScreen() == "zoom_screen" ||
            SDK::getCurrentScreen() == "f3_screen"
        )) {
        clone = true;
        // clone();
    }
}

void Clone::onMouse(MouseEvent &event) {
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press && this->isEnabled() && (
            SDK::getCurrentScreen() == "hud_screen" ||
            SDK::getCurrentScreen() == "zoom_screen" ||
            SDK::getCurrentScreen() == "f3_screen"
        )) {
        clone = true;
        // clone();
    }
}
