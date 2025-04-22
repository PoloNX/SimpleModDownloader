#include "views/spinner_image_view.hpp"

SpinnerImageView::SpinnerImageView(const int& width, const int& height, const int& margin) : width(width), height(height) {
    this->spinner = new brls::ProgressSpinner();
    this->image = new brls::Image();

    this->spinner->setWidth(width);
    this->spinner->setHeight(height);
    this->spinner->animate(true);

    this->image->setWidth(width);
    this->image->setHeight(height);
    this->image->setScalingType(brls::ImageScalingType::FILL);
    this->image->setCornerRadius(10);

    this->setMarginLeft(margin);
    this->setMarginRight(margin);
    this->setFocusable(true);

    this->addView(this->spinner);

    image->setVisibility(brls::Visibility::INVISIBLE);
}

void SpinnerImageView::setImage(const std::vector<unsigned char>& buffer) {
   
    this->image->setImageFromMem(buffer.data(), buffer.size());

    if (!imageLoaded) {
        this->image->setVisibility(brls::Visibility::VISIBLE);
        this->spinner->setVisibility(brls::Visibility::INVISIBLE);
        this->clearViews();
        this->addView(this->image);
    }
   

    imageLoaded = true;
}