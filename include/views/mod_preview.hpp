#pragma once

#include <borealis.hpp>

#include "api/mod.hpp"

class FileBox : public brls::Box {
    public:
        FileBox(File& file);
        brls::Button* getDownloadButton() { return download.getView(); }
    private:
        BRLS_BIND(brls::Label, title, "title");
        BRLS_BIND(brls::Label, date, "date");
        BRLS_BIND(brls::Label, size, "size");
        BRLS_BIND(brls::Button, download, "download");
};

class ModPreview : public brls::Box {
public:
    ModPreview(Mod& mod, std::vector<unsigned char>& bannerBuffer);
    ~ModPreview();
private:
    Mod mod;

    BRLS_BIND(brls::Rectangle, image_overlay, "image_overlay");
    BRLS_BIND(brls::Label, title, "mod_preview_title");
    BRLS_BIND(brls::Label, author, "author");
    BRLS_BIND(brls::Label, description, "description");
    BRLS_BIND(brls::ScrollingFrame, scrolling, "mod_preview_scrolling");
    BRLS_BIND(brls::Image, banner, "banner");

    BRLS_BIND(brls::Box, files_box, "files_box");
    BRLS_BIND(brls::Box, big_image_box, "big_image_box");
    BRLS_BIND(brls::Box, small_image_box, "small_image_box");

    void loadImages();
    void loadButtons();
    void stopThread();
    std::thread secondThread;
    std::mutex threadMutex;
    std::condition_variable threadCondition;
    bool stopThreadFlag = false;

    bool firstImageDownloaded = false;

    brls::Image* bigImg = new brls::Image();
};