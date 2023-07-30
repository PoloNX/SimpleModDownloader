#include "download.hpp"
#include <switch.h>
#include <chrono>
#include "progress_event.hpp"
#include <thread>
#include <fstream>
#include <filesystem>
#include <vector>

#define _1MiB 0x100000


namespace net {

    std::chrono::_V2::steady_clock::time_point time_old;
    double dlold;

    struct MemoryStruct_t
    {
        char* memory;
        size_t size;
    };

    struct ntwrk_struct_t
    {
        u_int8_t* data;
        size_t data_size;
        u_int64_t offset;
        std::ofstream* out;
        Aes128CtrContext* aes;
    };

    size_t WriteCallback(void* content, size_t size, size_t nmemb, std::string* response) {
        size_t totalSize = size * nmemb;
        response->append((char*)content, totalSize);
        return totalSize;
    }

    int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
        brls::Logger::info("Downloaded {} of {} bytes", dlnow, dltotal);

        std::string progress = fmt::format("Downloaded {} of {} bytes", dlnow, dltotal);

        svcOutputDebugString(progress.c_str(), progress.size());
        return 0;
    }

    int download_progress(void* p, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        if (dltotal <= 0.0) return 0;

        double fractionDownloaded = dlnow / dltotal;
        int counter = (int)(fractionDownloaded * ProgressEvent::instance().getMax());  //20 is the number of increments
        ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
        ProgressEvent::instance().setNow(dlnow);
        ProgressEvent::instance().setTotalCount(dltotal);
        auto time_now = std::chrono::steady_clock::now();
        double elasped_time = ((std::chrono::duration<double>)(time_now - time_old)).count();
        if (elasped_time > 1.2f) {
            ProgressEvent::instance().setSpeed((dlnow - dlold) / elasped_time);
            dlold = dlnow;
            time_old = time_now;
        }
        return 0;
    }

    
    size_t WriteCallbackImages(char* ptr, size_t size, size_t nmemb, void* userdata)
    {
        std::vector<unsigned char>* buffer = static_cast<std::vector<unsigned char>*>(userdata);
        size_t total_size = size * nmemb;
        buffer->insert(buffer->end(), ptr, ptr + total_size);
        return total_size;
    }

    static size_t WriteMemoryCallback(void* contents, size_t size, size_t num_files, void* userp)
    {
        if (ProgressEvent::instance().getInterupt()) {
            return 0;
        }
        ntwrk_struct_t* data_struct = static_cast<ntwrk_struct_t*>(userp);
        size_t realsize = size * num_files;

        if (realsize + data_struct->offset >= data_struct->data_size) {
            data_struct->out->write(reinterpret_cast<const char*>(data_struct->data), data_struct->offset);
            data_struct->offset = 0;
        }

        if (data_struct->aes)
            aes128CtrCrypt(data_struct->aes, &data_struct->data[data_struct->offset], contents, realsize);
        else
            memcpy(&data_struct->data[data_struct->offset], contents, realsize);

        data_struct->offset += realsize;
        data_struct->data[data_struct->offset] = 0;
        return realsize;
    }

    nlohmann::json downloadRequest(const std::string& url) {
        curl_global_init(CURL_GLOBAL_ALL);

        auto curl = curl_easy_init();

        if (!curl) 
            brls::Logger::error("Error while initializing libcurl");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);


        std::string response;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleModDownloader");

        auto res = curl_easy_perform(curl);

        nlohmann::json jsonResponse;

        if (res != CURLE_OK) 
            brls::Logger::error("Error while performing curl request: {}", curl_easy_strerror(res));
        else 
            jsonResponse = nlohmann::json::parse(response);
        
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return jsonResponse;
    }

    void downloadImage(const std::string& url, std::vector<unsigned char>& buffer)
    {

        CURL* curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackImages);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);

            curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleModDownloader");
            curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 120000L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 10L); // Temps d'inactivité avant de fermer la connexion (en secondes)

            CURLcode result = curl_easy_perform(curl);
            if (result != CURLE_OK)
            {
                brls::Logger::error(curl_easy_strerror(result));
            }
            curl_easy_cleanup(curl);
        }
        return;
    }

    long downloadFile(const std::string& url, const std::string& output, int api)
    {
        std::vector<std::uint8_t> dummy;
        return downloadFile(url, dummy, output, api);
    }

    bool checkSize(CURL* curl, const std::string& url)
    {
        curl_off_t dl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleModDownloader");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_perform(curl);
        auto res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &dl);
        if (!res) {
            s64 freeStorage;
            if (R_SUCCEEDED(nsGetFreeSpaceSize(NcmStorageId_SdCard, &freeStorage)) && dl * 1.1 > freeStorage) {
                return false;
            }
        }
        return true;
    }

    std::string extractDirectory(const std::string& filePath)
    {
        size_t lastSlashPos = filePath.find_last_of("/\\");
        if (lastSlashPos != std::string::npos) {
            return filePath.substr(0, lastSlashPos + 1);
        }
        return "";
    }

    long downloadFile(const std::string& url, std::vector<std::uint8_t>& res, const std::string& output, int api) {
        brls::Logger::info("Downloading {} to {}", url, output);
        
        std::string output_dir = extractDirectory(output);

        std::filesystem::create_directory(output_dir);

        const char* out = output.c_str();
        CURL* curl = curl_easy_init();
        ntwrk_struct_t chunk = {0};
        long status_code;
        time_old = std::chrono::steady_clock::now();
        dlold = 0.0f;
        bool can_download = true;
        std::string real_url = url;

        if (curl) {
            std::ofstream file(output, std::ios::binary);
            if (file || *out == 0) {

                chunk.data = static_cast<u_int8_t*>(malloc(_1MiB));
                chunk.data_size = _1MiB;
                chunk.out = &file;

                if (*out != 0) {
                    can_download = checkSize(curl, url);
                }

                curl_easy_setopt(curl, CURLOPT_URL, real_url.c_str());
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleModDownloader");
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

                if (api == false) {
                    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
                }
                curl_easy_perform(curl);
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

                if (file && chunk.offset && can_download)
                    file.write(reinterpret_cast<const char*>(chunk.data), chunk.offset);

                curl_easy_cleanup(curl);
                ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
            }
        }

        chunk.out->close();

        if (!can_download) {
            brls::Application::crash("Not enough space on SD card");
            std::this_thread::sleep_for(std::chrono::microseconds(2000000));
            brls::Application::quit();
            res = {};
        }

        if (*out == 0) {
            res.assign(chunk.data, chunk.data + chunk.offset);
        }

        free(chunk.data);
        free(chunk.aes);

        return status_code;
    }

}