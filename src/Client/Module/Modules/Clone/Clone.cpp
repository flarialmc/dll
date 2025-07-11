#include "Clone.hpp"

#include <utility>

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"

std::vector<std::shared_ptr<Packet>> Clone::inFlightPackets;

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

    for (const auto val: SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap() | std::views::values) {
        if (String::removeNonAlphanumeric(String::removeColorCodes(String::toLower(val.name))) != targetName) continue;
        if (!isValidUtf8(val.playerSkin.mId)) {
            SDK::clientInstance->getGuiData()->displayClientMessage("§cUnable to clone skin");
            break;
        };

        std::shared_ptr<Packet> packet = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
        auto pSkinPacket = std::static_pointer_cast<PlayerSkinPacket>(packet);

        pSkinPacket->mUUID = val.UUID;
        pSkinPacket->mSkin = val.playerSkin;

        SDK::clientInstance->getPacketSender()->sendToServer(pSkinPacket.get());
        inFlightPackets.push_back(pSkinPacket);
        // don't remove anything from inFlightPackets vec because it will mess up stuff
        SDK::clientInstance->getGuiData()->displayClientMessage("§aSkin cloned");
        break;
    }

    clone = false;
}

void Clone::onKey(KeyEvent &event) {
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (this->isEnabled() && this->isKeyPartOfKeybind(event.key) && this->isKeybind(event.keys) && (
            SDK::getCurrentScreen() == "hud_screen" ||
            SDK::getCurrentScreen() == "zoom_screen" ||
            SDK::getCurrentScreen() == "f3_screen"
        )) {
        clone = true;
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
    }
}
