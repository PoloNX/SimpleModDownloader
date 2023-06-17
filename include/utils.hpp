#pragma once

#include <switch.h>
#include <borealis.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "constants.hpp"

namespace utils {
    nlohmann::json getInstalledGames();
    nlohmann::json searchGames(std::string gameTitle);
    std::string replaceSpacesWithPlus(const std::string& str);
    nlohmann::json getMods(const int& gameID, const int& page = 1);
    nlohmann::json getMods(const int& gameID, const std::string& search, const int& page = 1);
    nlohmann::json getDownloadLinks(const std::string& ModelName, const int& idRow);
    uint8_t* getIconFromTitleId(const std::string& titleId);
    std::vector<brls::Image*> getModsImages(const nlohmann::json& mod_json, const int& bigImage, size_t& sizeOfArray);
    int openWebBrowser(const std::string url);
    void showDialogBoxInfo(const std::string& text);
    std::string getDescription(const int& mod_id);
    std::string convertTimestampToString(int timestamp);
    std::string formatFileSize(int size);
    std::string getModInstallPath();
};