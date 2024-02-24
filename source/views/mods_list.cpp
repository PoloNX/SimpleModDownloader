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
    brls::Logger::debug("Number of rows : {}", modList->getMods().size());
    return modList->getMods().size();
}

std::string ModData::titleForHeader(brls::RecyclerFrame* recycler, int section)
{
    return "1";
}

ModListTab::ModListTab() {
    this->inflateFromXMLRes("xml/activity/mods.xml");

    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Cell", []() { return ModCell::create();});
    recycler->setDataSource(modData.get());
}

void ModListTab::setGame(const Game& game)
{
    modData = std::make_unique<ModData>(game);
    recycler->reloadData();
}

brls::View* ModListTab::create() {
    return new ModListTab();
}