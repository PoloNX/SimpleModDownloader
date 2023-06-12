#include "images_viewer.hpp"

constexpr int BIG_IMAGE_WIDTH = 854;
constexpr int BIG_IMAGE_HEIGHT = 480;

constexpr int IMAGES_VIEWER_WIDTH = 100;
constexpr int IMAGES_VIEWER_HEIGHT = 56;
constexpr int IMAGES_VIEWER_PADDING = 4;

ImagesViewer::ImagesViewer(Mod& mod, Game& game): currentMod(mod), currentGame(game) {
    init();
}

ImagesViewer::ImagesViewer(Mod &mod, Game& game, const std::string& search): currentMod(mod), currentGame(game), search(search) {
    init();
}

void ImagesViewer::init() {
    brls::Logger::info("Big page is {}", this->currentMod.currentBigImage);

    this->updateActionHint(brls::Key::B, "Back");
    
    initItems();

    right->getClickEvent()->subscribe([this](brls::View* view){
        if (this->currentMod.currentBigImage < images.size() - 1) {
            this->currentMod.currentBigImage++;
            this->currentMod.images.clear();
            this->currentMod.images = utils::getModsImages(this->currentMod.json, this->currentMod.currentBigImage, this->currentMod.sizeBigImage);
            brls::Application::pushView(new ImagesViewer(this->currentMod, this->currentGame));
        }
    });

    left->getClickEvent()->subscribe([this](brls::View* view){
        if (this->currentMod.currentBigImage > 1) {
            this->currentMod.currentBigImage--;
            this->currentMod.images.clear();
            this->currentMod.images = utils::getModsImages(this->currentMod.json, this->currentMod.currentBigImage, this->currentMod.sizeBigImage);
            brls::Application::pushView(new ImagesViewer(this->currentMod, this->currentGame));
        }
    });

    this->navigationMap.add(this->right, brls::FocusDirection::LEFT, this->left);
    this->navigationMap.add(this->left, brls::FocusDirection::RIGHT, this->right);

    this->registerAction("", brls::Key::B, [this] { 
        if (this->search!="")  
            brls::Application::pushView(new ModsList(this->currentGame, this->search));
        else 
            brls::Application::pushView(new ModsList(this->currentGame));
        return 0;
    });
}


void ImagesViewer::initItems() {
    for (int i = 0; i < this->currentMod.images.size(); i++) {
        this->images.push_back(this->currentMod.images[i]);
    }

    for (auto i : images) {
        this->addView(i);
    }

    this->right = new brls::Button(brls::ButtonStyle::REGULAR);
    this->right->setLabel(">");
    this->addView(this->right);

    this->left = new brls::Button(brls::ButtonStyle::REGULAR);
    this->left->setLabel("<");
    this->addView(this->left);
}

void ImagesViewer::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
    images[this->currentMod.currentBigImage]->setWidth(BIG_IMAGE_WIDTH);
    images[this->currentMod.currentBigImage]->setHeight(BIG_IMAGE_HEIGHT);

    brls::Image* currentBigImage = new brls::Image(images[this->currentMod.currentBigImage]->copyImgBuf(), this->currentMod.sizeBigImage);
    this->addView(currentBigImage);
    currentBigImage->setWidth(BIG_IMAGE_WIDTH);
    currentBigImage->setHeight(BIG_IMAGE_HEIGHT);
    currentBigImage->setBoundaries(
        this->getWidth() / 2 - BIG_IMAGE_WIDTH / 2,
        IMAGES_VIEWER_PADDING * 10,
        BIG_IMAGE_WIDTH,
        BIG_IMAGE_HEIGHT
    );

    int numImages = this->images.size();

    int totalWidth = (IMAGES_VIEWER_WIDTH + IMAGES_VIEWER_PADDING) * (numImages) + IMAGES_VIEWER_WIDTH;

    // Calculer la position de départ pour centrer les éléments avec une marge supplémentaire
    int margin = (this->getWidth() - totalWidth) / 2;
    int startX = margin;

    for (int i = 0; i < this->images.size(); i++) {
        if (this->images[i] != nullptr) {
            images[i]->setWidth(IMAGES_VIEWER_WIDTH);
            images[i]->setHeight(IMAGES_VIEWER_HEIGHT);
            this->images[i]->setBoundaries(
                startX + (IMAGES_VIEWER_WIDTH + IMAGES_VIEWER_PADDING) * i + 0.5 * IMAGES_VIEWER_WIDTH,
                BIG_IMAGE_HEIGHT + IMAGES_VIEWER_PADDING * 15, 
                IMAGES_VIEWER_WIDTH,
                IMAGES_VIEWER_HEIGHT
            );
        }
    }

    // Positionner l'image de droite
    this->right->setBoundaries(
        startX + (IMAGES_VIEWER_WIDTH + IMAGES_VIEWER_PADDING) * (numImages) + 0.5 * IMAGES_VIEWER_WIDTH,
        BIG_IMAGE_HEIGHT + IMAGES_VIEWER_PADDING * 15,
        IMAGES_VIEWER_WIDTH,
        IMAGES_VIEWER_HEIGHT
    );

    // Positionner l'image de gauche
    this->left->setBoundaries(
        startX - IMAGES_VIEWER_WIDTH - IMAGES_VIEWER_PADDING + 0.5 * IMAGES_VIEWER_WIDTH,
        BIG_IMAGE_HEIGHT + IMAGES_VIEWER_PADDING * 15,
        IMAGES_VIEWER_WIDTH,
        IMAGES_VIEWER_HEIGHT
    );
}

brls::View* ImagesViewer::getDefaultFocus()
{
    return this->right->getDefaultFocus();
}

brls::View* ImagesViewer::getNextFocus(brls::FocusDirection direction, brls::View* currentView)
{
    return this->navigationMap.getNextFocus(direction, currentView);
}
