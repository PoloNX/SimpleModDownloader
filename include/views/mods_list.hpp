#pragma once

#include <borealis.hpp>

#include "views/recycling_grid.hpp"
#include "api/mod.hpp"
#include "api/game.hpp"

class ModCell : public RecyclingGridItem
{
  public:
    ModCell();

    BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
    BRLS_BIND(brls::Label, label, "modtitle");
    BRLS_BIND(brls::Label, subtitle, "modsubtitle");
    
    //TODO : Multithreaded image loading
    //BRLS_BIND(brls::Image, image, "image");

    static ModCell* create();
};

class CategorieCell : public brls::RecyclerCell
{
  public:
    CategorieCell();

    BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
    BRLS_BIND(brls::Label, label, "categorie");
    
    //TODO : Multithreaded image loading
    //BRLS_BIND(brls::Image, image, "image");

    static CategorieCell* create();
};

class ModData : public RecyclingGridDataSource
{
public:
    ModData(Game game);

    // int numberOfSections(brls::RecyclerFrame* recycler) override;
    // int numberOfRows(brls::RecyclerFrame* recycler, int section) override;
    // brls::RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath index) override;
    // void didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath) override;
    // std::string titleForHeader(brls::RecyclerFrame* recycler, int section) override;

    RecyclingGridItem* cellForRow(RecyclingGrid* recycler, size_t index) override;
    size_t getItemCount() override;
    void onItemSelected(RecyclingGrid* recycler, size_t index) override;
    void clearData() override;

    std::unique_ptr<ModList>& getModList() { return modList; }
private:
    Game& game;
    std::unique_ptr<ModList> modList;
    std::vector<unsigned char> bannerBuffer;
    //ModList* modList;
};

class ModListTab : public brls::Box {
public:
    ModListTab(Game& game);
    ModListTab();
    ~ModListTab() override;

    //static brls::View* create();

private:
    BRLS_BIND(RecyclingGrid, recycler, "modrecycler");

    std::unique_ptr<ModData> modData;
};