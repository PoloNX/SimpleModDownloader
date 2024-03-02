#include "views/installation_view.hpp"
#include "api/net.hpp"
#include "utils/progress_event.hpp"

#include <filesystem>
#include <switch.h>

InstallationView::InstallationView() {
    this->inflateFromXMLRes("xml/tabs/installation_tab.xml");

    smm_desc->setText("SimpleModDownloader allow you to switch to SimpleModManager, a mod manager for the Nintendo Switch. It will allow you to install mods.");

    smm_radio->setSelected(false);

    std::array<std::filesystem::path, 2> smm_nro_path = {
        "sdmc:/switch/SimpleModManager.nro",
        "sdmc:/switch/SimpleModManager/SimpleModManager.nro"
    };
    
    std::string smm_path;
    for (const auto& path : smm_nro_path) {
        if (std::filesystem::exists(path)) {
            smm_path = path.string();
            break;
        }
    }

    if(!smm_path.empty()) {
        smm_radio->title->setText("Launch SimpleModManager");
        smm_radio->registerClickAction([this, smm_path = std::move(smm_path)](brls::View* view) {
            brls::Logger::debug("Launching SimpleModManager, path: {}", smm_path);
            envSetNextLoad(smm_path.c_str(), fmt::format("\"{}\"", smm_path).c_str());
            brls::Application::quit();
            return true;
        });
    } 
    else {
        smm_radio->title->setText("Download SimpleModManager");
        smm_radio->registerClickAction([this](brls::View* view) {
            brls::sync([this]{getAppletFrame()->setActionsAvailable(false);});
            downloadThread = std::thread(&InstallationView::downloadSmm, this);
            progressThread = std::thread(&InstallationView::progressDownload, this);
            return true;
        });
    }
}

void InstallationView::downloadSmm() {
    {
        std::unique_lock<std::mutex> lock(threadMutex);
    }
    const std::string SimpleModManager_URL = "https://github.com/nadrino/SimpleModManager/releases/latest/download/SimpleModManager.nro";
    net::downloadFile(SimpleModManager_URL, "sdmc:/switch/SimpleModManager.nro");
    downloadFinished = true;
}

void InstallationView::progressDownload() {
    {
        std::unique_lock<std::mutex> lock(threadMutex);
    }
    {
        ASYNC_RETAIN
        brls::sync([ASYNC_TOKEN]() {
            ASYNC_RELEASE
            this->smm_progressBar = new brls::Slider();
            this->smm_progressBar->hidePointer();
            this->smm_progressBar->setProgress(0);
            this->smm_progressBar->setWidth(800);
            this->smm_progressBar->setHeight(20);
            this->smm_percent = new brls::Label();
            this->smm_percent->setText(fmt::format("{}%", 0));
            this->addView(this->smm_progressBar);
            this->addView(this->smm_percent);
        });
    }
    while(ProgressEvent::instance().getTotal() == 0) {
        if(downloadFinished)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while(ProgressEvent::instance().getNow() < ProgressEvent::instance().getTotal() || !downloadFinished) {
        ASYNC_RETAIN
        brls::sync([ASYNC_TOKEN](){
            ASYNC_RELEASE
            smm_progressBar->setProgress(ProgressEvent::instance().getNow() / ProgressEvent::instance().getTotal());
            smm_percent->setText(fmt::format("{}%", (int)((ProgressEvent::instance().getNow() * 100) / ProgressEvent::instance().getTotal())));
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASYNC_RETAIN
    brls::sync([ASYNC_TOKEN](){
        ASYNC_RELEASE
        this->removeView(smm_progressBar);
        this->removeView(smm_percent);
        this->smm_radio->title->setText("Launch SimpleModManager");
        this->smm_radio->registerClickAction([this](brls::View* view) {
            envSetNextLoad("sdmc:/switch/SimpleModManager.nro", "\"sdmc:/switch/SimpleModManager.nro\"");
            brls::Application::quit();
            return true;
        });
        brls::Application::giveFocus(this->smm_radio);
        getAppletFrame()->setActionsAvailable(true);
    });
    
}
