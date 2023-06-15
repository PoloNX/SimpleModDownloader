#include <switch.h>
#include <string>
#include <borealis.hpp>

#include "games_tab.hpp"
#include "main_activity.hpp"

using namespace brls::literals;

int main() {
    appletInitializeGamePlayRecording();

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);

    if(!brls::Application::init()) {
        brls::Logger::error("Failed to initialize application");
        return 1;
    }

    brls::Application::createWindow("SimpleModDownloader");

    brls::Application::setGlobalQuit(false);

    brls::Application::registerXMLView("GameTab", GameTab::create);

    brls::getLightTheme().addColor("captioned_image/caption", nvgRGB(2, 176, 183));
    brls::getDarkTheme().addColor("captioned_image/caption", nvgRGB(51, 186, 227));

    brls::getStyle().addMetric("about/padding_top_bottom", 50);
    brls::getStyle().addMetric("about/padding_sides", 75);
    brls::getStyle().addMetric("about/description_margin", 50);

    brls::Application::pushActivity(new MainActivity());

    while(brls::Application::mainLoop()) {
        ;
    }

    return 0;
}