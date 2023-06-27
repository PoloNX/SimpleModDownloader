#include "mods_list.hpp"
#include "main_frame.hpp"
#include "images_viewer.hpp"
#include "worker_page.hpp"
#include "download.hpp"
#include "confirm_page.hpp"
#include "extract.hpp"

#include "SimpleIniParser.hpp"
#include <filesystem>
#include <regex>
#include <iostream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

ModsList::ModsList(Game game, int page_f) : AppletFrame(true, true), currentGame(game), page(page_f) {
    //Create a json with all the 1 page of mods  we can do a loop to fetch a number of pages
    mods = utils::getMods(currentGame.gamebananaID, page);
    this->setTitle("menu/mods/mods_list_title"_i18n);
    //We create a list
    createList();
}

ModsList::ModsList(Game game, const std::string& search, int page_f) : AppletFrame(true, true), currentGame(game), search(search), page(page_f) {
    //Create a json with all the 1 page of mods  we can do a loop to fetch a number of pages
    mods = utils::getMods(currentGame.gamebananaID, search, page);
    this->setTitle("menu/mods/mods_list_title"_i18n);
    //We create a list
    createList();
}

void ModsList::createList(){
    list = new brls::List();

    brls::Logger::debug("json : {}", mods.dump(4));



    for (auto i : mods.at("_aRecords")) {
        //Check if the mod can be downloaded or no
        if(!i.at("_bHasFiles").get<bool>()) continue;

        listItem = new brls::ListItem(i.at("_sName").get<std::string>());

        listItem->getClickEvent()->subscribe([i, this](brls::View* view) {
            Mod currentMod;
            //initialize a struct which contains all importants informations for one mod
            currentMod.title = i.at("_sName").get<std::string>();
            currentMod.ID = i.at("_idRow").get<int>();
            currentMod.desc = utils::getDescription(currentMod.ID);
            currentMod.json = i;
            currentMod.author = i.at("_aSubmitter").at("_sName").get<std::string>();


            nlohmann::json json = utils::getDownloadLinks("Mod", currentMod.ID);

            for (auto i : json){
                File file; 
                file.name = i.at("_sFile").get<std::string>();
                file.checkSum = i.at("_sMd5Checksum").get<std::string>();
                file.size = utils::formatFileSize(i.at("_nFilesize").get<int>());
                file.url = i.at("_sDownloadUrl").get<std::string>();
                file.date = utils::convertTimestampToString(i.at("_tsDateAdded").get<int>());
                currentMod.files.push_back(file);
            }
            
            brls::PopupFrame::open(fmt::format("Mod : {}", currentMod.title), new ModsPage(currentMod, this->currentGame, this->search, this->page));
        });

        list->addView(listItem);
    }

    this->setContentView(list);
    //Back to MainFrame if you press B

    this->registerAction("<", brls::Key::L, [this] { 
        if (page > 1) {
            if (search != "")
                brls::Application::pushView(new ModsList(this->currentGame, this->search, this->page-1));
            else
                brls::Application::pushView(new ModsList(this->currentGame, this->page-1));
        }
        return 0;
    });
    this->registerAction(">", brls::Key::R, [this] { 
        if(search != "")
            brls::Application::pushView(new ModsList(this->currentGame, this->search, this->page+1));
        else
            brls::Application::pushView(new ModsList(this->currentGame, this->page+1));
        return 0;
    });


    this->registerAction("menu/mods/search"_i18n, brls::Key::Y, [this] { 
        std::string search;
        brls::Swkbd::openForText([&search](std::string text) {search = text;}, "menu/mods/search"_i18n, "menu/mods/search_placeholder"_i18n, 100, "", 0, "menu/mods/search"_i18n, "menu/mods/cancel"_i18n);
        brls::Application::pushView(new ModsList(this->currentGame, search, 1));
        return 0;
    });
    this->registerAction("", brls::Key::B, [] { brls::Application::pushView(new MainFrame()); return 0;});

    this->setFooterText(fmt::format("{} : {}", "menu/mods/page"_i18n,  this->page));
}

ModsPage::ModsPage(Mod &mod, Game& game, const std::string& search, const int& page) : AppletFrame(true, true), currentMod(mod), currentGame(game), page(page) {
    this->updateActionHint(brls::Key::B, "");
    this->updateActionHint(brls::Key::PLUS, "");

    this->list = new brls::List();
    this->label = new brls::Label(
        brls::LabelStyle::REGULAR,
        "menu/mods/links"_i18n,
        true);

    this->list->addView(this->label);

    //Prevent "/" in the name of the mod (for example : "Sonic/Shadow")
    this->currentMod.title.erase(std::remove(this->currentMod.title.begin(), this->currentMod.title.end(), '/'), this->currentMod.title.end());

    /*-------------------------------------------------*/
    //Create one item for downloading
    for (auto i : this->currentMod.files) {
            this->listItem = new brls::ListItem(i.name);
            this->listItem->setHeight(100);
            this->listItem->setSubLabel(fmt::format("{} : {} | {} : {}","menu/mods/size"_i18n, i.size,"menu/mods/size"_i18n, i.date));

            this->listItem->getClickEvent()->subscribe([this, i](brls::View* view) {
                std::string extension = i.name.substr(i.name.find_last_of(".") + 1);
                if(extension == "zip" || extension == "7z"){
                    
                    
                    auto* stagedFrame = new brls::StagedAppletFrame();

                    stagedFrame->setTitle(fmt::format("{} {}","menu/worker/install"_i18n, i.name));
                    
                    stagedFrame->addStage(new WorkerPage(stagedFrame, "menu/title/downloading"_i18n, [this, i]() {
                        net::downloadFile( i.url, fmt::format("sdmc:/config/SimpleModDownloader/{}", i.name));
                    }));

                    stagedFrame->addStage(new WorkerPage(stagedFrame, "menu/title/extracting"_i18n, [this, i]() {
                        brls::Logger::debug("tid : {}",this->currentGame.tid);
                        std::regex pattern(":");
                        std::string resultat = std::regex_replace(this->currentGame.title, pattern, " -");

                        

                        extract::extractEntry(fmt::format("sdmc:/config/SimpleModDownloader/{}", i.name), fmt::format("sdmc:{}{}/{}/contents/{}/romfs",utils::getModInstallPath(),resultat, this->currentMod.title, this->currentGame.tid));
                    }));
                    
                    stagedFrame->addStage(new ConfirmPage(stagedFrame, "menu/label/extract"_i18n));

                    brls::Application::pushView(stagedFrame);
                }
                else {
                    brls::Application::notify("menu/notify/not_supported"_i18n + extension);
                }
            });

            this->list->addView(this->listItem);
    }
    
    /*--------------------------------------------------*/
    this->label = new brls::Label(
        brls::LabelStyle::REGULAR,
        fmt::format("{}", this->currentMod.desc),
        true);
    this->list->addView(this->label);

    /*--------------------------------------------------*/
    //Create one item to view images
    this->listItem = new brls::ListItem("menu/mods/images"_i18n);
    this->listItem->setHeight(50);

    this->listItem->getClickEvent()->subscribe([this, search](brls::View* view) {
        if (this->currentMod.json.at("_aPreviewMedia").contains("_aImages")) {
            this->currentMod.images = utils::getModsImages(this->currentMod.json, 0, this->currentMod.sizeBigImage);
            this->currentMod.currentBigImage = 0;
            if (search !="")
                brls::Application::pushView(new ImagesViewer(this->currentMod, this->currentGame, search, this->page));
            else
                brls::Application::pushView(new ImagesViewer(this->currentMod, this->currentGame, this->page));
        }
        else {
            brls::Application::notify("menu/notify/no_images"_i18n);
        }
    });

    this->list->addView(this->listItem);

    /*----------------------------------------------------*/
    this->listItem = new brls::ListItem("menu/mods/browser"_i18n);
    this->listItem->setHeight(50);

    this->listItem->getClickEvent()->subscribe([this](brls::View* view) {
        utils::openWebBrowser(fmt::format("https://gamebanana.com/mods/{}", this->currentMod.ID));
    });
    this->list->addView(this->listItem);

    /*----------------------------------------------------*/
    this->label = new brls::Label(
        brls::LabelStyle::REGULAR,
        fmt::format("{} : {}", "menu/mods/author"_i18n ,this->currentMod.author),
        true);
    this->list->addView(this->label);
    /*-----------------------------------------------------*/

    this->setContentView(list);
}
