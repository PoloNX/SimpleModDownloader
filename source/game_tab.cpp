#include "game_tab.hpp"
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
    return 1;
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
    item->image->setImageFromMem(utils::getIconFromTitleId(games[indexPath.row].at("tid")), utils::getIconFromTitleId(games[indexPath.row].at("tid")).size() )
    return item;
}