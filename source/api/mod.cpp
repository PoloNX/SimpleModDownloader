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

void Mod::loadMod() {
    nlohmann::json mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_sText", std::to_string(this->ID)));

    this->description = mod_json.at("_sText");

    /*nlohmann::json files_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_aFiles", std::to_string(this->ID)));

    for(auto file : files_json.at("_aFiles")) {
        std::string name = file.at("_sFile");
        std::string url = file.at("_sDownloadUrl");
        std::string size = file.at("_sFilesize");
        std::string checkSum = file.at("_sMd5Checksum");
        int date = file.at("_tsDateAdded");

        files.push_back(File(name, size, url, checkSum, date));
    }*/
}

File::File(const std::string &name, const std::string &size, const std::string &url, const std::string &checkSum, const int& date) {            
    this->name = name;
    this->size = size;
    this->url = url;
    this->checkSum = checkSum;
    this->date = date;
}

ModList::ModList(Game m_game): game(m_game) {
    updatePage();
}

void ModList::updatePage() {
    nlohmann::json mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}?_nPerpage=50&_csvModelInclusions=Mod", game.getGamebananaID(), currentPage));

    mods.clear();
    
    for(auto mod : mod_json.at("_aRecords")) {
        std::string name = mod.at("_sName");
        int ID = mod.at("_idRow");
        
        std::string author = mod.at("_aSubmitter").at("_sName");

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