#include "utils/config.hpp"

#include <switch.h>
#include <filesystem>
#include <fstream>
#include <vector>

bool cp(char *filein, const char *fileout) {
    FILE *exein, *exeout;
    exein = fopen(filein, "rb");
    if (exein == NULL) {
        /* handle error */
        perror("file open for reading");
        return false;
    }
    exeout = fopen(fileout, "wb");
    if (exeout == NULL) {
        /* handle error */
        perror("file open for writing");
        return false;
    }
    size_t n, m;
    unsigned char buff[8192];
    do {
        n = fread(buff, 1, sizeof buff, exein);
        if (n) m = fwrite(buff, 1, n, exeout);
        else   m = 0;
    }
    while ((n > 0) && (n == m));
    if (m) {
        perror("copy");
        return false;
    }
    if (fclose(exeout)) {
        perror("close output file");
        return false;
    }
    if (fclose(exein)) {
        perror("close input file");
        return false;
    }
    return true;
} 

namespace cfg {
    Config::Config() {
        this->loadConfig();
        this->parseConfig();
    }

    void Config::loadConfig() {
        if(!std::filesystem::exists("sdmc:/config/SimpleModDownloader/settings.json")) {
            chdir("sdmc:/");
            std::filesystem::create_directories("sdmc:/config/SimpleModDownloader/");
            cp("romfs:/json/settings.json","sdmc:/config/SimpleModDownloader/settings.json");
        }

        std::ifstream file("sdmc:/config/SimpleModDownloader/settings.json"); //No is open because it is created if it doesn't exist
        config = nlohmann::json::parse(file);
        file.close();
    }

    void Config::parseConfig() {
        if(config.contains("language")) {
            app_language = config["language"];
        } else {
            app_language = "en-US";
        }

    }

    std::string Config::getAppLanguage() {
        return app_language;
    }

    void Config::setAppLanguage(const std::string& app_language) {
        this->app_language = app_language;
    }

    void Config::saveConfig() {
        this->config["language"] = app_language;

        std::ofstream file("sdmc:/config/SimpleModDownloader/settings.json");
        file << this->config.dump(4);
        file.close();
    }
}