#pragma once

#include <curl/curl.h>
#include <borealis.hpp>
#include <nlohmann/json.hpp>
#include <string>

constexpr int ON = 1;
constexpr int OFF = 0;

namespace net {
    nlohmann::json downloadRequest(const std::string& url);
    long downloadFile(const std::string& url, std::vector<std::uint8_t>& res, const std::string& output = "", int api = OFF);
    long downloadFile(const std::string& url, const std::string& output = "", int api = OFF);
    void downloadImage(const std::string& url, std::vector<unsigned char>& buffer);
}