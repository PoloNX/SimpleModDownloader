#include <borealis.hpp>
#include <switch.h>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>

#include "views/game_list_tab.hpp"
#include "views/installation_view.hpp"
#include "views/settings_tab.hpp"
#include "activity/main_activity.hpp"
#include "utils/config.hpp" 
#include "views/recycling_grid.hpp"

void init();
void exit();

int main(int argc, char* argv[]) {     
    init();
    
    brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);

    std::filesystem::create_directories("sdmc:/config/SimpleModDownloader");

    #ifdef NDEBUG //release
        // Using FILE* because brls::Logger::setLogOutput only takes FILE*, not std::ofstream
        FILE* logFile = fopen("sdmc:/config/SimpleModDownloader/log.log", "w");
        brls::Logger::setLogOutput(logFile);
    #endif


    {
        cfg::Config config;
        if(config.getAppLanguage() != "auto") {
            brls::Platform::APP_LOCALE_DEFAULT = config.getAppLanguage();
            brls::Logger::debug("Loaded translations for language {}", config.getAppLanguage());
        }
    }

    if(!brls::Application::init()) {
        brls::Logger::error("Unable to init Borealis application");
    }

    brls::loadTranslations();

    brls::Application::setGlobalQuit(false);

    brls::Application::createWindow(fmt::format("UpcomingSwitchGames"));
    brls::Application::setGlobalQuit(false);

    //XML View
    brls::Application::registerXMLView("GameListTab", GameListTab::create);
    brls::Application::registerXMLView("InstallationTab", InstallationView::create);
    brls::Application::registerXMLView("SettingsTab", SettingsTab::create);
    brls::Application::registerXMLView("RecyclingGrid", RecyclingGrid::create);

    // Add custom values to the theme
    brls::Theme::getLightTheme().addColor("captioned_image/caption", nvgRGB(2, 176, 183));
    brls::Theme::getDarkTheme().addColor("captioned_image/caption", nvgRGB(51, 186, 227));

    // Add custom values to the style
    brls::getStyle().addMetric("about/padding_top_bottom", 50);
    brls::getStyle().addMetric("about/padding_sides", 75);
    brls::getStyle().addMetric("about/description_margin", 50);

    brls::Application::pushActivity(new MainActivity());

    while(brls::Application::mainLoop()) ;

    exit();

    return -1;
}

void init() {
    setsysInitialize();
    socketInitializeDefault();
    nxlinkStdio();
    plInitialize(PlServiceType_User);
    nsInitialize();
    pmdmntInitialize();
    pminfoInitialize();
    splInitialize();
    fsInitialize();
    romfsInit();
    setInitialize();
    psmInitialize();
    nifmInitialize(NifmServiceType_User);
    lblInitialize();
    curl_global_init(CURL_GLOBAL_ALL);
}

void exit() {
    lblExit();
    nifmExit();
    psmExit();
    setExit();
    romfsExit();
    splExit();
    pminfoExit();
    pmdmntExit();
    nsExit();
    setsysExit();
    fsExit();
    plExit();
    socketExit();
    curl_global_cleanup();
}