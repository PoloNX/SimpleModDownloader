#include "audio_preview.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>




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

    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        // Gestion de l'erreur en cas d'échec de l'ouverture du périphérique audio
        brls::Logger::error("Failed to open audio device");
        return;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
        // Gestion de l'erreur en cas d'échec de la création du contexte audio
        brls::Logger::error("Failed to create audio context");
        alcCloseDevice(device);
        return;
    }

    if (!alcMakeContextCurrent(context)) {
        // Gestion de l'erreur en cas d'échec de la définition du contexte audio actuel
        brls::Logger::error("Failed to make audio context current");
        alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR) {
        // Gestion de l'erreur en cas d'erreur OpenAL
        brls::Logger::error("OpenAL error: {}", alGetString(alError));
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    ALuint bufferID;
    alGenBuffers(1, &bufferID);
    alBufferData(bufferID, AL_FORMAT_MONO16, buffer.data(), buffer.size(), 44100);

    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, bufferID);

    this->progressDisplay = new brls::ProgressDisplay();
    this->progressDisplay->setParent(this);
}

void AudioPreview::play() {
    alSourcePlay(this->sourceID);
    //brls::Logger::debug("Failed to play audio");
}

void AudioPreview::stop() {

}

void AudioPreview::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
    this->playButton->setState(brls::ButtonState::ENABLED);

    this->playButton->frame(ctx);
    this->stopButton->frame(ctx);

    brls::Logger::debug("currentTime : {}", currentTime);

    this->progressDisplay->setProgress(currentTime, 10);
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

}