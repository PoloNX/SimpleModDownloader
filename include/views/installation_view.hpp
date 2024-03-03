#pragma once

#include <borealis.hpp>

class InstallationView : public brls::Box {
public:
    InstallationView();
    ~InstallationView() {
        if(downloadThread.joinable()) {
            downloadThread.join();
        }
        if(progressThread.joinable()) {
            progressThread.join();
        }
    }

    void downloadSmm();
    void progressDownload();

    std::thread downloadThread;
    std::thread progressThread;
    std::mutex threadMutex;
    std::condition_variable threadCondition;

    bool downloadFinished = false;

    static brls::View* create() {
        return new InstallationView;
    }
private:
    BRLS_BIND(brls::Label, smm_desc, "smm_desc");
    BRLS_BIND(brls::RadioCell, smm_radio, "smm_button");

    brls::Slider* smm_progressBar;
    brls::Label* smm_percent;
};