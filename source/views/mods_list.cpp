#include "views/mods_list.hpp"
#include "views/mod_preview.hpp"
#include "utils/utils.hpp"
#include "api/net.hpp"

#include <borealis.hpp>

using namespace brls::literals;

ModCell::ModCell()
{
    this->inflateFromXMLRes("xml/cells/cell_no_img.xml");
}

ModCell* ModCell::create()
{
    return new ModCell();
}

brls::RecyclerCell* ModData::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    auto cell = (ModCell*)recycler->dequeueReusableCell("Cell");
    brls::Logger::debug("Mod name : {}", modList->getMods()[indexPath.row].getName());
    cell->label->setText(modList->getMods()[indexPath.row].getName());
    cell->subtitle->setText(fmt::format("{} : {}", "menu/mods/author"_i18n, modList->getMods()[indexPath.row].getAuthor()));
    return cell;
}

void ModData::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{ 
    brls::Logger::debug("Mod name : {}", modList->getMods()[indexPath.row].getName());
    recycler->present(new ModPreview(modList->getMods()[indexPath.row], bannerBuffer));
    
    //recycler->present(new ModPreview(modList->getMods()[indexPath.row]));
}

ModData::ModData(Game game): game(game)
{
    //modList = new ModList(game);
    modList = std::make_unique<ModList>(game);
    net::downloadImage(game.getBannerUrl(), bannerBuffer);
    brls::Logger::debug("{} mods found", modList->getMods().size());
}

int ModData::numberOfSections(brls::RecyclerFrame* recycler)
{
    return 1;
}

int ModData::numberOfRows(brls::RecyclerFrame* recycler, int section)
{
    return modList->getMods().size();
}

std::string ModData::titleForHeader(brls::RecyclerFrame* recycler, int section)
{
    return "";
}

ModListTab::ModListTab(Game& game) {
    this->inflateFromXMLRes("xml/tabs/mods.xml");

    modData = std::make_unique<ModData>(game);
    getAppletFrameItem()->title = fmt::format("{} {}", "menu/mods/mods_for"_i18n, game.getTitle());

    this->registerAction("<", brls::ControllerButton::BUTTON_LB, [this](brls::View* view){
        brls::Logger::debug("previous button pressed");
        this->modData->getModList()->previousPage();
        this->recycler->reloadData();
        return true;
    });

    this->registerAction(">", brls::ControllerButton::BUTTON_RB, [this](brls::View* view){
        brls::Logger::debug("Next button pressed");
        this->modData->getModList()->nextPage();
        this->recycler->reloadData();
        return true;
    });

    this->registerAction("menu/mods/search"_i18n, brls::ControllerButton::BUTTON_Y, [this](brls::View* view) {
        brls::Logger::debug("Search button pressed");
        std::string search;
        brls::Application::getImeManager()->openForText([&](std::string text){
            search = text;
        });
        if(search.size() < 3) {
            auto dialog = new brls::Dialog("menu/notify/string_to_short"_i18n);
            dialog->addButton("hints/ok"_i18n, []() {});
            dialog->open();
            return true;
        }
        this->modData->getModList()->search(search);
        this->recycler->reloadData();
        return true;
    });

    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Cell", []() { return ModCell::create();});
    recycler->setDataSource(modData.get(), false);
}

/*brls::View* ModListTab::create() {
    return new ModListTab();
}*/