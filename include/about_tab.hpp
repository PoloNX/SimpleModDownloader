#pragma once

#include <borealis.hpp>

namespace i18n = brls::i18n;
using namespace i18n::literals;

class AboutTab : public brls::List {
public:
    AboutTab();
private:
    brls::Label* title;
    brls::Label* description;
    brls::Label* thanks;
};