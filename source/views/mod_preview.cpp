#include "views/mod_preview.hpp"
#include "views/download_view.hpp"
#include "utils/utils.hpp"
#include "utils/progress_event.hpp"

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
    {
        std::unique_lock<std::mutex> lock(threadMutex);
        if(stopThreadFlag)
            return;
    } 


    std::vector<unsigned char> buffer;

    for(auto i = 0; i < this->mod.getImagesUrl().size(); i++) {
        if(stopThreadFlag)
            return;

        if(i == 0)
            buffer = this->mod.downloadImage(i);
        else    
            this->mod.downloadImage(i);
            
        //Used to push on main thread because borealis isn't thread safe
        ASYNC_RETAIN
        brls::sync([ASYNC_TOKEN, i, &buffer]() mutable {
            ASYNC_RELEASE
            this->mod.loadImage(i);
            if(i == 0) {
                bigImg->setImageFromMem(buffer.data(), buffer.size());
                bigImg->setHeight(298*1.5);
                bigImg->setWidth(500*1.5);
                bigImg->setCornerRadius(10);
                bigImg->setScalingType(brls::ImageScalingType::FILL);
                this->big_image_box->clearViews();
                this->big_image_box->addView(bigImg);
                this->firstImageDownloaded = true;
                loadButtons();
            }

            auto img = this->mod.getImage(i);
            if (img == nullptr) {
                brls::Logger::error("Image is null");
                return;
            }
            img->setHeight(56);
            img->setWidth(100);
            img->setCornerRadius(10);
            img->setScalingType(brls::ImageScalingType::FILL);
            img->setFocusable(true);

            img->registerClickAction(brls::ActionListener([this, i](brls::View* view) {
                bigImg->setImageFromMem(this->mod.getImageBuffer(i).data(), this->mod.getImageBuffer(i).size());
                return true;
            }));


            this->small_image_box->addView(img);
        });


    }
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
    if(secondThread.joinable())
        secondThread.join();
}