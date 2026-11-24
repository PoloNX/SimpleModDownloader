#include "api/net.hpp"
#include "utils/progress_event.hpp"

#include <curl/curl.h>
#include <borealis.hpp>
#include <fstream>

namespace net {
    std::chrono::_V2::steady_clock::time_point time_old;
    double dlold;

    size_t WriteCallback(void* content, size_t size, size_t nmemb, std::string* buffer) {
        buffer->append((char*)content, size * nmemb);
        return size * nmemb;
    }

    nlohmann::json downloadRequest(std::string url) {
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
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

        auto res = curl_easy_perform(curl);

        nlohmann::json json;

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        if(res != CURLE_OK) {
            brls::Logger::error("Failed to perform request: " + std::string(curl_easy_strerror(res)));
        } else if(response.empty()) {
            // GameBanana now answers some removed routes (e.g. apiv11/File/{id}) with an empty 200
            brls::Logger::error("Empty response (HTTP {}) for {}", httpCode, url);
        } else {
            // Don't throw: an unexpected/invalid body used to crash the whole app
            json = nlohmann::json::parse(response, nullptr, false);
            if(json.is_discarded()) {
                brls::Logger::error("Invalid JSON (HTTP {}) for {}", httpCode, url);
                json = nlohmann::json();
            } else if(httpCode >= 400) {
                brls::Logger::error("HTTP {} for {}: {}", httpCode, url, response.substr(0, 200));
                json = nlohmann::json();
            }
        }

        curl_easy_cleanup(curl);

        return json;
    }

    size_t WriteCallbackImage(char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (ProgressEvent::instance().getInterupt()) {
            return 0;
        }
        std::vector<unsigned char>* buffer = static_cast<std::vector<unsigned char>*>(userdata);
        size_t total_size = size * nmemb;
        buffer->insert(buffer->end(), ptr, ptr + total_size);
        return total_size;
    }

    void downloadImage(const std::string& url, std::vector<unsigned char>& buffer) {
        auto curl = curl_easy_init();

        brls::Logger::debug("Downloading image: {}", url);
        if(!curl) {
            brls::Logger::error("Failed to initialize curl");
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackImage);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 120000L);

        auto res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            brls::Logger::error(curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }

    size_t WriteCallbackFile(void* ptr, size_t size, size_t nmemb, std::ofstream *stream) {
        stream->write(static_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
    }
    
    int downloadFileProgress(void* p, double dltotal, double dlnow, double ultotal, double ulnow) {
        //brls::Logger::debug("Downloaded: {} / {}", dlnow, dltotal);
        if(dltotal < 0.0) return 0;

        double progress = dlnow / dltotal;
        int counter = (int)(progress * ProgressEvent::instance().getMax());
        ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
        ProgressEvent::instance().setNow(dlnow);
        ProgressEvent::instance().setTotalCount(dltotal);
        auto time_now = std::chrono::steady_clock::now();
        double elapsed_time = ((std::chrono::duration<double>)(time_now - time_old)).count();
        if(elapsed_time > 1.2f) {
            ProgressEvent::instance().setSpeed((dlnow - dlold) / elapsed_time);
            dlold = dlnow;
            time_old = time_now;
        }

        return 0;
    }

    void downloadFile(const std::string& url, const std::string& path) {
        brls::Logger::debug("Downloading file: {}, in the location : {}", url, path);

        auto curl = curl_easy_init();

        if(!curl) {
            brls::Logger::error("Failed to initialize curl");
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, downloadFileProgress);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        std::ofstream ofs(path, std::ios::binary); 

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

        auto res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            brls::Logger::error(curl_easy_strerror(res));
            return; 
        }

        ofs.close();

        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            brls::Logger::error(curl_easy_strerror(res));
            std::filesystem::remove(path); 
        }
    }

}