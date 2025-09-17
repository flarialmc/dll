#pragma once

#include <random>
#include <filesystem>
#include <fstream>
#include <thread>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>

#include <fmt/format.h>
#include "miniz.h"
#include "../../../../Client.hpp"
#include "../../../../../Utils/Logger/Logger.hpp"
#include "curl/curl/curl.h"
#include <Scripting/ScriptManager.hpp>
#include "Client/GUI/Engine/EngineImgui.hpp"

class ScriptMarketplace : public Listener {

public:
    ScriptMarketplace() {
        Listen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
        Listen(this, ProtocolEvent, &ScriptMarketplace::onProtocolConfig);
    }

    ~ScriptMarketplace() {
        Deafen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
        Deafen(this, ProtocolEvent, &ScriptMarketplace::onProtocolConfig);
    }

    static size_t WriteStringCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string* response = static_cast<std::string*>(userp);
        size_t totalSize = size * nmemb;
        response->append(static_cast<const char*>(contents), totalSize);
        return totalSize;
    }

    static std::string base64_decode(const std::string& encoded) {
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string decoded;
        std::vector<int> T(256, -1);
        
        for (int i = 0; i < 64; i++) T[chars[i]] = i;

        int val = 0, valb = -8;
        for (unsigned char c : encoded) {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                decoded.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return decoded;
    }

    static std::string url_encode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
            std::string::value_type c = (*i);
            
            // Keep alphanumeric and other accepted characters intact
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char)c);
            escaped << std::nouppercase;
        }

        return escaped.str();
    }

    static std::string GetString(const std::string &URL) {
        try {
            CURL* curl = curl_easy_init();
            if (!curl) {
                LOG_ERROR("Failed to initialize cURL");
                return "";
            }

            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Flarial Client/1.0");

            CURLcode res = curl_easy_perform(curl);
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                LOG_ERROR("cURL error: {}", curl_easy_strerror(res));
                return "";
            }

            if (httpCode != 200) {
                LOG_ERROR("HTTP error: {}", httpCode);
                return "";
            }

            return response;
        } catch (const std::exception &e) {
            LOG_ERROR("Exception in GetString: {}", e.what());
        }
        return "";
    }

    void onProtocol(ProtocolEvent& event) {
        if (event.getPath() != std::wstring(L"flarial-scripting")) return;

        std::wstring scriptNameW, scriptTypeW;
        for (const auto& pair : event.getProtocolArgs()) {
            const auto& key = pair.first;
            const auto& value = pair.second;
            if (key == L"scriptName") scriptNameW = value;
            if (key == L"type") scriptTypeW = value;
        }

        if (scriptNameW.empty() || scriptTypeW.empty()) {
            LOG_ERROR("Missing scriptName or type in protocol event");
            return;
        }

        std::string scriptName = String::WStrToStr(scriptNameW);
        std::string scriptType = String::WStrToStr(scriptTypeW);
        FlarialGUI::Notify("Importing script '" + scriptName + "'... this may take a while.");
        std::thread([this, scriptName, scriptType]() {
            // First, get the script index to find the correct path
            std::string indexUrl = fmt::format("https://cdn.statically.io/gh/flarialmc/scripts/main/{}-index.json", scriptType);
            Logger::info("Fetching script index from: {}", indexUrl);
            
            std::string indexData = GetString(indexUrl);
            if (indexData.empty()) {
                LOG_ERROR("Failed to download script index for type {}", scriptType);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Could not access script index");
                return;
            }

            // Parse the JSON to find the script path
            // Look for the script with matching name
            std::string searchPattern = fmt::format("\"name\": \"{}\"", scriptName);
            size_t namePos = indexData.find(searchPattern);
            if (namePos == std::string::npos) {
                LOG_ERROR("Script '{}' not found in {} index", scriptName, scriptType);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Script not found in index");
                return;
            }

            // Find the path field for this script
            size_t pathStart = indexData.find("\"path\": \"", namePos);
            if (pathStart == std::string::npos) {
                LOG_ERROR("Path field not found for script '{}'", scriptName);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Invalid script index format");
                return;
            }
            
            pathStart += 9; // Skip "path": "
            size_t pathEnd = indexData.find("\"", pathStart);
            if (pathEnd == std::string::npos) {
                LOG_ERROR("Path field malformed for script '{}'", scriptName);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Invalid script path format");
                return;
            }

            std::string scriptPath = indexData.substr(pathStart, pathEnd - pathStart);
            
            // Now download the script using the correct path
            std::string url = fmt::format("https://cdn.statically.io/gh/flarialmc/scripts/main/{}", scriptPath);
            Logger::info("Downloading script '{}' from URL: {}", scriptName, url);
            
            std::string data = GetString(url);

            if (data.empty()) {
                LOG_ERROR("Failed to download script content for {} from URL: {}", scriptName, url);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': No content received");
                return;
            }

            if (data.length() < 10) {
                LOG_ERROR("Downloaded script content too short for {}: {} characters", scriptName, data.length());
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Invalid content");
                return;
            }

            // Basic validation for Lua content
            if (data.find("function") == std::string::npos && data.find("local") == std::string::npos && data.find("--") == std::string::npos) {
                LOG_ERROR("Downloaded content doesn't appear to be valid Lua script for {}", scriptName);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Invalid Lua content");
                return;
            }

            std::string destinationFolder;
            if (scriptType == "module") {
                destinationFolder = "\\Scripts\\Modules\\";
            } else if (scriptType == "command") {
                destinationFolder = "\\Scripts\\Commands\\";
            } else {
                LOG_ERROR("Invalid script type: {}", scriptType);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "'");
                return;
            }

            std::filesystem::path dirPath = std::filesystem::path(Utils::getClientPath()).string() + destinationFolder;
            std::error_code ec;
            std::filesystem::create_directories(dirPath, ec);

            if (ec) {
                LOG_ERROR("Failed to create directory {}: {}", dirPath.string(), ec.message());
                FlarialGUI::Notify("Failed to import script '" + scriptName + "'");
                return;
            }

            // Check if script already exists
            std::string filePath = dirPath.string() + scriptName + ".lua";
            if (std::filesystem::exists(filePath)) {
                Logger::info("Overwriting existing script: {}", scriptName);
                FlarialGUI::Notify("Overwriting existing script '" + scriptName + "'");
            }

            // Write the script file
            std::ofstream file(filePath, std::ios::out | std::ios::trunc);

            if (!file.is_open()) {
                LOG_ERROR("Failed to open file for writing: {}", filePath);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': File creation failed");
                return;
            }

            file << data;
            file.close();

            if (file.fail()) {
                LOG_ERROR("Failed to write script content to file: {}", filePath);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "': Write failed");
                return;
            }

            Logger::info("Successfully imported {} script: {} to {}", scriptType, scriptName, filePath);
            FlarialGUI::Notify("Successfully imported script '" + scriptName + "'");
            ModuleManager::restartModules = true;
            ScriptManager::reloadScripts();
        }).detach();
    }

    // Ensure this callback is declared as static if it's inside a class.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::ofstream* ofs = static_cast<std::ofstream*>(userp);
        if (!ofs || !ofs->is_open()) {
            // If the stream is not valid or open, return 0 to signal an error.
            return 0;
        }
        size_t totalSize = size * nmemb;
        try {
            ofs->write(static_cast<const char*>(contents), totalSize);
        } catch (const std::ios_base::failure& e) {
            // Optionally log the error here.
            return 0;
        }
        return totalSize;
    }

    bool DownloadFile(const std::string& url, const std::string& localFilePath) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cout << "Failed to initialize libcurl." << std::endl;
            return false;
        }

        // Open the file for binary writing.
        std::ofstream ofs(localFilePath, std::ios::binary);
        if (!ofs.is_open()) {
            std::cout << "Failed to open file for writing: " << localFilePath << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
        // Optionally set exceptions on the stream so that failures throw
        ofs.exceptions(std::ios::failbit | std::ios::badbit);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ScriptMarketplace::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);
        // Optionally follow redirects if needed.
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "Error during download: " << curl_easy_strerror(res) << std::endl;
            ofs.close();
            curl_easy_cleanup(curl);
            return false;
        }

        ofs.close();
        curl_easy_cleanup(curl);
        return true;
    }



    bool hasFlarialExtension(mz_zip_archive& zip) {
        // Get the number of files in the archive
        mz_uint numFiles = mz_zip_reader_get_num_files(&zip);

        for (mz_uint i = 0; i < numFiles; ++i) {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip, i, &file_stat)) {
                std::cerr << "Failed to get file stat for index " << i << "\n";
                continue;
            }

            std::string filename(file_stat.m_filename);
            // Check if the filename ends with ".flarial"
            if (filename.size() >= 8 && filename.substr(filename.size() - 8) == ".flarial") {
                return true;
            }
        }

        return false;
    }


    void onProtocolConfig(ProtocolEvent& event) {
        if (event.getPath() != std::wstring(L"flarial-configs")) return;

        for (const auto &pair : event.getProtocolArgs()) {
            if (pair.first == std::wstring(L"configName")) {
                std::string id = String::WStrToStr(pair.second);
                FlarialGUI::Notify("Importing config ..." + id + " this may take a while.");
                std::thread([this, id]() {
                    Logger::info("config name {}", id);

                    // Convert to lowercase for GitHub CDN compatibility  
                    std::string configId = id;
                    std::transform(configId.begin(), configId.end(), configId.begin(), ::tolower);
                    
                    // Use direct GitHub CDN URL (marketplace API has CORS restrictions)
                    std::string url = fmt::format("https://cdn.statically.io/gh/flarialmc/configs/main/{}.zip", configId);
                    Logger::info("Downloading config from URL: {}", url);

                    std::filesystem::path tempZipPath = std::filesystem::temp_directory_path() / (id + ".zip");

                    // Download directly as binary since GitHub CDN serves raw files
                    if (!DownloadFile(url, tempZipPath.string())) {
                        LOG_ERROR("Failed to download config {} from URL: {}", id, url);
                        FlarialGUI::Notify("Failed to import config '" + id + "': Download failed");
                        return;
                    }

                    if (!std::filesystem::exists(tempZipPath)) {
                        LOG_ERROR("Temporary zip file does not exist after creation for config {}", id);
                        FlarialGUI::Notify("Failed to import config '" + id + "': Zip file not created");
                        return;
                    }

                    std::uintmax_t fileSize = std::filesystem::file_size(tempZipPath);
                    if (fileSize == 0) {
                        LOG_ERROR("Temporary zip file is empty for config {}", id);
                        FlarialGUI::Notify("Failed to import config '" + id + "': Empty zip file");
                        return;
                    }

                    Logger::info("Created temporary zip file for config {} with size: {} bytes", id, fileSize);

                    // Debug: Check the first few bytes of the file to see if it's actually a zip
                    std::ifstream debugFile(tempZipPath, std::ios::binary);
                    if (debugFile.is_open()) {
                        char header[4];
                        debugFile.read(header, 4);
                        Logger::info("File header bytes: 0x{:02X} 0x{:02X} 0x{:02X} 0x{:02X}", 
                                   (unsigned char)header[0], (unsigned char)header[1], 
                                   (unsigned char)header[2], (unsigned char)header[3]);
                        debugFile.close();
                        
                        // ZIP files should start with "PK" (0x50 0x4B)
                        if (header[0] != 0x50 || header[1] != 0x4B) {
                            LOG_ERROR("Downloaded file is not a valid ZIP archive for config {}", id);
                            
                            // Read a bit more to see what we actually got
                            std::ifstream contentFile(tempZipPath);
                            std::string content((std::istreambuf_iterator<char>(contentFile)), std::istreambuf_iterator<char>());
                            contentFile.close();
                            Logger::info("First 200 chars of downloaded content: {}", content.substr(0, 200));
                            
                            FlarialGUI::Notify("Failed to import config '" + id + "': Invalid file format");
                            std::error_code cleanup_ec;
                            std::filesystem::remove(tempZipPath, cleanup_ec);
                            return;
                        }
                    }

                    std::string configname = id;

                    std::filesystem::path extractionDir = std::filesystem::path(Utils::getConfigsPath()) / configname;
                    std::error_code ec;
                    std::filesystem::create_directories(extractionDir, ec);
                    if (ec) {
                        LOG_ERROR("Failed to create extraction directory {}: {}", extractionDir.string(), ec.message());
                        return;
                    }

                    mz_zip_archive zip_archive;
                    memset(&zip_archive, 0, sizeof(zip_archive));
                    if (!mz_zip_reader_init_file(&zip_archive, tempZipPath.string().c_str(), 0)) {
                        LOG_ERROR("Failed to initialize zip archive from file for config {}: File path: {}, Size: {} bytes", 
                                 configname, tempZipPath.string(), fileSize);
                        FlarialGUI::Notify("Failed to import config '" + id + "': Invalid zip format");
                        
                        // Clean up temp file
                        std::error_code cleanup_ec;
                        std::filesystem::remove(tempZipPath, cleanup_ec);
                        return;
                    }

                    bool isLegacy = hasFlarialExtension(zip_archive);
                    if (isLegacy)
                    {
                        Logger::info("IMPORTING LEGACY CONFIG {}", configname);
                        extractionDir = std::filesystem::path(Utils::getConfigsPath()) / "Legacy" / configname;
                    }


                    int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zip_archive));
                    for (int j = 0; j < fileCount; j++) {
                        mz_zip_archive_file_stat file_stat;
                        if (!mz_zip_reader_file_stat(&zip_archive, j, &file_stat)) {
                            LOG_ERROR("Failed to get file stat for file index {} in config {}", j, configname);
                            continue;
                        }

                        std::string filename = file_stat.m_filename;
                        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
                        std::filesystem::path destPath = extractionDir / file_stat.m_filename;

                        if (mz_zip_reader_is_file_a_directory(&zip_archive, j)) {
                            std::filesystem::create_directories(destPath, ec);
                            if (ec) {
                                LOG_ERROR("Failed to create directory {}: {}", destPath.string(), ec.message());
                            }
                        } else {
                            
                                if (filename == "main.json" || filename == "manifest.json" ||
                                    std::filesystem::path(filename).extension() == ".png") {
                                    continue;
                                }

                            std::filesystem::create_directories(destPath.parent_path(), ec);
                            if (ec) {
                                LOG_ERROR("Failed to create directory {}: {}", destPath.parent_path().string(), ec.message());
                                continue;
                            }

                            // Extract the file
                            if (!mz_zip_reader_extract_to_file(&zip_archive, j, destPath.string().c_str(), 0)) {
                                LOG_ERROR("Failed to extract file {} in config {}", file_stat.m_filename, configname);
                            }
                        }
                    }

                    mz_zip_reader_end(&zip_archive);

                    if (!std::filesystem::remove(tempZipPath, ec)) {
                        LOG_ERROR("Failed to delete temporary zip file {}: {}", tempZipPath.string(), ec.message());
                    }

                    Logger::success("Successfully extracted config zip for {} to {}", configname, extractionDir.string());
                    FlarialGUI::Notify("Successfully imported config: " + configname);
                    ModuleManager::restartModules = true;
                    reloadAllConfigs();
                }).detach();
            }
        }
    }

    static void reloadAllConfigs() {
        if (fs::exists(Client::legacyDir) && fs::is_directory(Client::legacyDir)) {
            Client::legacySettings.addSetting("currentConfig", (std::string)"default");
            Client::hasLegacySettings = true;
            Client::softLoadLegacy = true;
            Logger::custom(fg(fmt::color::dark_magenta), "Config", "Legacy dir already exists... aborting");
        }
        Client::availableConfigs.clear();
        Client::loadAvailableConfigs();
        ModuleManager::restartModules = true;
    }
};
