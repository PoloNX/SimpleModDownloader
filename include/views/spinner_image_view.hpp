#pragma once

#include <borealis.hpp>

class SpinnerImageView : public brls::Box {
private:
    brls::ProgressSpinner* spinner;
    brls::Image* image;

    bool imageLoaded = false;

    int width;
    int height;

public:
    SpinnerImageView(const int& width, const int& height, const int& margin = 0);
    void setImage(const std::vector<unsigned char>& buffer);

    bool isImageLoaded() {
        return imageLoaded;
    }
};