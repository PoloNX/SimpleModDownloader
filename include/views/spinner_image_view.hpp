#pragma once

#include <borealis.hpp>

class SpinnerImageView : public brls::Box {
private:
    brls::ProgressSpinner* spinner;
    brls::Image* image;

    bool isImageLoaded = false;

    int width;
    int height;

public:
    SpinnerImageView(const int& width, const int& height);
    void setImage(const std::vector<unsigned char>& buffer);
};