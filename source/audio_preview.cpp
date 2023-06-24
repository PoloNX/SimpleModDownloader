#include "audio_preview.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <chrono>

#include <mpg123.h>

constexpr int BUTTON_WIDTH = 200;
constexpr int BUTTON_HEIGHT = 100;
constexpr int PADDING = 75;

AudioPreview::AudioPreview(std::vector<unsigned char>& buffer) {
    this->playButton = new brls::Button(brls::ButtonStyle::REGULAR);
    this->playButton->setLabel("play");

    playButton->getClickEvent()->subscribe([this](brls::View* view){
        this->play();
    });
    
    this->playButton->setParent(this);


    this->stopButton = new brls::Button(brls::ButtonStyle::REGULAR);
    this->stopButton->setLabel("stop");

    stopButton->getClickEvent()->subscribe([this](brls::View* view){
        this->stop();
    });

    this->stopButton->setParent(this);

    this->navigationMap.add(
        this->playButton,
        brls::FocusDirection::RIGHT,
        this->stopButton
    );

    this->navigationMap.add(
        this->stopButton,
        brls::FocusDirection::LEFT,
        this->playButton
    );


    std::string tempFilePath = "sdmc:/config/SimpleModDownloader/temp.mp3";
    std::ofstream tempFile(tempFilePath, std::ios::binary);
    tempFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    tempFile.close();

    Mix_Init(MIX_INIT_MP3);

    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
    music = Mix_LoadMUS(tempFilePath.c_str());

    mpg123_init();
    mpg123_handle* handle = mpg123_new(nullptr, nullptr);
    mpg123_open(handle, tempFilePath.c_str());

    int channel, encoding;
    long rate;

    mpg123_getformat(handle, &rate, &channel, &encoding);

    off_t totalFrames = mpg123_length(handle);
    totalTime = (float)totalFrames/rate;

    mpg123_close(handle);
    mpg123_delete(handle);
    mpg123_exit();

    this->progressDisplay = new brls::ProgressDisplay(brls::PERCENTAGE_ONLY_PROGRESS_DISPLAY_FLAGS);
    this->progressDisplay->setParent(this);
}

void AudioPreview::play() {
    if(music != nullptr) {
        start = std::chrono::steady_clock::now();
        Mix_PlayMusic(music, 1);
        paused = false;
    }       
    else    
        brls::Logger::info("music is null");
}

void AudioPreview::stop() {
    Mix_HaltMusic();
    paused = true;
}

void AudioPreview::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
    this->playButton->setState(brls::ButtonState::ENABLED);

    this->playButton->frame(ctx);
    this->stopButton->frame(ctx);

    if (!paused)
        end = std::chrono::steady_clock::now();

    currentTime = end - this->start;

    brls::Logger::debug("currentTime : {}", currentTime.count());

    this->progressDisplay->setProgress(currentTime.count(), totalTime);
    this->progressDisplay->frame(ctx);

}

void AudioPreview::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    int x = this->getX();
    int y = this->getY();

    // Fully custom layout
    this->playButton->setBoundaries(
        x + PADDING,
        y + PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT);

    this->stopButton->setBoundaries(
        x + PADDING * 2 + BUTTON_WIDTH,
        y + PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT);

    this->progressDisplay->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth,
        this->y + this->height / 2,
        style->CrashFrame.buttonWidth * 2,
        style->CrashFrame.buttonHeight);
}

brls::View* AudioPreview::getDefaultFocus()
{
    return this->playButton->getDefaultFocus();
}

brls::View* AudioPreview::getNextFocus(brls::FocusDirection direction, brls::View* currentView)
{
    return this->navigationMap.getNextFocus(direction, currentView);
}

AudioPreview::~AudioPreview() {
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();
}