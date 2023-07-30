#include "install_tab.hpp"
#include <filesystem>
#include "worker_page.hpp"
#include <switch.h>
#include "download.hpp"
#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

InstallTab::InstallTab() {
    title = new brls::Label(brls::LabelStyle::REGULAR, "menu/label/install"_i18n, true);
    title->setFontSize(25);
    title->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(title);

    description = new brls::Label(brls::LabelStyle::DESCRIPTION, "menu/label/desc_install"_i18n, true);
    this->addView(description);


    std::filesystem::path path ("sdmc:/switch/SimpleModManager/SimpleModManager.nro");
    std::filesystem::path alternative_path ("sdmc:/switch/SimpleModManager.nro");

    if (std::filesystem::exists(path)) {
        SimpleModManagerPath = path;
    } else if (std::filesystem::exists(alternative_path)) {
        SimpleModManagerPath = alternative_path;
    } else {
        SimpleModManagerPath = "";
    }


    if (SimpleModManagerPath != "") {
        install = new brls::ListItem("menu/item/launch_smm"_i18n, "", fmt::format("{} : {}", "menu/label/path"_i18n ,SimpleModManagerPath));
        install->setHeight(100);
        install->getClickEvent()->subscribe([this](brls::View* view) {
            envSetNextLoad(SimpleModManagerPath.c_str(), fmt::format("\"{}\"", SimpleModManagerPath).c_str());
            brls::Application::quit();
        });
        
        this->addView(install);
    } else {
        install = new brls::ListItem("menu/item/download_smm"_i18n);
        install->setThumbnail(fmt::format("url : {}", SimpleModManager_URL));
        install->getClickEvent()->subscribe([this, path](brls::View* view) {
            auto* stagedFrame = new brls::StagedAppletFrame();

            stagedFrame->setTitle("menu/item/downloading_smm"_i18n);

            stagedFrame->addStage(new WorkerPage(stagedFrame, "menu/title/downloading"_i18n, [this]() {
                net::downloadFile(SimpleModManager_URL, "sdmc:/switch/SimpleModManager.nro");
            }));

            brls::Application::pushView(stagedFrame);

        });
        this->addView(install);
    }


}