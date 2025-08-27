#include "APIUtils.hpp"

#include <wininet.h>
#include <Utils/Utils.hpp>
#include <Utils/Logger/Logger.hpp>

#include <miniz/miniz.h>
#include <curl/curl/curl.h>
#include <curl/curl/easy.h>

#include "SDK/SDK.hpp"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::pair<long, std::string> APIUtils::Request(
    const std::string& url,
    const std::string& method,
    const std::string& data,
    struct curl_slist* headers
) {
    long responseCode = 0;
    std::string responseBody;

    static bool curlInitialized = false;
    if (!curlInitialized) {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
            LOG_ERROR("failed curl_global_init");
            return { 0, "" };
        }
        curlInitialized = true;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("failed curl_easy_init");
        return { 0, "" };
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Samsung Smart Fridge");
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    // Ensure headers are initialized
    if (!headers) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    if (method == "GET") {
        // No need for CURLOPT_CUSTOMREQUEST, GET is default
    }
    else if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    }
    else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    }
    else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else {
        LOG_ERROR("invalid http method: {}", method);
        curl_easy_cleanup(curl);
        return { 0, "" };
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        LOG_ERROR("failed curl request: {}", curl_easy_strerror(res));
        responseBody = "[ERROR] cURL failed";
    }
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    }

    // Free headers only if they were newly created inside this function
    if (headers) {
        curl_slist_free_all(headers);
    }

    curl_easy_cleanup(curl);
    return { responseCode, responseBody };
}

std::pair<long, std::string> APIUtils::POST_Simple(const std::string& url, const std::string& postData) {
    long responseCode = 0;
    std::string responseBody;

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        LOG_ERROR("curl_global_init failed in POST_SIMPLE");
        return {0, ""};
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("curl_easy_init failed in POST_SIMPLE");
        curl_global_cleanup();
        return {0, ""};
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Samsung Smart Fridge");

    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        LOG_ERROR("curl_easy_perform failed in POST_SIMPLE: {}", curl_easy_strerror(res));
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return {responseCode, responseBody};
}

std::string APIUtils::legacyGet(const std::string &URL) {
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

     std::string APIUtils::get(const std::string &link) {
        try {
            HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (!interwebs) {
                return "";
            }

            DWORD http_decoding = TRUE;
            if (InternetSetOption(interwebs, INTERNET_OPTION_HTTP_DECODING, &http_decoding, sizeof(http_decoding)) == FALSE) {
                InternetCloseHandle(interwebs);
                LOG_ERROR("InternetSetOption(INTERNET_OPTION_HTTP_DECODING) failed");
                return "";
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


                char encodingBuffer[256];
                DWORD encodingBufferSize = sizeof(encodingBuffer);
                bool isGzipEncoded = false;
                if (HttpQueryInfoA(urlFile, HTTP_QUERY_CONTENT_ENCODING, &encodingBuffer, &encodingBufferSize, NULL)) {
                    std::string contentEncoding(encodingBuffer, encodingBufferSize > 0 ? encodingBufferSize -1 : 0);
                    if (String::replaceAll(contentEncoding, " ", "") == "gzip") {
                        isGzipEncoded = true;
                    }
                } else {
                    // hi
                    DWORD lastError = GetLastError();
                    //LOG_ERROR("HttpQueryInfoA(HTTP_QUERY_CONTENT_ENCODING) failed, assuming no gzip or plain text. LastError: " + FlarialGUI::cached_to_string(lastError));
                }


                InternetCloseHandle(urlFile);

                std::string compressedString = compressedData.str();


                if (isGzipEncoded && !compressedString.empty()) {
                    // Decompress using miniz

                    size_t uncompressedSizeGuess = compressedString.length() * 5;
                    std::vector<unsigned char> uncompressedBuffer(uncompressedSizeGuess);
                    mz_ulong finalUncompressedSize = static_cast<mz_ulong>(uncompressedSizeGuess);

                    int status = mz_uncompress(uncompressedBuffer.data(), &finalUncompressedSize, (const unsigned char*)compressedString.data(), compressedString.length());
                    if (status == Z_OK) {
                        rtn = std::string(reinterpret_cast<const char*>(uncompressedBuffer.data()), finalUncompressedSize);
                    } else {
                        InternetCloseHandle(interwebs);
                        LOG_ERROR("mz_uncompress failed with status: " + FlarialGUI::cached_to_string(status));
                        return ""; // Decompression failed
                    }
                } else {
                    rtn = compressedString;
                }


            } else {
                InternetCloseHandle(interwebs);
                return "";
            }

            InternetCloseHandle(interwebs);
            return String::replaceAll(rtn, "|n", "\r\n");
        } catch (const std::exception &e) {
            LOG_ERROR(e.what());
        }
        return "";
    }

nlohmann::json APIUtils::getVips() {
    try {
        std::string users = get("https://api.flarial.xyz/vips");

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
        LOG_ERROR("An error occurred while parsing vip users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        LOG_ERROR("An unexpected error occurred: {}", e.what());
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
        LOG_ERROR("An error occurred while parsing online users: {}", e.what());
        return nlohmann::json::object();
    }
    catch (const std::exception& e) {
        LOG_ERROR("An unexpected error occurred: {}", e.what());
        return nlohmann::json::object();
    }
}

bool APIUtils::hasRole(std::string_view role, std::string_view name) {
    const auto vipIt = vipUserToRole.find(name);
    const auto isVip = (vipIt != vipUserToRole.cend()) && (vipIt->second == role);

    if (isVip) {
        return true;
    }

    const auto isOnline = onlineUsersSet.contains(name);
    return isOnline && (role == "Regular");
}

std::vector<std::string> APIUtils::ListToVector(const std::string& commandListStr) {
    std::vector<std::string> commands;
    std::stringstream ss(commandListStr);
    char delimiter = ',';
    std::string segment;

    std::string trimmedStr = commandListStr;
    std::string prefix = "{\"players\": [";
    std::string suffix = "]}";

    if (trimmedStr.rfind(prefix, 0) == 0) {
        trimmedStr.erase(0, prefix.length());
    }

    if (trimmedStr.rfind(suffix) == trimmedStr.length() - suffix.length()) {
        trimmedStr.erase(trimmedStr.length() - suffix.length());
    }

    std::stringstream trimmed_ss(trimmedStr);


    while (std::getline(trimmed_ss, segment, delimiter)) {
        std::string trimmedSegment = segment;

        trimmedSegment.erase(trimmedSegment.begin(), std::find_if(trimmedSegment.begin(), trimmedSegment.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        trimmedSegment.erase(std::find_if(trimmedSegment.rbegin(), trimmedSegment.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), trimmedSegment.end());


        if (trimmedSegment.length() >= 2 && trimmedSegment.front() == '"' && trimmedSegment.back() == '"') {
            trimmedSegment = trimmedSegment.substr(1, trimmedSegment.length() - 2);
        }

        if (!trimmedSegment.empty()) {
            commands.push_back(trimmedSegment);
        }
    }
    return commands;
}


std::string APIUtils::VectorToList(const std::vector<std::string>& vec) {
    std::stringstream ss;
    ss << "{\"players\": [";

    for (size_t i = 0; i < vec.size(); ++i) {
        ss << std::quoted(vec[i]);
        if (i < vec.size() - 1) {
            ss << ',';
        }
    }

    ss << "]}";
    return ss.str();
}


std::vector<std::string> APIUtils::UpdateVector(
    const std::vector<std::string>& currentVec,
    const std::vector<std::string>& commands) {

    std::vector<std::string> result = currentVec;

    for (const auto& command : commands) {
        if (command.empty()) {
            continue;
        }

        char prefix = command[0];
        std::string item = command.substr(1);

        if (prefix == '+') {
            if (std::find(result.begin(), result.end(), item) == result.end()) {
                result.push_back(item);
            }
        }
        else if (prefix == '-') {
            auto it = std::find(result.begin(), result.end(), item);
            if (it != result.end()) {
                result.erase(it);
            }
        }
    }

    if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {
        try {
            std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();
            std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));

            if (clearedName.empty()) {
                clearedName = String::removeColorCodes(name);
            }

            if (std::find(result.begin(), result.end(), clearedName) == result.end()) {
                result.push_back(clearedName);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error processing local player name: {}", e.what());
        }
    }

    return result;
}

std::vector<std::string> APIUtils::UpdateVector(
    const std::vector<std::string>& currentVec,
    const std::string& commandListStr) {

    std::vector<std::string> commands = ListToVector(commandListStr);
    return UpdateVector(currentVec, commands);
}

std::vector<std::string> APIUtils::UpdateVectorFast(
    const std::vector<std::string>& currentVec,
    const std::vector<std::string>& commands) {

    std::unordered_set<std::string> itemSet(currentVec.begin(), currentVec.end());

    for (const auto& command : commands) {
        if (command.empty()) {
            continue;
        }

        char prefix = command[0];
        std::string item = command.substr(1);

        if (prefix == '+') {
            itemSet.insert(item);
        }
        else if (prefix == '-') {
            itemSet.erase(item);
        }
    }

    return std::vector<std::string>(itemSet.begin(), itemSet.end());
}