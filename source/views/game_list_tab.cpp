#include "views/game_list_tab.hpp"
#include "utils/utils.hpp"

#include <borealis.hpp>

using namespace brls::literals;

RecyclerCell::RecyclerCell()
{
    this->inflateFromXMLRes("xml/cells/cell.xml");
}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}

brls::RecyclerCell* DataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    auto cell = (RecyclerCell*)recycler->dequeueReusableCell("Cell");
    cell->label->setText(games[indexPath.row].first);
    cell->subtitle->setText(games[indexPath.row].second);
    return cell;
}

void DataSource::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{ 
    //brls::Application::pushActivity(new brls::Activity(new GameView(upcomingGames->getGames()[indexPath.row])));    
}

DataSource::DataSource() {
    games = utils::getInstalledGames();
    brls::Logger::debug("{} games found", games.size());
}

int DataSource::numberOfSections(brls::RecyclerFrame* recycler) {
    return 1;
}

int DataSource::numberOfRows(brls::RecyclerFrame* recycler, int section) {
    return games.size();
}

std::string DataSource::titleForHeader(brls::RecyclerFrame* recycler, int section) {
    return "";
}

GameListTab::GameListTab() {
    this->inflateFromXMLRes("xml/tabs/game_list_tab.xml");

    dataSource = new DataSource();


    recycler->estimatedRowHeight = 100;
    recycler->registerCell("Cell", []() { return RecyclerCell::create();});
    recycler->setDataSource(dataSource);
}

brls::View* GameListTab::create() {
    return new GameListTab();
}