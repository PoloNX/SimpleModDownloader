#include "views/game_list_tab.hpp"

using namespace brls::literals;

class RecyclerHeader
    : public brls::RecyclerHeader
{
};

class RecyclerCell
    : public brls::RecyclerCell
{
  public:
    RecyclerCell();

    BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
    BRLS_BIND(brls::Label, label, "title");
    BRLS_BIND(brls::Label, subtitle, "subtitle");
    //BRLS_BIND(brls::Image, image, "image");

    static RecyclerCell* create();
};

class DataSource
    : public brls::RecyclerDataSource
{
  public:
    DataSource();
    int numberOfSections(brls::RecyclerFrame* recycler) override;
    int numberOfRows(brls::RecyclerFrame* recycler, int section) override;
    brls::RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath index) override;
    void didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath) override;
    std::string titleForHeader(brls::RecyclerFrame* recycler, int section) override;
  private:
    std::vector<Game> games;
};

RecyclerCell::RecyclerCell()
{
    this->inflateFromXMLRes("xml/cells/cell.xml");
}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}

DataSource::DataSource() {

}

int DataSource::numberOfSections(brls::RecyclerFrame* recycler) {
    return 1;
}

int DataSource::numberOfRows(brls::RecyclerFrame* recycler, int section) {
    return upcomingGames->getGames().size();
}

std::string DataSource::titleForHeader(brls::RecyclerFrame* recycler, int section) {
    return "";
}

GameListTab::GameListTab() {
    this->inflateFromXMLRes("xml/tabs/game_list_tab.xml");


}