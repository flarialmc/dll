#pragma once
#include <Command/Command.hpp>
#include <Utils/Logger/CrashTest.hpp>

class CrashTestCommand : public Command {
public:
    CrashTestCommand() : Command("crashtest", "Triggers a test crash to verify crash logging", {"crash"}) {}

    void execute(const std::vector<std::string>& args) override {
        int crashType = 1; // Default to null pointer crash

        if (!args.empty()) {
            try {
                crashType = std::stoi(args[0]);
            } catch (...) {
                crashType = 1;
            }
        }

        std::string crashName;
        switch (crashType) {
            case 1: crashName = "Null Pointer Dereference"; break;
            case 2: crashName = "Stack Overflow"; break;
            case 3: crashName = "Divide by Zero"; break;
            case 4: crashName = "Illegal Instruction"; break;
            case 5: crashName = "Write to Read-Only Memory"; break;
            case 6: crashName = "Pure Virtual Call"; break;
            default: crashName = "Null Pointer Dereference"; crashType = 1; break;
        }

        addCommandMessage("Triggering crash: " + crashName);
        addCommandMessage("Check %APPDATA%\\Flarial\\logs\\ for crash log");

        // Small delay so messages can be displayed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        glaiel::crashlogs::test::trigger_test_crash(crashType);
    }
};
