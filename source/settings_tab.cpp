#include "settings_tab.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

SettingsTab::SettingsTab() {
    item = new brls::SelectListItem(
        "menu/settings_tab/language"_i18n,
        langVector
    );
    item->setHeight(50);

    nlohmann::json json;
    std::ifstream file("sdmc:/config/SimpleModDownloader/settings.json");
    file >> json;
    file.close();

    std::string currentLang = utils::getCurrentLang();

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
        std::ifstream file("sdmc:/config/SimpleModDownloader/settings.json");
        file >> settings;
        file.close();

        settings.at("language") = langVector[selection];
    
        std::ofstream outFile("sdmc:/config/SimpleModDownloader/settings.json");
        outFile << settings.dump(4);
        outFile.close();
    });
    this->addView(item);
}