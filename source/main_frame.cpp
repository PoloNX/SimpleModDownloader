#include "main_frame.hpp"
#include "games_tab.hpp"
#include "install_tab.hpp"
#include "about_tab.hpp"
#include "update_tab.hpp"
#include "settings_tab.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

MainFrame::MainFrame() {
    //Initialize the main frame
    this->setTitle(fmt::format("{} {}", "menu/main/app_name"_i18n, APP_VER));

    //TODO : Create one icon
    this->setIcon(BOREALIS_ASSET("icon/icon.png")); 

    //Here we add tabs to the main frame
    this->addTab("menu/tabs/downloads"_i18n, new DownloadTab());

    this->addTab("menu/tabs/install"_i18n, new InstallTab());

    this->addTab("menu/tabs/update"_i18n, new UpdateTab());

    this->addSeparator();

    this->addTab("menu/tabs/settings"_i18n, new SettingsTab());

    this->addTab("menu/tabs/about"_i18n, new AboutTab());

    this->registerAction("", brls::Key::B, [this] { return true; });
}