#pragma once

#include <borealis.hpp>

#include "api/mod.hpp"

class ModPreview : public brls::Box {
public:
    ModPreview(Mod& mod);
private:
    Mod mod;

    BRLS_BIND(brls::Rectangle, image_overlay, "image_overlay");
    BRLS_BIND(brls::Label, title, "mod_preview_title");
    BRLS_BIND(brls::Label, author, "author");
    BRLS_BIND(brls::Label, description, "description");
    BRLS_BIND(brls::ScrollingFrame, scrolling, "mod_preview_scrolling");
};