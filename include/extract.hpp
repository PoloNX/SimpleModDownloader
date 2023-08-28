#pragma once

#include <string>

namespace extract {
    bool extractEntry(const std::string& zipFile, const std::string& outputDir, const std::string& tid);
    bool extractZip(const std::string& zipFile, const std::string& outputDir);
    bool extractRar(const std::string& rarFile, const std::string& outputDir);
    bool extract7z(const std::string& zip7File, const std::string& outputDir);
};