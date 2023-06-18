#include "update_tab.hpp"
#include "download.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "worker_page.hpp"
#include "confirm_page.hpp"

#include <iostream>


namespace i18n = brls::i18n;
using namespace i18n::literals;

UpdateTab::UpdateTab() {
    this->label = new brls::Label(brls::LabelStyle::REGULAR, "menu/label/update"_i18n, true);
    this->label->setFontSize(25);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(label);

    this->label = new brls::Label(brls::LabelStyle::DESCRIPTION, "menu/label/desc_smd"_i18n, true);
    this->addView(label);

    this->json = net::downloadRequest(SimpleModDownloader_API);
    
    std::cout << json << std::endl;

    if(this->json.size()) {
        this->version = this->json.at("tag_name").get<std::string>();
        this->nro_url = this->json.at("assets")[0].at("browser_download_url").get<std::string>();
        if (utils::is_older_version(APP_VER, version)) {
            this->label = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("{} {}\n{} {}","menu/label/new_update"_i18n, APP_VER, "menu/label/available_version"_i18n, this->version), true);
            this->addView(label);

            item = new brls::ListItem("menu/item/download_smd"_i18n, "", fmt::format("url : {}", this->nro_url));
            item->setHeight(100);

            item->getClickEvent()->subscribe([this](brls::View* view){
                brls::StagedAppletFrame* staggedFrame = new brls::StagedAppletFrame();
                staggedFrame->setTitle("menu/title/downloading_smd"_i18n);

                staggedFrame->addStage(new WorkerPage(staggedFrame, "menu/title/downloading_smd"_i18n, [this](){
                    net::downloadFile(this->nro_url, SimpleModDownloader_PATH);
                }));

                staggedFrame->addStage(new ConfirmPage(staggedFrame, "menu/label/restart_smd"_i18n, true));

                brls::Application::pushView(staggedFrame);
            });

            this->addView(item);
        }
        else {
            this->label = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("{} {}\n{} {}","menu/label/new_update"_i18n, APP_VER, "menu/label/available_version"_i18n, this->version), true);
            this->addView(label);
        }
    }
    else {
        this->label = new brls::Label(brls::LabelStyle::REGULAR, "menu/label/no_internet"_i18n, true);
        this->addView(label);
    }

}