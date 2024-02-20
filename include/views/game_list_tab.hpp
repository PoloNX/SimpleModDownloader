#pragma once

#include <borealis.hpp>

class GameListTab : public brls::Box {
public:
    GameListTab();

    static brls::View* create();
private:
    BRLS_BIND(brls::RecyclerFrame, recycler, "recycler");
};