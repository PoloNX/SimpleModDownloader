#include "views/download_view.hpp"
#include "api/net.hpp"
#include "api/extract.hpp"
#include "utils/progress_event.hpp"
#include "utils/utils.hpp"

#include <regex>

using namespace brls::literals;

DownloadView::DownloadView(File& file): file(file) {
    this->inflateFromXMLRes("xml/tabs/download_view.xml");

    ProgressEvent::instance().reset();


    extract_progressBar->hidePointer();
    extract_progressBar->setProgress(0);
    download_progressBar->hidePointer();
    download_progressBar->setProgress(0);
    download_percent->setText(fmt::format("{}%", 0));
    extract_percent->setText(fmt::format("{}%", 0));

    download_text->setText(fmt::format("Downloading : {}", file.getName()));
    extract_text->setText(fmt::format("Extracting {} to {}", file.getName(), "mods/JEUX/" + file.getName()));

    this->setActionAvailable(brls::ControllerButton::BUTTON_B, false);

    this->setFocusable(true);
    this->setHideHighlightBackground(true);
    this->setHideHighlightBorder(true);

    downloadThread = std::thread(&DownloadView::downloadFile, this);
    updateThread = std::thread(&DownloadView::updateProgress, this);

    brls::sync([this]() {getAppletFrame()->setActionAvailable(brls::ControllerButton::BUTTON_B, false);});
    
}

void DownloadView::downloadFile() {
    {
        std::unique_lock<std::mutex> lock(threadMutex);
    }
    net::downloadFile(this->file.getUrl(), this->file.getPath());
    this->downloadFinished = true;

    ProgressEvent::instance().reset();

    //Prevent incorrect chars in the path
    std::regex badChars("[:/|*]");
    extract::extractEntry(this->file.getPath(), fmt::format("sdmc:/{}/{}/{}/contents/{}/romfs", utils::getModInstallPath(), std::regex_replace(this->file.getGame().getTitle(), badChars, "-"), std::regex_replace(this->file.getModName(), badChars, "-"), std::regex_replace(this->file.getGame().getTid(), badChars, "-")), this->file.getGame().getTid());
}

void DownloadView::updateProgress() {
    {
        std::unique_lock<std::mutex> lock(threadMutex);
    }
    //DOWNLOAD
    {
        while(ProgressEvent::instance().getTotal() == 0) {
            if(downloadFinished)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        while(ProgressEvent::instance().getNow() < ProgressEvent::instance().getTotal() && !downloadFinished) {
            ASYNC_RETAIN
            brls::sync([ASYNC_TOKEN](){
                ASYNC_RELEASE
                this->download_percent->setText(fmt::format("{}%", (int)(ProgressEvent::instance().getNow() / ProgressEvent::instance().getTotal() * 100)));
                this->download_progressBar->setProgress((float)ProgressEvent::instance().getNow() / ProgressEvent::instance().getTotal());
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        ASYNC_RETAIN
        brls::sync([ASYNC_TOKEN](){
            ASYNC_RELEASE
            this->download_percent->setText("100%");
            this->download_progressBar->setProgress(1);
            this->download_spinner->animate(false);
        });
    }
    //EXTRACT
    {
        while(ProgressEvent::instance().getMax() == 0) {
            if(extractFinished)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while(ProgressEvent::instance().getStep() < ProgressEvent::instance().getMax() && !extractFinished) {
            ASYNC_RETAIN
            brls::sync([ASYNC_TOKEN](){
                ASYNC_RELEASE
                this->extract_percent->setText(fmt::format("{}%", (int)((ProgressEvent::instance().getStep() / ProgressEvent::instance().getMax()) * 100)));
                this->extract_progressBar->setProgress((float)ProgressEvent::instance().getStep() / ProgressEvent::instance().getMax());
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        ASYNC_RETAIN
        brls::sync([ASYNC_TOKEN](){
            ASYNC_RELEASE
            this->extract_percent->setText("100%");
            this->extract_progressBar->setProgress(1);
            this->extract_spinner->animate(false);
        });
    }

    //Add a button to go back after the end of the download
    ASYNC_RETAIN
    brls::sync([ASYNC_TOKEN]() {
        ASYNC_RELEASE
        auto button = new brls::Button();
        button->setText("Back");
        button->setFocusable(true);
        button->registerClickAction(brls::ActionListener([this](brls::View* view) {
            this->dismiss();
            return true;
        }));
        this->addView(button);
        brls::Application::giveFocus(button);
        getAppletFrame()->setActionAvailable(brls::ControllerButton::BUTTON_B, true);
    });
}

