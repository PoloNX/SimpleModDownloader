#pragma once

#include <vector>
#include <string>

namespace utils {
    std::vector<std::pair<std::string, std::string>> getInstalledGames();
    uint8_t* getIconFromTitleId(const std::string& titleId);
    std::string removeHtmlTags(const std::string& str);
    std::string getModInstallPath();
}