#include "MessageLogger.hpp"

MessageLogger::MessageLogger(): Module("Message Logger", "Saves chat messages into a file.", IDR_MEMORY_PNG, "")
{}

void MessageLogger::onEnable()
{
    Listen(this, PacketEvent, &MessageLogger::onPacket)
    Module::onEnable();
}

void MessageLogger::onDisable()
{
    Deafen(this, PacketEvent, &MessageLogger::onPacket)
    Module::onDisable();
}

void MessageLogger::defaultConfig()
{
    Module::defaultConfig("core");

}

void MessageLogger::onPacket(PacketEvent &event) {
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();

    if (id == MinecraftPacketIds::Text && SDK::clientInstance->getGuiData()->getGuiMessages().size() != prevMsgVecSize) {
        prevMsgVecSize = SDK::clientInstance->getGuiData()->getGuiMessages().size();
        std::string path = Utils::getClientPath() + "\\MessageLogger";

        auto now = std::chrono::system_clock::now();
        std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&time_t_now);

        std::stringstream ss;
        ss << std::put_time(local_tm, "%b-%d-%Y") << ".txt";
        std::string filename = ss.str();

        for (char &c : filename) {
            c = std::tolower(c);
        }

        std::string curTxtFile = path + "\\" + filename;

        // auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());

        std::ofstream logFile(curTxtFile, std::ios::app);

        if (logFile.is_open()) {
            ss.str("");
            ss.clear();
            ss << std::put_time(local_tm, "%H:%M:%S"); // e.g., "10:23:13"
            logFile << "[" << ss.str() << "] " << SDK::clientInstance->getGuiData()->getGuiMessages().back().fullMsg << std::endl;
            logFile.close();
        } else Logger::error("Error opening message logger file {}", curTxtFile);
    }
}