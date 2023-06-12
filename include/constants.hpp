#pragma once

#include <vector>
#include <string>
#include <borealis.hpp>
#include <nlohmann/json.hpp>

const std::string APP_VER = "0.0.1";
const std::string SimpleModManager_URL = "https://github.com/nadrino/SimpleModManager/releases/latest/download/SimpleModManager.nro";

struct File {
    std::string name;
    std::string size;
    std::string url;
    std::string checkSum;
    std::string date;
};

struct Mod {
    std::string title;
    int ID;
    std::string desc;
    std::vector<File> files;
    nlohmann::json json;
    std::vector<brls::Image*> images;
    size_t sizeBigImage;
    int currentBigImage;
    std::string author;
};

struct Game {
    std::string title;
    int gamebananaID;
    std::string tid;
};

