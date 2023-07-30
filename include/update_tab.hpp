#pragma once

#include <borealis.hpp>


#include <nlohmann/json.hpp>

class UpdateTab : public brls::List {
public:
    UpdateTab();
private:
    brls::Label* label;


    std::string version;
    nlohmann::json json;

    brls::ListItem* item;

    std::string nro_url;
};