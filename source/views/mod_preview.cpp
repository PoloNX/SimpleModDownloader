#include "views/mod_preview.hpp"

using namespace brls::literals;

ModPreview::ModPreview(Mod& mod) {
    this->mod = mod;

    brls::Logger::debug("ModPreview::ModPreview");

    this->inflateFromXMLRes("xml/tabs/mod_preview.xml");

    brls::Logger::debug("ModPreview::ModPreview 2");

    auto dismissAction = [this](brls::View* view) {
        brls::Application::popActivity();
        return true;
    };

    this->setFocusable(false);
    scrolling->setFocusable(true);

    brls::Logger::debug("ModPreview::ModPreview 3");

    this->registerAction("Close", brls::ControllerButton::BUTTON_B, dismissAction);
    
    image_overlay->setColor(nvgRGBA(100, 100, 100, 0.8*255));
    title->setText(mod.getName());
    author->setText(fmt::format("Author : {}", mod.getAuthor()));
    brls::Logger::info("Mod description : {}", mod.getDescription());
    description->setText(mod.getDescription());
}