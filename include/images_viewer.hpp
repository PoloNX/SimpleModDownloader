#pragma once

#include "constants.hpp"
#include "mods_list.hpp"
#include <borealis.hpp>
#include <vector>

class ImagesViewer : public brls::AbsoluteLayout{
public:
    ImagesViewer(Mod &mod, Game& game, const std::string& search, const int& page = 1);
    ImagesViewer(Mod& mod, Game& game, const int& page = 1);
    brls::View* getDefaultFocus() override;
    brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView) override;
    void init();
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash);
    void initItems();
private:
    brls::NavigationMap navigationMap;
    brls::Button* right;
    brls::Button* left;
    std::vector<brls::Image*> images;
    std::string search;

    int page;

    Mod &currentMod; 
    Game& currentGame;
};