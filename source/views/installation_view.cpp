#include "views/installation_view.hpp"

#include <switch.h>

InstallationView::InstallationView() {
    this->inflateFromXMLRes("xml/tabs/installation_tab.xml");

    smm_desc->setText("SimpleModDownloader allow you to switch to SimpleModManager, a mod manager for the Nintendo Switch. It will allow you to install mods.");

    smm_radio->setSelected(false);
    smm_radio->title->setText("Launch SimpleModManager");
    smm_radio->registerClickAction([this](brls::View* view) {
        envSetNextLoad("/switch/SimpleModManager/SimpleModManager.nro", "\"/switch/SimpleModManager/SimpleModManager.nro\"");
        brls::Application::quit();
        return true;
    });
}