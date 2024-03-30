#pragma once 

#include <string>
#include <borealis.hpp>
#include <nlohmann/json.hpp>

class Game {
    public:
        Game(const std::string& title, const std::string& tid);
        Game() {}
        
        std::string getTitle() { return title; }
        int getGamebananaID() { return gamebananaID; }
        std::string getTid() { return tid; }
        brls::Image* getIcon() { return icon; }
        std::string getBannerUrl() { return bannerURL; }
    private:
        void searchGame();
        void parseJson();

        nlohmann::json json;

        std::string title;
        std::string bannerURL;
        int gamebananaID = 0;
        std::string tid;
        brls::Image* icon;
};