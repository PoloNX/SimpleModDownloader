#include "settings_tab.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

SettingsTab::SettingsTab() {
    nlohmann::json settings;
    item = new brls::SelectListItem(
        "menu/settings_tab/language"_i18n,
        langVector
    );
    item->setHeight(50);
    arcropolisItem = new brls::SelectListItem(
        "menu/settings_tab/arc"_i18n,
        arcVector
    );
    arcropolisItem->setHeight(50);

    std::string currentLang = utils::getCurrentLang();
    bool hasArcropolisOption = utils::fileHasSmashOption();

    // If the settings.json is from an older version, add the json ARCropolis option
    if (!hasArcropolisOption){
        brls::Logger::debug("Adding arcropolis option to settings.json");
        settings = utils::getSettings();
        settings["use_arcropolis"] = "false";
        utils::writeSettings(settings);
    }
    
    // Set current value
    if (utils::useARCropolis() == true) {
        arcropolisItem->setSelectedValue(0);
    } else {
        arcropolisItem->setSelectedValue(1);
    }
    

    if(utils::isLangAuto()) {
        item->setSelectedValue(langVector.size() - 1);
    }
    else {
        for (auto i = 0; i < langVector.size(); ++i) {
            if (langVector[i] == currentLang) {
                item->setSelectedValue(i);
            }
        }
    }
    item->getValueSelectedEvent()->subscribe([=](size_t selection) {
        nlohmann::json settings;
        settings = utils::getSettings();

        settings.at("language") = langVector[selection];
        utils::writeSettings(settings);
    });
    arcropolisItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
        nlohmann::json settings;
        settings = utils::getSettings();

        settings.at("use_arcropolis") = arcVector[selection];
        utils::writeSettings(settings);
    });
    this->addView(item);
    this->addView(arcropolisItem);
}