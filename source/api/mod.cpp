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
    try {
        nlohmann::json mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_sText,_aFiles,_aPreviewMedia", std::to_string(this->ID)));

        this->description = mod_json.at("_sText");
        this->description = utils::removeHtmlTags(this->description);

        for(auto file : mod_json.at("_aFiles")) {
            std::string name = file.at("_sFile");
            std::string url = file.at("_sDownloadUrl");
            int size = file.at("_nFilesize");
            std::string checkSum = file.at("_sMd5Checksum");
            int date = file.at("_tsDateAdded");
            std::string id = std::to_string(file.at("_idRow").get<int>());

            brls::Logger::debug("File details: Name: {}, URL: {}, Size: {}, Checksum: {}, Date: {}, ID: {}", name, url, size, checkSum, date, id);

            files.push_back(File(name, size, url, checkSum, this->getName(), date, id, game));
        }

        for(auto image : mod_json.at("_aPreviewMedia").at("_aImages")) {
            std::string url = image.at("_sFile");

            brls::Logger::debug("Image URL: {}", url);

            imageUrls.push_back(url);
        }
    } catch (const std::exception& e) {
        brls::Logger::error("Error in loadMod: {}", e.what());
    }
}

File::File(const std::string &name, const int &size, const std::string &url, const std::string &checkSum, const std::string& modName,const int& date, const std::string& fileID,const Game& game): game(game) {            
    this->name = name;
    this->size = size;
    this->url = url;
    this->checkSum = checkSum;
    this->date = date;
    this->path = fmt::format("sdmc:/config/SimpleModDownloader/{}", name);
    this->modName = modName;
    this->fileID = fileID;
}

bool File::findRomfsRecursive(const nlohmann::json& obj) {
    for (const auto& item : obj.items()) {
        if (item.key() == "romfs") {
            brls::Logger::debug("found romfs");
            return true;
        }

        if (item.value().is_object()) {
            if (findRomfsRecursive(item.value())) {
                return true;
            }
        }
    }

    return false;
}

void File::loadFile() {
    auto json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/File/{}", fileID));
    if(json.at("_aMetadata").at("_aArchiveFileTree").is_object()) {
        auto archiveFileTree = json.at("_aMetadata").at("_aArchiveFileTree");
        for (const auto& item : archiveFileTree.items()) {
            if (item.value().is_object()) {
                if (findRomfsRecursive(item.value())) {
                    this->romfs = true;
                    break;
                }
            }
        }
    }
}

ModList::ModList(Game m_game): game(m_game) {
    updatePage();
}

void ModList::updatePage() {
    
    nlohmann::json mod_json;
    
    if(currentSearch == "") {
        mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}?_nPerpage=50&_csvModelInclusions=Mod", game.getGamebananaID(), currentPage));
    } else {
        mod_json = net::downloadRequest(fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}&_nPerpage=50&_sName={}&_csvModelInclusions=Mod", game.getGamebananaID(), currentPage, currentSearch));
    }

    if(mod_json.empty()) {
        return;
    }

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

void ModList::search(const std::string& search) {
    if(search.size() < 3) {
        return;
    }
    this->currentSearch = search;
    currentPage = 1;
    updatePage();
}