#pragma once

#include <borealis.hpp>

class SettingsTab : public brls::Box {
public:
    SettingsTab();

    static brls::View* create();
private:
    BRLS_BIND(brls::SelectorCell, language_selector, "language_selector");
    BRLS_BIND(brls::BooleanCell, strict_cell, "strict_cell");
    BRLS_BIND(brls::Box, settings_box, "settings_box");
};