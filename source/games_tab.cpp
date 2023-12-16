#include "games_tab.hpp"
#include "main_frame.hpp"
#include <string>
#include "utils.hpp"
#include "mods_list.hpp"
#include <borealis.hpp>

namespace i18n = brls::i18n;
using namespace i18n::literals;

DownloadTab::DownloadTab() : brls::List() {
    //Create a json with your games
    nlohmann::json games = utils::getInstalledGames();
    if (!games.empty()) {
        for (auto i : games) {
            //Fill with importants informations
            std::string title = i.at("name");
            std::string tid = i.at("tid");

            //Creat one item with the game
            listItem = new brls::ListItem(fmt::format("{} ({})", title, tid));
            listItem->setHeight(100);
            listItem->setSubLabel(fmt::format("{}", tid));

            auto* icon = utils::getIconFromTitleId(tid);

            if (icon != nullptr)
                listItem->setThumbnail(icon, 0x20000);

            listItem->getClickEvent()->subscribe([title, this, tid](brls::View* view) {
                //Create a json with games found on gamebanana 
                nlohmann::json games_found = utils::searchGames(title);
                
                if (games_found.size()) {
                    if (games_found.at("_aMetadata").at("_nRecordCount").get<int>() > 1) {
                        //Create a list if there is more than one game found
                        this->game_selection = new brls::List();
                        for (auto i : games_found.at("_aRecords")) {
                            game = new brls::ListItem(i.at("_sName").get<std::string>());
                            game->setHeight(100);
                            game->getClickEvent()->subscribe([i, tid](brls::View* view) {
                                //TODO : Create the download view with the id which is getable with std::to_string(i.at("_idRow").get<int>());
                                Game game;
                                game.title = i.at("_sName").get<std::string>();
                                game.tid = tid;
                                game.gamebananaID = i.at("_idRow").get<int>();
                                game.icon = utils::getIconFromUrl(i.at("_sIconUrl").get<std::string>());
                                brls::Application::pushView(new ModsList(game));
                            });
                            this->game_selection->addView(game);
                        }

                        this->game_selection->registerAction("", brls::Key::B, [] { brls::Application::popView(); return 0; });
                        brls::Application::pushView(game_selection);
                    }
                    

                    else if(games_found.at("_aMetadata").at("_nRecordCount").get<int>() == 1) {
                        Game game;
                        game.title = games_found.at("_aRecords")[0].at("_sName").get<std::string>();
                        game.tid = tid;
                        game.gamebananaID = games_found.at("_aRecords")[0].at("_idRow").get<int>();
                        game.icon = utils::getIconFromUrl(games_found.at("_aRecords")[0].at("_sIconUrl").get<std::string>());
                        brls::Application::pushView(new ModsList(game));
                    }
                    
                    else {
                        brls::Application::notify("menu/notify/no_games_gamebanana"_i18n);
                    }
                }
            });
            this->addView(listItem);
        }
        this->updateActionHint(brls::Key::A, "Open");
    }
    else {
        noGames = new brls::Label(
            brls::LabelStyle::REGULAR,
            "menu/label/no_games"_i18n,
            true);
        noGames->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(noGames);
    }
}

void DownloadTab::createList() {

}
