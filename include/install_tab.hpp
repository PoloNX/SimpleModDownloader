#pragma once

#include <borealis.hpp>

class InstallTab : public brls::List {
public:
    InstallTab();
private:
    brls::Label* title;
    brls::Label* description;
    
    std::string SimpleModManagerPath;

    brls::ListItem* install;
};