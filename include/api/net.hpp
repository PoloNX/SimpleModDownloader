#pragma once

#include <nlohmann/json.hpp>

namespace net {
    nlohmann::json downloadRequest(std::string url);
    void downloadImage(const std::string& url, std::vector<unsigned char>& buffer);
    void downloadFile(const std::string& url, const std::string& path);
}