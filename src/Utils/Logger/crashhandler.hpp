#pragma once
#include <string>
#include <functional>

namespace CrashHandler {
    // Initialize crash handling with symbol resolution
    void initialize(const std::string& crashLogPath = "");

    // Set custom crash callback
    void setCustomCallback(std::function<void(const std::string&)> callback);

    // Clean up crash handler
    void shutdown();

    // Generate a crash report manually (for testing)
    std::string generateReport();
}