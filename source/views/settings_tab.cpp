#include "views/settings_tab.hpp"
#include "utils/config.hpp"

#include <filesystem>

using namespace brls::literals;

SettingsTab::SettingsTab() {
    this->inflateFromXMLRes("xml/tabs/settings_tab.xml");
    cfg::Config config;

    std::filesystem::path i18n_path = "romfs:/i18n";
    std::vector<std::string> languages = {"auto"};
    for (const auto& entry : std::filesystem::directory_iterator(i18n_path)) {
        if (entry.is_directory()) {
            languages.push_back(entry.path().filename().string());
        }
    }

    int selected = 0;
    std::string current_language = config.getAppLanguage();
    for(auto i = 0; i < languages.size(); ++i) {
        if(languages[i] == current_language) {
            selected = i;
            break;
        }
    }

    this->language_selector->init("menu/settings_tab/language"_i18n, languages, selected, [](int selected){}, [languages = std::move(languages)](int selected) {
        cfg::Config config;
        config.setAppLanguage(languages[selected]);
    });

    this->strict_cell->init("menu/settings_tab/strict"_i18n, config.getStrictSearch(), [](bool value){
        cfg::Config config;
        config.setStringSearch(value);
    });

    #ifndef NDEBUG
    auto debug_cell = new brls::BooleanCell();
    debug_cell->init("menu/settings_tab/debug"_i18n, brls::Application::isDebuggingViewEnabled(), [](bool value){
        brls::Application::enableDebuggingView(value);
        brls::sync([value](){
            brls::Logger::info("{} the debug layer", value ? "Open" : "Close");
        });
    });
    settings_box->addView(debug_cell);

    auto wireframe_cell = new brls::BooleanCell();
    wireframe_cell->init("menu/settings_tab/debug"_i18n, brls::Application::isDebuggingViewEnabled(), [](bool value){
        brls::sync([value](){
            brls::Logger::info("{} wireframe", value ? "Enable" : "Disable");
        });
        cfg::Config config;
        config.setWireframe(value);
    });
    settings_box->addView(wireframe_cell);

    if (config.getWireframe()) {
        brls::sync([this]{
            this->setWireframeEnabled(true);
            for (auto& view : this->getChildren()) {
                view->setWireframeEnabled(true);
            }
            for (auto& view : this->settings_box->getChildren()) {
                view->setWireframeEnabled(true);
            }
        });
    }
    #endif
}

brls::View* SettingsTab::create()
{
    return new SettingsTab;
}