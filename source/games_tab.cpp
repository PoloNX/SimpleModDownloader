#include "games_tab.hpp"
#include "utils.hpp"

#include <nlohmann/json.hpp>

nlohmann::json games;

RecyclerCell::RecyclerCell() {
    this->inflateFromXMLRes("xml/cells/cell.xml");
}

RecyclerCell* RecyclerCell::create() {
    return new RecyclerCell();
}

// DATA SOURCE

int DataSource::numberOfSections(brls::RecyclerFrame* recycler) {
    return 2;
}

int DataSource::numberOfRows(brls::RecyclerFrame* recyclern, int section) {
    return games.size();
}

std::string DataSource::titleForHeader(brls::RecyclerFrame* recycler, int section) {
    if (section == 0)
        return "";
    return "Section #" + std::to_string(section+1);
}

brls::RecyclerCell* DataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    RecyclerCell* item = (RecyclerCell*)recycler->dequeueReusableCell("Cell");
    item->label->setText(games[indexPath.row].at("title"));
    item->image->setImageFromMem(utils::getIconFromTitleId(games[indexPath.row].at("tid")), 0x20000);
    return item;
}

void DataSource::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
//    brls::Logger::info("Item Index(" + std::to_string(index.section) + ":" + std::to_string(index.row) + ") selected.");
    //recycler->present(new PokemonView(pokemons[indexPath.row]));
}

GameTab::GameTab() {
    this->inflateFromXMLRes("xml/tabs/game_tab.xml");

    games.clear();
    games = utils::getInstalledGames();

    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Header", []() {return RecyclerHeader::create();});
    recycler->registerCell("Cell", [](){return RecyclerCell::create();});
    recycler->setDataSource(new DataSource());
}

brls::View* GameTab::create() {
    return new GameTab();
}