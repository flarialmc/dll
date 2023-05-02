#include "../Packet.h"

enum class TextPacketType {
    RAW = 0,
    CHAT = 1,
    TRANSLATION = 2,
    POPUP = 3,
    JUKEBOX_POPUP = 4,
    TIP = 5,
    SYSTEM = 6,
    WHISPER = 7,
    ANNOUNCEMENT = 8,
    JSON_WHISPER = 9,
    JSON = 10,
    JSON_ANNOUNCEMENT = 11
};

class TextPacket : public Packet {
public:
    TextPacketType type;
public:
    std::string author;
    std::string message;
private:
    unsigned char paddd[24];
public:
    bool translationNeeded = false;
private:
    unsigned char padddd[7];
public:
    std::string xboxUserId;
    std::string platformChatId;
public:
    TextPacket(void) {
        static uintptr_t sig = NULL;

        if (sig == NULL)
            sig = Mem::findSig(Signatures::TextPacket);

        this->setVTable<TextPacket>(sig);
    }

    TextPacket(std::string authorUsername, std::string chatMessage) {

        *this = TextPacket();

        /* if (PacketVTables::TextPacketSig == NULL) {
            PacketVTables::TextPacketSig = Mem::findSig(Signatures::TextPacket);
        }

        this->setVTable<TextPacket>(PacketVTables::TextPacketSig); */

        this->type = TextPacketType::CHAT;
        this->translationNeeded = false;

        this->author = authorUsername;
        this->message = chatMessage;

        this->xboxUserId = std::string("");
        this->platformChatId = std::string("");
    }
};