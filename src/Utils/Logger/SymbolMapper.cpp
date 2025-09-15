#include "SymbolMapper.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

thread_local std::vector<SymbolMapper::CallContext> SymbolMapper::contextStack;

void SymbolMapper::registerFunction(void* address, const std::string& name, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mapMutex);
    symbolMap[address] = {name, file, line, address};
}

void SymbolMapper::pushContext(const std::string& function, const std::string& file, int line, const std::string& extra) {
    // Extract just the filename from the full path
    std::filesystem::path filePath(file);
    std::string filename = filePath.filename().string();

    contextStack.push_back({function, filename, line, extra});
}

void SymbolMapper::popContext() {
    if (!contextStack.empty()) {
        contextStack.pop_back();
    }
}

SymbolMapper::SymbolEntry* SymbolMapper::findSymbol(void* address) {
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = symbolMap.find(address);
    if (it != symbolMap.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<SymbolMapper::CallContext> SymbolMapper::getCurrentContext() {
    return contextStack;
}

void SymbolMapper::exportSymbolMap(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mapMutex);
    std::ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& [addr, symbol] : symbolMap) {
            file << std::hex << reinterpret_cast<uintptr_t>(addr) << " "
                 << symbol.functionName << " "
                 << symbol.fileName << ":"
                 << symbol.lineNumber << "\n";
        }
        file.close();
    }
}

std::string SymbolMapper::getCrashContext() {
    std::stringstream ss;

    ss << "Execution Context at Crash:\n";
    ss << "============================================================\n";

    auto context = getCurrentContext();
    if (context.empty()) {
        ss << "(No context information available)\n";
    } else {
        int depth = 0;
        for (const auto& ctx : context) {
            // Indent based on depth
            for (int i = 0; i < depth; ++i) ss << "  ";

            ss << "-> " << ctx.functionName;
            ss << " at " << ctx.fileName << ":" << ctx.lineNumber;
            if (!ctx.extra.empty()) {
                ss << " [" << ctx.extra << "]";
            }
            ss << "\n";
            depth++;
        }
    }

    return ss.str();
}