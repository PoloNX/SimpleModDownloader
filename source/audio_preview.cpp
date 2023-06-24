#include "audio_preview.hpp"

#include <stdexcept>
#include <SDL2/SDL_rwops.h>
#include <iostream>

constexpr int BUTTON_WIDTH = 200;
constexpr int BUTTON_HEIGHT = 100;
constexpr int PADDING = 75;

AudioPreview::AudioPreview() {
    this->play = new brls::Button(brls::ButtonStyle::REGULAR);
    this->play->setLabel("play");
    this->addView(this->play);

    this->stop = new brls::Button(brls::ButtonStyle::REGULAR);
    this->stop->setLabel("stop");
    this->addView(this->stop);

    this->navigationMap.add(
        this->play,
        brls::FocusDirection::RIGHT,
        this->stop
    );

    this->navigationMap.add(
        this->stop,
        brls::FocusDirection::LEFT,
        this->play
    );
}

void AudioPreview::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    int x = this->getX();
    int y = this->getY();

    // Fully custom layout
    this->play->setBoundaries(
        x + PADDING,
        y + PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT);

    this->stop->setBoundaries(
        x + PADDING * 2 + BUTTON_WIDTH,
        y + PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT);
}

brls::View* AudioPreview::getDefaultFocus()
{
    return this->play->getDefaultFocus();
}

brls::View* AudioPreview::getNextFocus(brls::FocusDirection direction, brls::View* currentView)
{
    return this->navigationMap.getNextFocus(direction, currentView);
}