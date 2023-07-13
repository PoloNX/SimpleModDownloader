#pragma once

#include <vector>
#include <string>
#include <borealis.hpp>
#include <nlohmann/json.hpp>

const std::string APP_VER = "1.0.3";
const std::string SimpleModManager_URL = "https://github.com/nadrino/SimpleModManager/releases/latest/download/SimpleModManager.nro";

const std::string SimpleModDownloader_API = "https://api.github.com/repos/PoloNX/SimpleModDownloader/releases/latest";
const std::string SimpleModDownloader_PATH = "sdmc:/config/SimpleModDownloader/app.nro";

const std::string FORWARDER_PATH = "sdmc:/config/SimpleModDownloader/forwarder.nro";

const std::vector<std::string> langVector ={"de", "en-US", "es", "fr", "gr", "it", "ja", "ko","pt-BR", "ro", "zh-CN", "auto"};

const std::vector<std::pair<std::string, std::string>> goodGamesName = {
    {"Pokémon Brillant Diamond", "Pokemon Brilliant Diamond and Shining Pearl"},
    {"Pokémon Shining Pearl", "Pokemon Brilliant Diamond and Shining Pearl"},
    {"Pokémon Sword", "Pokemon Sword  Shield"},
    {"Pokémon Shield", "Pokemon Sword  Shield"},
    {"Pokémon Scarlet", "Pokemon Scarlet  Violet"},
    {"Pokémon Violet", "Pokemon Scarlet  Violet"},
    {"Miitopia", "Miitopia (Switch)"},
    {"Super Meat Boy", "Super Meat Boy (Nintendo Switch)"},  
    {"UNDERTAILE", "UNDERTALE (Switch)"},
    {"Fairy Tail", "Fairy Tail (Switch)"},
    {"Sonic Frontiers", "Sonic Frontiers (Switch)"},
    {"Stardew Valley", "Stardew Valley (Switch)"},
    {"Dragon Ball FighterZ", "Dragon Ball FighterZ (Switch)"},
    {"The Legend of Zelda: Link's Awakening", "The Legend of Zelda: Link's Awakening (Switch)"},
    {"The Binding of Isaac: Afterbirth+", "The Binding of Isaac: Afterbirth+ (Switch)"},
    {"The Legend of Zelda: Breath of the Wild", "The Legend of Zelda: Breath of the Wild (Switch)"},
    {"LAYTON’S MYSTERY JOURNEY™: Katrielle and the Millionaires’ Conspiracy - Deluxe Edition", "LAYTON’S MYSTERY JOURNEY: Katrielle and the Millionaires’ Conspiracy - Deluxe Edition"}
};

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

