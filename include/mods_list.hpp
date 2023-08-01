#pragma once

#include <borealis.hpp>
#include <nlohmann/json.hpp>
#include "utils.hpp"
#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

class ModsList : public brls::AppletFrame {
public:
    ModsList(Game Game, int page = 1);
    /*~ModsList() {
        delete this->list;
        delete this->listItem;
    }*/
    ModsList(Game game, const std::string& search, int page = 1);

    void createList(brls::View* return_view);
private:
    Game currentGame;
    brls::List* list;
    brls::ListItem* listItem;
    std::string search = "";
    nlohmann::json mods;
    int page;
};

class ModsPage : public brls::AppletFrame {
public:
    ModsPage(brls::View* return_view, Mod &mod, Game &game, const std::string& search, const int& page);
    /*~ModsPage() {
        delete this->list;
        delete this->listItem;
        delete this->label;
    }*/
private:
    Mod currentMod;
    Game currentGame;
    brls::List* list;
    brls::ListItem* listItem;
    brls::Label* label;
    int page;
};
