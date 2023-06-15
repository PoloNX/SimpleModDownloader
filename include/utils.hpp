#pragma once

#include <borealis.hpp>
#include <nlohmann/json.hpp>

namespace utils {
    uint8_t* getIconFromTitleId(const std::string& titleId);
    nlohmann::json getInstalledGames();
}