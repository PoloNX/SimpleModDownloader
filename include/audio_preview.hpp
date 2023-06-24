#pragma once 

#include <string>
#include <vector>
#include <borealis.hpp>
#include <SDL2/SDL_mixer.h>

class AudioPreview : public brls::AbsoluteLayout {
    public:
        AudioPreview();

        brls::View* getDefaultFocus() override;
        brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView) override;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash);
    private:
        brls::NavigationMap navigationMap;

        brls::Button* play;
        brls::Button* stop;
};