#include "Memory.hpp"

std::string MEM::getDisplayValue() {
    //TODO: (Memory module) Do megabytes mode
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatusEx(&memory_status);
    DWORDLONG total_memory = memory_status.ullTotalPhys;
    DWORDLONG free_memory = memory_status.ullAvailPhys;
    DWORDLONG used_memory = total_memory - free_memory;

    int sussymem = static_cast<int>((used_memory * 100) / total_memory);

    return FlarialGUI::cached_to_string(sussymem) + "%";
}
