#include "api/mod.hpp"
#include "api/net.hpp"

Mod::Mod(const std::string &name, int ID, const std::vector<std::string>& imageUrls, const std::string &author) {
    this->name = name;
    this->ID = ID;
    this->imageUrls = imageUrls;
    this->author = author;
}

void Mod::downloadImages() {
    //todo
}

void Mod::loadImages() {
    //todo
}

File::File(const std::string &name, const std::string &path, const std::string &url, const std::string &checkSum, const std::string &date) {            
    this->name = name;
    this->path = path;
    this->url = url;
    this->checkSum = checkSum;
    this->date = date;
}

ModList::ModList(Game game): game(game) {
    updatePage();
}

void ModList::updatePage() {
    nlohmann::json mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}?_nPerpage=50&_csvModelInclusions=Mod", "18604", currentPage));

    mods.clear();

    for(auto mod : mod_json.at("_aRecords")) {
        std::string name = mod.at("_sName");
        int ID = mod.at("_idRow");
        
        std::string author;// = mod.at("_aSubmitter").at("_sName");

        std::vector<std::string> images;
        // for(auto image : mod.at("_aPreviewMedia").at("_aImages")) {
        //     std::string url = image.at("_sFile530");
        //     images.push_back(url);
        // }

        Mod newMod(name, ID, images, author);

        mods.push_back(newMod);
    }
}

void ModList::nextPage() {
    currentPage++;
    updatePage();
}

void ModList::previousPage() {
    if(currentPage > 1) {
        currentPage--;
        updatePage();
    }
}