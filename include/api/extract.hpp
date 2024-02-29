#pragma once

#include <string> 

namespace extract {
    bool extractEntry(const std::string& path, const std::string& outputDir, const std::string& tid);
}