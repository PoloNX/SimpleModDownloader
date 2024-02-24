#include "api/net.hpp"

#include <curl/curl.h>
#include <borealis.hpp>

namespace net {
    size_t WriteCallback(void* content, size_t size, size_t nmemb, std::string* buffer) {
        buffer->append((char*)content, size * nmemb);
        return size * nmemb;
    }

    nlohmann::json downloadRequest(std::string url) {
        curl_global_init(CURL_GLOBAL_ALL);
        auto curl = curl_easy_init();

        brls::Logger::debug("Requesting: " + url);
        if(!curl) {
            brls::Logger::error("Failed to initialize curl");
            return nlohmann::json();
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        std::string response;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleModDownloader");

        auto res = curl_easy_perform(curl);

        nlohmann::json json;

        if(res != CURLE_OK) {
            brls::Logger::error("Failed to perform request: " + std::string(curl_easy_strerror(res)));
        } else {
            json = nlohmann::json::parse(response);
        }

        curl_easy_cleanup(curl);


        return json;
    }
}