#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <source_location>
#include <vector>

class SymbolMapper {
public:
    struct SymbolEntry {
        std::string functionName;
        std::string fileName;
        int lineNumber;
        void* address;
    };

    struct CallContext {
        std::string functionName;
        std::string fileName;
        int lineNumber;
        std::string extra;
    };

    static SymbolMapper& getInstance() {
        static SymbolMapper instance;
        return instance;
    }

    // Register a function entry (called at function start)
    void registerFunction(void* address, const std::string& name, const std::string& file, int line);

    // Track current execution context
    void pushContext(const std::string& function, const std::string& file, int line, const std::string& extra = "");
    void popContext();

    // Get symbol info for an address
    SymbolEntry* findSymbol(void* address);

    // Get current call stack context
    std::vector<CallContext> getCurrentContext();

    // Generate a lightweight symbol map
    void exportSymbolMap(const std::string& filepath);

    // Get crash context information
    std::string getCrashContext();

private:
    SymbolMapper() = default;
    std::unordered_map<void*, SymbolEntry> symbolMap;
    thread_local std::vector<CallContext> contextStack;
    std::mutex mapMutex;
};

// Macro for auto-registering functions
#define TRACK_FUNCTION() \
    static bool _registered_##__LINE__ = []() { \
        SymbolMapper::getInstance().registerFunction( \
            reinterpret_cast<void*>(&TRACK_FUNCTION), \
            __FUNCTION__, __FILE__, __LINE__); \
        return true; \
    }(); \
    SymbolMapper::ContextGuard _guard(__FUNCTION__, __FILE__, __LINE__)

// Context guard for RAII-based context tracking
class ContextGuard {
public:
    ContextGuard(const std::string& func, const std::string& file, int line, const std::string& extra = "") {
        SymbolMapper::getInstance().pushContext(func, file, line, extra);
    }
    ~ContextGuard() {
        SymbolMapper::getInstance().popContext();
    }
};

// Macros for critical sections
#define CRASH_CONTEXT(description) \
    SymbolMapper::ContextGuard _ctx_guard(__FUNCTION__, __FILE__, __LINE__, description)

#define CRASH_ZONE(name) \
    SymbolMapper::ContextGuard _zone_guard(name, __FILE__, __LINE__)