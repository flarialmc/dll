#pragma once
#include <string>
#include <vector>
#include <mutex>

// Lightweight crash tracking without PDB files
namespace CrashTracker {

    // Track execution context for better crash reports
    struct ExecutionContext {
        std::string moduleName;
        std::string functionName;
        std::string description;
        int lineNumber;
    };

    // Initialize crash tracking
    void initialize();

    // Cleanup
    void shutdown();

    // Add context information that will be included in crash reports
    void pushContext(const std::string& module, const std::string& function, int line = 0, const std::string& desc = "");
    void popContext();

    // Set a persistent context value
    void setGlobalContext(const std::string& key, const std::string& value);

    // Get formatted crash information
    std::string generateCrashReport();

    // RAII context guard
    class ContextGuard {
    public:
        ContextGuard(const std::string& module, const std::string& function, int line = 0, const std::string& desc = "");
        ~ContextGuard();
    private:
        ContextGuard(const ContextGuard&) = delete;
        ContextGuard& operator=(const ContextGuard&) = delete;
    };
}

// Convenience macros for tracking execution context
#define CRASH_ZONE(description) \
    CrashTracker::ContextGuard _crash_guard(__FILE__, __FUNCTION__, __LINE__, description)

#define CRASH_ZONE_SIMPLE() \
    CrashTracker::ContextGuard _crash_guard(__FILE__, __FUNCTION__, __LINE__)

// Track module operations
#define MODULE_CRASH_ZONE(moduleName, operation) \
    CrashTracker::ContextGuard _module_guard(moduleName, operation, __LINE__)