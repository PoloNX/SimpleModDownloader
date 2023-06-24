#pragma once 

#include <string>
#include <vector>
#include <borealis.hpp>
#include <chrono>

#include <SDL2/SDL_mixer.h>
class AudioPreview : public brls::View {
    public:
        AudioPreview(std::vector<unsigned char>& buffer);
        ~AudioPreview();	
        brls::View* getDefaultFocus() override;
        brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView) override;
        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash);
    private:
        brls::NavigationMap navigationMap;

        brls::Button* playButton;
        brls::Button* stopButton;
        brls::ProgressDisplay* progressDisplay;

        void play();
        void stop();

        Mix_Music* music;
        int volume;

        bool paused = true;

        std::chrono::_V2::steady_clock::time_point start;
        std::chrono::_V2::steady_clock::time_point end;
        std::chrono::duration<float> currentTime;

        float totalTime;
};