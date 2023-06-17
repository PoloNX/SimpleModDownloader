#include <borealis.hpp>
#include <switch.h>
#include <iostream>
#include "constants.hpp"
#include "main_frame.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

void init();
void exit();

int main() {
    if (!brls::Application::init(fmt::format("{} v{}", "menu/main/app_name"_i18n, APP_VER))) {
        brls::Logger::error("Unable to init borealis application");
        return EXIT_FAILURE;
    }

    //Init translation and app
    const std::string currentLocale = i18n::getCurrentLocale();
    if (currentLocale != "fr" && currentLocale != "en-US" && currentLocale != "es" && currentLocale != "de" && currentLocale != "it" && currentLocale != "zh-CN") 
        i18n::loadTranslations("en-US");
    else 
        i18n::loadTranslations();

    init();
    //Use debug log level
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);

    brls::Application::pushView(new MainFrame());

    while(brls::Application::mainLoop()) {
        ;
    }

    exit();
    //nsExit();
    return 0;
}

void init() {
    setsysInitialize();
    plInitialize(PlServiceType_User);
    nsInitialize();
    socketInitializeDefault();
    nxlinkStdio();
    pmdmntInitialize();
    pminfoInitialize();
    splInitialize();
    romfsInit();
}

void exit() {
    romfsExit();
    splExit();
    pminfoExit();
    pmdmntExit();
    socketExit();
    nsExit();
    setsysExit();
    plExit();
}
