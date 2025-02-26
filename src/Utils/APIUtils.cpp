#include "APIUtils.hpp"

#include <wininet.h>
#include <Utils/Utils.hpp>
#include <Utils/Logger/Logger.hpp>

#include <miniz/miniz.h>

std::vector<std::string> APIUtils::onlineUsers;
std::map<std::string, std::string> APIUtils::onlineVips;



     std::string APIUtils::get(const std::string &link) {
        try {
            HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (!interwebs) {
                return "";
            }

            // Set option to request HTTP decoding (gzip, deflate etc.) - Let's try this first
            DWORD http_decoding = TRUE;
            if (InternetSetOption(interwebs, INTERNET_OPTION_HTTP_DECODING, &http_decoding, sizeof(http_decoding)) == FALSE) {
                InternetCloseHandle(interwebs);
                Logger::error("InternetSetOption(INTERNET_OPTION_HTTP_DECODING) failed");
                return ""; // Or handle error as needed
            }


            std::string rtn;
            HINTERNET urlFile = InternetOpenUrlA(interwebs, link.c_str(), "Accept-Encoding: gzip\r\nUser-Agent: Samsung Smart Fridge\r\nContent-Type: application/json\r\nshould-compress: 1\r\n", -1, INTERNET_FLAG_RELOAD, 0);
            if (urlFile) {
                char buffer[2000];
                DWORD bytesRead;
                std::stringstream compressedData;


                while (InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                    compressedData.write(buffer, bytesRead);
                }


                // **MOVED HttpQueryInfoA HERE, before closing urlFile**
                char encodingBuffer[256];
                DWORD encodingBufferSize = sizeof(encodingBuffer);
                bool isGzipEncoded = false; // Flag to track gzip encoding
                if (HttpQueryInfoA(urlFile, HTTP_QUERY_CONTENT_ENCODING, &encodingBuffer, &encodingBufferSize, NULL)) {
                    std::string contentEncoding(encodingBuffer, encodingBufferSize > 0 ? encodingBufferSize -1 : 0); //Ensure null termination if size > 0
                    if (String::replaceAll(contentEncoding, " ", "") == "gzip") { // Normalize and compare, remove spaces if any
                        isGzipEncoded = true; // Set the flag
                    }
                } else {
                    DWORD lastError = GetLastError();
                    Logger::error("HttpQueryInfoA(HTTP_QUERY_CONTENT_ENCODING) failed, assuming no gzip or plain text. LastError: " + std::to_string(lastError));
                    // If HttpQueryInfoA fails, we assume not gzip or handle as plain text.
                    // Importantly, do NOT consider it an error that prevents further processing if you want to be resilient to missing headers.
                }


                InternetCloseHandle(urlFile); // Close urlFile handle AFTER header check

                std::string compressedString = compressedData.str(); // Get compressed data after closing, just for clarity


                if (isGzipEncoded && !compressedString.empty()) { // Decompress only if gzip and data is not empty
                    // Decompress using miniz

                    size_t uncompressedSizeGuess = compressedString.length() * 5;
                    std::vector<unsigned char> uncompressedBuffer(uncompressedSizeGuess);
                    mz_ulong finalUncompressedSize = static_cast<mz_ulong>(uncompressedSizeGuess);

                    int status = mz_uncompress(uncompressedBuffer.data(), &finalUncompressedSize, (const unsigned char*)compressedString.data(), compressedString.length());
                    if (status == Z_OK) {
                        rtn = std::string(reinterpret_cast<const char*>(uncompressedBuffer.data()), finalUncompressedSize);
                    } else {
                        InternetCloseHandle(interwebs);
                        Logger::error("mz_uncompress failed with status: " + std::to_string(status));
                        return ""; // Decompression failed
                    }
                } else {
                    rtn = compressedString; // Return as is if not gzip or empty
                }


            } else {
                InternetCloseHandle(interwebs);
                return ""; // InternetOpenUrlA failed
            }

            InternetCloseHandle(interwebs);
            return String::replaceAll(rtn, "|n", "\r\n");
        } catch (const std::exception &e) {
            Logger::error(e.what());
        }
        return "";
    }

nlohmann::json APIUtils::getVips() {
    try {
        std::string users = get("https://api.flarial.xyz/vips");
        std::cout << users << std::endl;

        if (users.empty()) {
            Logger::warn("Unable to fetch vips, API is down or you are not connected to the internet.");
            return nlohmann::json::object();
        }

        if (nlohmann::json::accept(users)) {
            return nlohmann::json::parse(users);
        }

        Logger::warn("VIP JSON rejected: {}", users);
        return nlohmann::json::object();
    }
    catch (const nlohmann::json::parse_error& e) {
        Logger::error("An error occurred while parsing vip users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        Logger::error("An unexpected error occurred: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json APIUtils::getUsers() {
    try {
        std::string users = get("https://api.flarial.xyz/allOnlineUsers");

        if (users.empty()) {
            Logger::warn("Unable to fetch users, API is down or you are not connected to the internet.");
            return nlohmann::json::object();
        }

        if (nlohmann::json::accept(users)) {
            return nlohmann::json::parse(users);
        }

        Logger::warn("Users JSON rejected: {}", users);
        return nlohmann::json::object();
    }
    catch (const nlohmann::json::parse_error& e) {
        Logger::error("An error occurred while parsing online users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        Logger::error("An unexpected error occurred: {}", e.what());
        return nlohmann::json::object();
    }
}

bool APIUtils::hasRole(const std::string& role, const std::string& name) {
    try {
        auto it = onlineVips.find(name);
        bool onlineVip = it != onlineVips.end() && it->second == role;
        bool online = std::find(onlineUsers.begin(), onlineUsers.end(), name) != onlineUsers.end();
        return onlineVip || (!onlineVip && online && role == "Regular");
    } catch (const std::exception& e) {
        Logger::error("An error occurred while checking roles: {}", e.what());
        return false;
    }
}