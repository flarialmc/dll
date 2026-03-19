#pragma once
#include "../Hook.hpp"
#include <cstdarg>

class BedrockLogHook : public Hook {

private:
    static int callback(void* logDetails, int area, unsigned int priority,
                        const char* function, int line, int messageId,
                        const char* pszFormat, va_list argptr);

public:
    typedef int(__fastcall *original)(void* logDetails, int area, unsigned int priority,
                                     const char* function, int line, int messageId,
                                     const char* pszFormat, va_list argptr);

    static inline original funcOriginal = nullptr;

    BedrockLogHook();

    void enableHook() override;
};
