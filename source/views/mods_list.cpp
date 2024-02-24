#include "views/mods_list.hpp"
#include "utils/utils.hpp"

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
    cell->subtitle->setText(fmt::format("Author : {}", modList->getMods()[indexPath.row].getAuthor()));
    return cell;
}

void ModData::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{ 

}

ModData::ModData(Game game): game(game)
{
    //modList = new ModList(game);
    modList = std::make_unique<ModList>(game);
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

ModListTab::ModListTab(const Game& game) {
    this->inflateFromXMLRes("xml/tabs/mods.xml");

    modData = std::make_unique<ModData>(game);

    this->registerAction("next", brls::ControllerButton::BUTTON_RT, [this](brls::View* view){
        brls::Logger::debug("Next button pressed");
        this->modData->getModList()->nextPage();
        this->recycler->reloadData();
        return true;
    });

    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Cell", []() { return ModCell::create();});
    recycler->setDataSource(modData.get());
}

brls::View* ModListTab::create() {
    return new ModListTab();
}