#pragma once

#include <random>
#include <winrt/impl/windows.storage.2.h>
#include <wininet.h>

#include "miniz.h"
#include "../../../../Client.hpp"
#include "curl/curl/curl.h"

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

    std::string scriptPath() {
        using namespace winrt::Windows::Storage;

        auto roamingFolder = Utils::getRoamingPath();
        std::filesystem::path scriptsDir = std::filesystem::path(roamingFolder) / "Flarial" / "scripts";

        return scriptsDir.string();
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
            Logger::error(e.what());
        }
        return "";
    }

    void onProtocol(ProtocolEvent event) {
        if (event.getPath() == std::wstring(L"flarial-scripting")) {
            for (const auto &pair: event.getProtocolArgs()) {
                if (pair.first == std::wstring(L"scriptName")) {
                    std::string id = String::WStrToStr(pair.second);
                    Logger::info("script id {}", id);
                    std::string url = "http://node2.sear.host:5019/api/scripts/" + id + "/download";
                    std::string data = GetString(url);
                    std::ofstream file(Utils::getClientPath() + "\\Scripts\\Modules\\tmpd.tmp", std::ios::binary);
                    Logger::info("data: {}", data.c_str());
                    file.write(data.c_str(), data.size());
                    file.close();

                    ModuleManager::restartModules = true;
                }
            }
        }
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

void onProtocolConfig(ProtocolEvent event) {
    if (event.getPath() == std::wstring(L"flarial-configs")) {
        for (const auto &pair : event.getProtocolArgs()) {
            if (pair.first == std::wstring(L"configName")) {

                    std::string id = String::WStrToStr(pair.second);
                    Logger::info("config name {}", id);

                    std::string url = "http://node2.sear.host:5019/api/configs/" + id + "/download";
                    url.erase(std::remove(url.begin(), url.end(), ' '), url.end());

                    std::filesystem::path tempZipPath = std::filesystem::temp_directory_path() / (id + ".zip");

                    if (!ScriptMarketplace::DownloadFile(url, tempZipPath.string())) {
                        Logger::error("Failed to download config {} to temporary location", id);
                        return;
                    }

                    std::string configname = id;
                    Client::createConfig(configname);

                    std::filesystem::path extractionDir = std::filesystem::path(Utils::getConfigsPath()) / configname;
                    std::error_code ec;
                    std::filesystem::create_directories(extractionDir, ec);
                    if (ec) {
                        Logger::error("Failed to create extraction directory {}: {}", extractionDir.string(), ec.message());
                        return;
                    }

                    mz_zip_archive zip_archive;
                    memset(&zip_archive, 0, sizeof(zip_archive));
                    if (!mz_zip_reader_init_file(&zip_archive, tempZipPath.string().c_str(), 0)) {
                        Logger::error("Failed to initialize zip archive from file for config {}", configname);
                        return;
                    }

                    int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zip_archive));
                    for (int j = 0; j < fileCount; j++) {
                        mz_zip_archive_file_stat file_stat;
                        if (!mz_zip_reader_file_stat(&zip_archive, j, &file_stat)) {
                            Logger::error("Failed to get file stat for file index {} in config {}", j, configname);
                            continue;
                        }
                        std::filesystem::path destPath = extractionDir / file_stat.m_filename;
                        if (mz_zip_reader_is_file_a_directory(&zip_archive, j)) {
                            std::filesystem::create_directories(destPath, ec);
                            if (ec) {
                                Logger::error("Failed to create directory {}: {}", destPath.string(), ec.message());
                            }
                        } else {
                            std::filesystem::create_directories(destPath.parent_path(), ec);
                            if (ec) {
                                Logger::error("Failed to create directory {}: {}", destPath.parent_path().string(), ec.message());
                                continue;
                            }
                            if (!mz_zip_reader_extract_to_file(&zip_archive, j, destPath.string().c_str(), 0)) {
                                Logger::error("Failed to extract file {} in config {}", file_stat.m_filename, configname);
                            }
                        }
                    }
                    mz_zip_reader_end(&zip_archive);

                    if (!std::filesystem::remove(tempZipPath, ec)) {
                        Logger::error("Failed to delete temporary zip file {}: {}", tempZipPath.string(), ec.message());
                    }

                    Logger::success("Extracted config zip for {} to {}", configname, extractionDir.string());
                    FlarialGUI::Notify("Imported Config as: " + configname);
                    ModuleManager::restartModules = true;
                    reloadAllConfigs();

            }
        }
    }
}


    static void reloadAllConfigs() {
        Client::availableConfigs.clear();
        Client::loadAvailableConfigs();
        ModuleManager::restartModules = true;
    }
};
