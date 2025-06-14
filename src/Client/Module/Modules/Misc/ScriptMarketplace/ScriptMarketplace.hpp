#pragma once

#include <random>
#include <wininet.h>

#include "miniz.h"
#include "../../../../Client.hpp"
#include "curl/curl/curl.h"
#include <Scripting/ScriptManager.hpp>

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

    static std::string GetString(const std::string &URL) {
        try {
            HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (!interwebs) {
                return "";
            }

            std::string rtn;
            HINTERNET urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
            if (urlFile) {
                char buffer[2000];
                DWORD bytesRead;
                while (InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                    rtn.append(buffer, bytesRead);
                }
                InternetCloseHandle(urlFile);
            }

            InternetCloseHandle(interwebs);
            return String::replaceAll(rtn, "|n", "\r\n");
        } catch (const std::exception &e) {
            LOG_ERROR(e.what());
        }
        return "";
    }

    void onProtocol(ProtocolEvent event) {
        if (event.getPath() != std::wstring(L"flarial-scripting")) return;

        std::wstring scriptNameW, scriptTypeW;
        for (const auto& [key, value] : event.getProtocolArgs()) {
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
            std::string url = fmt::format("https://1klcjc8um5aq.flarial.xyz/api/scripts/{}/{}/download", scriptType, scriptName);
            std::string data = GetString(url);

            if (data.empty()) {
                LOG_ERROR("Failed to download script content for {}", scriptName);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "'");
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

            // Write the script file
            std::string filePath = dirPath.string() + scriptName + ".lua";
            std::ofstream file(filePath, std::ios::binary);

            if (!file.is_open()) {
                LOG_ERROR("Failed to open file for writing: {}", filePath);
                FlarialGUI::Notify("Failed to import script '" + scriptName + "'");
                return;
            }

            file.write(data.c_str(), data.size());
            file.close();

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


    void onProtocolConfig(ProtocolEvent event) {
        if (event.getPath() != std::wstring(L"flarial-configs")) return;

        for (const auto &pair : event.getProtocolArgs()) {
            if (pair.first == std::wstring(L"configName")) {
                std::string id = String::WStrToStr(pair.second);
                FlarialGUI::Notify("Importing config ..." + id + " this may take a while.");
                std::thread([this, pair, id]() {
                    Logger::info("config name {}", id);

                    std::string url = "https://1klcjc8um5aq.flarial.xyz/api/configs/" + id + "/download";
                    url.erase(std::remove(url.begin(), url.end(), ' '), url.end());

                    std::filesystem::path tempZipPath = std::filesystem::temp_directory_path() / (id + ".zip");

                    if (!ScriptMarketplace::DownloadFile(url, tempZipPath.string())) {
                        LOG_ERROR("Failed to download config {} to temporary location", id);
                        return;
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
                        LOG_ERROR("Failed to initialize zip archive from file for config {}", configname);
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

                    Logger::success("Extracted config zip for {} to {}", configname, extractionDir.string());
                    FlarialGUI::Notify("Imported Config as: " + configname);
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
