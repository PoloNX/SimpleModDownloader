#include "api/game.hpp"
#include "api/net.hpp"
#include <curl/curl.h>

Game::Game(const std::string& m_title, const std::string& m_tid) {
    title = m_title;
    tid = m_tid;
    searchGame();
    parseJson();
}

void Game::searchGame() {
    auto curl = curl_easy_init();  
    std::string title_url;
    title_url = curl_easy_escape(curl, title.c_str(), title.length());
    curl_easy_cleanup(curl);


    json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Util/Game/NameMatch?_sName={}", title_url));

    if(json.empty()) {
        brls::Logger::error("Failed to search for game: " + title);
    }
}

void Game::parseJson() {
    if(json.empty()) {
        return;
    }

    if(json.at("_aRecords")[0].find("_sName") != json.at("_aRecords")[0].end()) {
        title = json.at("_aRecords")[0].at("_sName").get<std::string>();
    }

    if(json.at("_aRecords")[0].find("_idRow") != json.at("_aRecords")[0].end()) {
        gamebananaID = json.at("_aRecords")[0].at("_idRow").get<int>();
    }
}