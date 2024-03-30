#include "views/game_list_tab.hpp"
#include "utils/utils.hpp"
#include "views/mods_list.hpp"

#include <borealis.hpp>

using namespace brls::literals;

GameCell::GameCell()
{
    this->inflateFromXMLRes("xml/cells/cell.xml");
}

GameCell* GameCell::create()
{
    return new GameCell();
}

brls::RecyclerCell* GameData::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    auto cell = (GameCell*)recycler->dequeueReusableCell("Cell");
    cell->label->setText(games[indexPath.row].first);
    cell->subtitle->setText(fmt::format("TitleID : {}",games[indexPath.row].second));
    uint8_t* icon = utils::getIconFromTitleId(games[indexPath.row].second);
    if(icon != nullptr)
        cell->image->setImageFromMem(icon, 0x20000);
    return cell;
}

void GameData::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{ 
    Game game(games[indexPath.row].first, games[indexPath.row].second);

    auto modListTab = new ModListTab(game);
    recycler->present(modListTab);    
}

GameData::GameData() {
    games = utils::getInstalledGames();
    brls::Logger::debug("{} games found", games.size());
}

int GameData::numberOfSections(brls::RecyclerFrame* recycler) {
    return 1;
}

int GameData::numberOfRows(brls::RecyclerFrame* recycler, int section) {
    return games.size();
}

std::string GameData::titleForHeader(brls::RecyclerFrame* recycler, int section) {
    return "";
}

GameListTab::GameListTab() {
    this->inflateFromXMLRes("xml/tabs/game_list_tab.xml");

    gameData = new GameData();


    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Cell", []() { return GameCell::create();});
    recycler->setDataSource(gameData, false);
}

brls::View* GameListTab::create() {
    return new GameListTab();
}