#pragma once

#include <borealis.hpp>
#include "constants.hpp"

class DownloadTab : public brls::List {
private:
    brls::ListItem* listItem;
    brls::ListItem* game;
    brls::List* game_selection;
    brls::Label* noGames ;
    void createList();
public:
    DownloadTab();
    /*~DownloadTab() {
        delete this->listItem;
        delete this->game_selection;
        delete this->noGames;
        delete this->game;
    }*/
};