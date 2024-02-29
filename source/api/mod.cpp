#include "api/mod.hpp"
#include "api/net.hpp"
#include "utils/utils.hpp"

Mod::Mod(const std::string &name, int ID, const std::vector<std::string>& imageUrls, const std::string &author, const Game& game): game(game) {
    this->name = name;
    this->ID = ID;
    this->imageUrls = imageUrls;
    this->author = author;
}

std::vector<unsigned char> Mod::downloadImage(const int& index) {
    std::vector<unsigned char> buffer;

    net::downloadImage(fmt::format("https://images.gamebanana.com/img/ss/mods/{}",imageUrls[index]), buffer);
       
    if(buffer.size() <= 0) {
        brls::Logger::error("Failed to download image: {}", imageUrls[index]);
        return buffer;
    } 
    imageBuffer = buffer;
    return buffer;
}

void Mod::loadImage(const int& index) {
    std::vector<unsigned char> buffer = imageBuffer;

    if(buffer.size() <= 0) {
        brls::Logger::error("Failed to download image: {}", imageUrls[index]);
        return;
    }
    auto image = new brls::Image();
    image->setImageFromMem(buffer.data(), buffer.size());
    images.push_back(image);
    imageBuffers.push_back(buffer);
    imageBuffer.clear();
}


brls::Image* Mod::getImage(const int& index) {
    return images[index];
}

void Mod::loadMod() {
    nlohmann::json mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_sText", std::to_string(this->ID)));

    this->description = mod_json.at("_sText");
    this->description = utils::removeHtmlTags(this->description);

    nlohmann::json files_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_aFiles", std::to_string(this->ID)));

    for(auto file : files_json.at("_aFiles")) {
        std::string name = file.at("_sFile");
        std::string url = file.at("_sDownloadUrl");
        int size = file.at("_nFilesize");
        std::string checkSum = file.at("_sMd5Checksum");
        int date = file.at("_tsDateAdded");

        files.push_back(File(name, size, url, checkSum, this->getName(), date, game));
    }

    nlohmann::json images_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_aPreviewMedia", std::to_string(this->ID)));

    for(auto image : images_json.at("_aPreviewMedia").at("_aImages")) {
        std::string url = image.at("_sFile");
        imageUrls.push_back(url);
    }
}

File::File(const std::string &name, const int &size, const std::string &url, const std::string &checkSum, const std::string& modName,const int& date, const Game& game): game(game) {            
    this->name = name;
    this->size = size;
    this->url = url;
    this->checkSum = checkSum;
    this->date = date;
    this->path = fmt::format("sdmc:/config/SimpleModDownloader/{}", name);
    this->modName = modName;
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

        Mod newMod(name, ID, images, author, game);

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