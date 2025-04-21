#include "views/mod_preview.hpp"
#include "views/download_view.hpp"
#include "views/spinner_image_view.hpp"
#include "utils/utils.hpp"
#include "utils/progress_event.hpp"

#include <future>

using namespace brls::literals;

FileBox::FileBox(File& file) {
    this->inflateFromXMLRes("xml/cells/file_cell.xml");
    this->setFocusable(false);
    title->setText(file.getName());
    date->setText(fmt::format("{} : {}", "menu/mods/date"_i18n, utils::timestamp_to_date(static_cast<time_t>(file.getDate()))));
    size->setText(fmt::format("{} : {}", "menu/mods/size"_i18n, utils::file_size_to_string(file.getSize())));
    download->setText("menu/mods/download"_i18n);
    download->setFocusable(true);
}

ModPreview::ModPreview(Mod& mod, std::vector<unsigned char>& bannerBuffer): mod(mod) {
    this->mod.loadMod();

    this->inflateFromXMLRes("xml/tabs/mod_preview.xml");

    this->setFocusable(false);
    scrolling->setFocusable(true);
    
    banner->setImageFromMem(bannerBuffer.data(), bannerBuffer.size());
    banner->setHeight(200);
    
    image_overlay->setColor(nvgRGBA(100, 100, 100, 0.8*255));
    title->setText(this->mod.getName());
    author->setText(fmt::format("{} : {}", "menu/mods/author"_i18n, this->mod.getAuthor()));
    description->setText(this->mod.getDescription());

    secondThread = std::thread(&ModPreview::loadImages, this);

    brls::sync([this] {
        getAppletFrame()->setHeaderVisibility(brls::Visibility::GONE);
    });

    this->registerAction("back", brls::ControllerButton::BUTTON_B, [this](brls::View* view) {
        brls::sync([this]{ getAppletFrame()->setHeaderVisibility(brls::Visibility::VISIBLE); });
        this->dismiss();
        return true;
    }, true);
}

void ModPreview::loadButtons() {
    //Don't give the focus in the constructor of ModPreview() because it will break the focus on the scroller. If you have a better option please tell me
    for(auto file : this->mod.getFiles()) {
        auto fileBox = new FileBox(file);
        fileBox->getDownloadButton()->registerClickAction(brls::ActionListener([file = std::move(file), this](brls::View* view) mutable {
            brls::Logger::debug("File clicked : {}", file.getName());
            file.loadFile();
            //Smash tid
            if(file.getRomfs() || file.getGame().getTid() == "01006A800016E000") {  
                brls::sync([this] {
                    getAppletFrame()->setHeaderVisibility(brls::Visibility::VISIBLE);
                });
                ProgressEvent::instance().setInterupt(true);
                this->present(new DownloadView(file));
                this->stopThreadFlag = true;
            }  
            else {
                auto dialog = new brls::Dialog("menu/notify/mod_unsupported"_i18n);
                dialog->addButton("hints/ok"_i18n, []() {});
                dialog->open();
            } 
            return true;
        }));
        files_box->addView(fileBox);
    }
}

void ModPreview::loadImages() {
    if (shouldStopThread()) {
        brls::Logger::debug("Thread stopped before starting.");
        return;
    }

    std::vector<SpinnerImageView*> spinnerViews;

    // add spinners to the boxes
    std::promise<void> spinnersAddedPromise;
    auto spinnersAddedFuture = spinnersAddedPromise.get_future();

    brls::sync([this, &spinnerViews, &spinnersAddedPromise]() {
        brls::Logger::debug("Adding spinners...");
        size_t imageCount = this->mod.getImagesUrl().size();
        brls::Logger::debug("Number of image URLs: {}", imageCount);

        for (size_t i = 0; i < std::min(imageCount, static_cast<size_t>(24)); i++) {
            if (shouldStopThread()) {
                brls::Logger::debug("Thread stopped while adding spinners.");
                return;
            }

            auto spinnerImageView = new SpinnerImageView(bigImageWidth/8, bigImageWidth/8 * 9/16);
            spinnerViews.push_back(spinnerImageView);

            if (i < 8) {
                small_image_box_1->addView(spinnerImageView);
            } else if (i < 16) {
                small_image_box_2->addView(spinnerImageView);
            } else {
                small_image_box_3->addView(spinnerImageView);
            }
        }

        big_image_box->addView(bigSpinImg);
        brls::Logger::debug("Spinners added.");
        spinnersAddedPromise.set_value();
    });

    // wait for the spinners to be added before downloading images
    spinnersAddedFuture.wait();

    brls::Logger::debug("SpinnerViews size: {}", spinnerViews.size());

    for (size_t i = 0; i < spinnerViews.size(); i++) {
        if (shouldStopThread()) {
            brls::Logger::debug("Thread stopped during image download.");
            return;
        }

        std::vector<unsigned char> buffer;

        brls::Logger::debug("Downloading image {}", i);
        buffer = this->mod.downloadImage(i);
        brls::Logger::debug("Downloaded image {} with size {}", i, buffer.size());

        if (shouldStopThread()) {
            brls::Logger::debug("Thread stopped after downloading image {}.", i);
            return;
        }

        brls::sync([this, spinnerView = spinnerViews[i], buffer, i]() mutable {
            if (shouldStopThread()) {
                brls::Logger::debug("Thread stopped while updating UI for image {}.", i);
                return;
            }

            // if it's the first image, set it to the big image
            if (i == 0) {
                bigSpinImg->setImage(buffer);
            }

            brls::Logger::debug("Replacing spinner with image {}", i);
            spinnerView->setImage(buffer);
            spinnerView->registerClickAction(brls::ActionListener([this, buffer](brls::View* view) {
                this->bigSpinImg->setImage(buffer);
                return true;
            }));
        });
    }
}

bool ModPreview::shouldStopThread() {
    std::unique_lock<std::mutex> lock(threadMutex);
    return stopThreadFlag || isExiting;
}

void ModPreview::stopThread() {
    {
        std::unique_lock<std::mutex> lock(threadMutex);
        stopThreadFlag = true;
    }
    threadCondition.notify_one();
}

ModPreview::~ModPreview() {
    stopThread();
    {
        std::unique_lock<std::mutex> lock(threadMutex);
        isExiting = true;
        stopThreadFlag = true;
    }
    threadCondition.notify_one();
    if (secondThread.joinable())
        secondThread.join();
}