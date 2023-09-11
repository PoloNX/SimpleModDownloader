#pragma once

#include <borealis.hpp>

class SettingsTab : public brls::List {
public:
    SettingsTab();
private:
    brls::SelectListItem* item;
    brls::SelectListItem* arcropolisItem;
};