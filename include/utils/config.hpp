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

            bool getStrictSearch();
            void setStringSearch(bool strict);

            bool getWireframe() {
                return wireframe;
            }
            void setWireframe(bool wireframe) {
                this->wireframe = wireframe;
            }


        private:

            std::string app_language;
            bool is_strict;
            bool wireframe;

            void loadConfig();
            void parseConfig();

            nlohmann::json config;
    };
}