#include "MessageLogger.hpp"


void MessageLogger::onEnable() {
    Listen(this, PacketEvent, &MessageLogger::onPacket)
    Listen(this, TickEvent, &MessageLogger::onTick)
    Module::onEnable();
}

void MessageLogger::onDisable() {
    Deafen(this, PacketEvent, &MessageLogger::onPacket)
    Deafen(this, TickEvent, &MessageLogger::onTick)
    Module::onDisable();
}

void MessageLogger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("prependTimestamp", true);
    setDef("saveCleanVersion", true);
}

void MessageLogger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addToggle("Prepend Timestamp", "[timestamp] {msg} format", "prependTimestamp");
    addToggle("Save Clean Version", "Removes color codes from texts", "saveCleanVersion");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}


void MessageLogger::onPacket(PacketEvent &event) {
    if (!this->isEnabled()) return;

    if (event.getPacket()->getId() == MinecraftPacketIds::Text) update = true;
}

void MessageLogger::onTick(TickEvent &event) {
    if (!this->isEnabled() || !update) return;
    update = false;

    auto& messages = SDK::clientInstance->getGuiData()->getGuiMessages();
    size_t currentSize = messages.size();

    if (currentSize == prevMsgVecSize || currentSize == 0) return;
    prevMsgVecSize = currentSize;

    // Get the message BEFORE any file operations to avoid race conditions
    std::string fullMsg = messages.back().fullMsg;
    if (fullMsg.empty()) return;

    std::string path = Utils::getClientPath() + "\\MessageLogger";

    auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm *local_tm = std::localtime(&time_t_now);

    std::stringstream ss;
    ss << std::put_time(local_tm, "%b-%d-%Y");
    std::string filename = ss.str();

    std::string curTxtFile = path + "\\" + filename + ".txt";
    std::string curTxtFile_clean = path + "\\" + filename + "_clean.txt";

    std::ofstream logFile(curTxtFile, std::ios::app);

    if (logFile.is_open()) {
        if (getOps<bool>("prependTimestamp")) {
            ss.str("");
            ss.clear();
            ss << std::put_time(local_tm, "%H:%M:%S"); // e.g., "10:23:13"
            logFile << "[" << ss.str() << "] " << fullMsg << std::endl;
        }
        else logFile << fullMsg << std::endl;

        logFile.close();
    } else Logger::error("Error opening message logger file {}", curTxtFile);

    if (getOps<bool>("saveCleanVersion")) {
        std::ofstream logFile_clean(curTxtFile_clean, std::ios::app);

        if (logFile_clean.is_open()) {
            if (getOps<bool>("prependTimestamp")) {
                ss.str("");
                ss.clear();
                ss << std::put_time(local_tm, "%H:%M:%S"); // e.g., "10:23:13"
                logFile_clean << "[" << ss.str() << "] " << String::removeColorCodes(fullMsg) << std::endl;
            }
            else logFile_clean << String::removeColorCodes(fullMsg) << std::endl;

            logFile_clean.close();
        } else Logger::error("Error opening message logger file_clean {}", curTxtFile_clean);
    }
}