#pragma once

#include <borealis.hpp>

#include "api/mod.hpp"
#include "views/spinner_image_view.hpp"

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
    BRLS_BIND(brls::Box, screenshot_box, "screenshots_box");

    std::vector<brls::Box*> smallScreenshotsBoxs;

    void loadImages();
    bool shouldStopThread();
    void processImageOnMainThread(size_t index, std::vector<unsigned char>& buffer);

    void loadButtons();
    void stopThread();
    std::thread secondThread;
    std::mutex threadMutex;
    std::condition_variable threadCondition;
    
    std::mutex bigImageMutex;

    const int bigImageWidth = 1000;

    bool stopThreadFlag = false;
    bool isExiting = false;
    bool firstImageDownloaded = false;

    SpinnerImageView* bigSpinImg = new SpinnerImageView(bigImageWidth, bigImageWidth * 9/16);
};