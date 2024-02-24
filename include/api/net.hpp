#pragma once

#include <nlohmann/json.hpp>

namespace net {
    nlohmann::json downloadRequest(std::string url);
}