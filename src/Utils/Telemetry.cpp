#include "Telemetry.hpp"
#include <json/json.hpp>
#include "APIUtils.hpp"
#include "Utils.hpp"
#include "Logger/Logger.hpp"
#include <windows.h>
#include <wincrypt.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <vector>
#include <cstdint>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")

std::string Telemetry::s_userHash;
std::once_flag Telemetry::s_initFlag;

void Telemetry::sendModuleEvent(const std::string& moduleName, const std::string& action) {
    try {
        std::call_once(s_initFlag, []() {
            s_userHash = generateUserHash();
        });

        nlohmann::json payload = {
            {"userHash", s_userHash},
            {"moduleName", moduleName},
            {"action", action},
            {"version", getClientVersion()}
        };

        sendTelemetryAsync(payload);
    } catch (const std::exception& e) {
        Logger::warn("Telemetry error: {}", e.what());
    }
}

std::string Telemetry::generateUserHash() {
    try {
        std::string systemInfo = getSystemIdentifier();
        return sha256(systemInfo);
    } catch (const std::exception& e) {
        Logger::warn("Failed to generate user hash: {}", e.what());
        return sha256("fallback_" + std::to_string(GetTickCount64()));
    }
}

std::string Telemetry::getSystemIdentifier() {
    std::string identifier;
    
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameLength = sizeof(computerName);
    if (GetComputerNameA(computerName, &computerNameLength)) {
        identifier += computerName;
    }

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                      "SOFTWARE\\Microsoft\\Cryptography", 
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char machineGuid[256];
        DWORD dataSize = sizeof(machineGuid);
        if (RegQueryValueExA(hKey, "MachineGuid", nullptr, nullptr, 
                            (LPBYTE)machineGuid, &dataSize) == ERROR_SUCCESS) {
            identifier += machineGuid;
        }
        RegCloseKey(hKey);
    }

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    identifier += std::to_string(systemInfo.dwProcessorType);
    identifier += std::to_string(systemInfo.dwNumberOfProcessors);

    if (identifier.empty()) {
        identifier = "unknown_system_" + std::to_string(GetTickCount64());
    }

    return identifier;
}

std::string Telemetry::sha256(const std::string& input) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    std::string result;
    
    try {
        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            throw std::runtime_error("CryptAcquireContext failed");
        }
        
        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            throw std::runtime_error("CryptCreateHash failed");
        }
        
        if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(input.c_str()),
                          static_cast<DWORD>(input.length()), 0)) {
            throw std::runtime_error("CryptHashData failed");
        }
        
        DWORD hashSize = 0;
        DWORD hashSizeSize = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), 
                              &hashSizeSize, 0)) {
            throw std::runtime_error("CryptGetHashParam failed");
        }
        
        std::vector<BYTE> hashData(hashSize);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, hashData.data(), &hashSize, 0)) {
            throw std::runtime_error("CryptGetHashParam failed");
        }
        
        std::stringstream ss;
        for (DWORD i = 0; i < hashSize; ++i) {
            ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(hashData[i]);
        }
        result = ss.str();
        
    } catch (const std::exception& e) {
        Logger::warn("SHA-256 hash failed: {}, using fallback", e.what());
        std::hash<std::string> hasher;
        std::stringstream fallback;
        fallback << std::hex << hasher(input);
        result = fallback.str();
    }
    
    if (hHash) CryptDestroyHash(hHash);
    if (hProv) CryptReleaseContext(hProv, 0);
    
    return result;
}

std::string Telemetry::getClientVersion() {
    return "2.5.0";
}

void Telemetry::sendTelemetryAsync(const nlohmann::json& payload) {
    std::thread([payload]() {
        try {
            std::string jsonData = payload.dump();
            auto result = APIUtils::POST_Simple("https://api.flarial.xyz/telemetry/module-events", jsonData);
            
            if (result.first < 200 || result.first >= 300) {
                Logger::warn("Telemetry failed: HTTP {}", result.first);
            }
        } catch (const std::exception& e) {
            Logger::debug("Telemetry send failed: {}", e.what());
        }
    }).detach();
}