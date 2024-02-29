#pragma once

#include <borealis.hpp>

class InstallationView : public brls::Box {
public:
    InstallationView();

    static brls::View* create() {
        return new InstallationView;
    }
private:
    BRLS_BIND(brls::Label, smm_desc, "smm_desc");
    BRLS_BIND(brls::RadioCell, smm_radio, "smm_button");
};