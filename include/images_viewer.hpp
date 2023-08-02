#pragma once

#include "constants.hpp"
#include "mods_list.hpp"
#include <borealis.hpp>
#include <vector>

class ImagesViewer : public brls::AbsoluteLayout{
public:
    ImagesViewer(Mod& mod, Game& game);
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

    Mod &currentMod; 
    Game& currentGame;
};