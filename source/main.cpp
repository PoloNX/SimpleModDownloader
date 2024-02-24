#include <borealis.hpp>
#include <switch.h>
#include <curl/curl.h>

#include "views/game_list_tab.hpp"
#include "activity/main_activity.hpp"

void init();
void exit();

int main(int argc, char* argv[]) {
    brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);

    if(!brls::Application::init()) {
        brls::Logger::error("Unable to init Borealis application");
    }

    init();

    brls::loadTranslations();

    brls::Application::createWindow(fmt::format("UpcomingSwitchGames"));
    brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);
    brls::Application::setGlobalQuit(false);

    //XML View
    brls::Application::registerXMLView("GameListTab", GameListTab::create);
    //brls::Application::registerXMLView("ModListTab", ModListTab::create);

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
    plInitialize(PlServiceType_User);
    nsInitialize();
    //socketInitializeDefault();
    //nxlinkStdio();
    pmdmntInitialize();
    pminfoInitialize();
    splInitialize();
    fsInitialize();
    romfsInit();
    curl_global_init(CURL_GLOBAL_ALL);
}

void exit() {
    romfsExit();
    splExit();
    pminfoExit();
    pmdmntExit();
    socketExit();
    nsExit();
    setsysExit();
    fsExit();
    plExit();
    curl_global_cleanup();
}