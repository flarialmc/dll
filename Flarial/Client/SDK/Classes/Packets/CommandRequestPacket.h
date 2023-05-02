#include "../Packet.h"

class CommandRequestPacket : public Packet {
public:
    std::string command;
private:
    char pad_0050[0x42];
public:
    CommandRequestPacket(std::string command) {
        uintptr_t sig = Mem::findSig(Signatures::CommandRequestPacket);
        this->setVTable<CommandRequestPacket>(sig);

        this->command = command;
    }
};