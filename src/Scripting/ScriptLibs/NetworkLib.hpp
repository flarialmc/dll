#pragma once

#include "ScriptLib.hpp"

#include <curl/curl/curl.h>

class NetworkLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("network")
                .addFunction("get", [](const std::string& url) -> std::string {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_perform(curl);
                        curl_easy_cleanup(curl);
                    }
                    return response;
                })
                .addFunction("post", [](const std::string& url, const std::string& data) -> std::string {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                        curl_easy_setopt(curl, CURLOPT_POST, 1L);
                        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_perform(curl);
                        curl_easy_cleanup(curl);
                    }
                    return response;
                })
            .endNamespace();
    }

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }
};