#include "about_tab.hpp"

AboutTab::AboutTab() {
    title = new brls::Label(
        brls::LabelStyle::REGULAR,
        "menu/about/title"_i18n,
        true
    );
    title->setFontSize(25);
    title->setHorizontalAlign(NVG_ALIGN_CENTER);

    description = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menu/about/description"_i18n,
        true
    );
    title->setFontSize(20);


    thanks = new brls::Label(
        brls::LabelStyle::SMALL,
        "menu/about/thanks"_i18n,
        true
    );

    this->addView(title);
    this->addView(description);
    this->addView(thanks);
}