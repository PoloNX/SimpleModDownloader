#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace cfg {
    class Config {
        public:
            Config();
            ~Config() {
                this->saveConfig();
            }

            void saveConfig();

            std::string getAppLanguage();
            void setAppLanguage(const std::string& app_language);


        private:

            std::string app_language;

            void loadConfig();
            void parseConfig();

            nlohmann::json config;
    };
}