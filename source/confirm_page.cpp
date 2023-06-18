#include "confirm_page.hpp"
#include "main_frame.hpp"
#include "utils.hpp"
#include "constants.hpp"
#include <switch.h>

namespace i18n = brls::i18n;
using namespace i18n::literals;

ConfirmPage::ConfirmPage(brls::StagedAppletFrame* frame, const std::string& text, const bool& restart) : frame(frame), text(text) {
    this->button = new brls::Button(brls::ButtonStyle::REGULAR);
    this->button->setLabel("brls/hints/ok"_i18n);
    this->button->setParent(this);

    this->label = new brls::Label(brls::LabelStyle::DIALOG, this->text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);

    this->button->getClickEvent()->subscribe([this, restart](brls::View* view){
        if(restart) {
            utils::cp("romfs:/forwarder/forwarder.nro", "sdmc:/config/SimpleModDownloader/forwarder.nro");
            envSetNextLoad(FORWARDER_PATH.c_str(), fmt::format("\"{}\"", FORWARDER_PATH).c_str());
            romfsExit();
            brls::Application::quit();
        }
        else if(!this->frame->isLastStage()) {
            this->frame->nextStage();
        }
        else {
            brls::Application::pushView(new MainFrame());
        }
    });

    this->registerAction("", brls::Key::B, [this] { return true; });
}

void ConfirmPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
    this->button->setState(brls::ButtonState::ENABLED);

    this->label->frame(ctx);
    this->button->frame(ctx);
}

brls::View* ConfirmPage::getDefaultFocus() {
    return this->button;
}

void ConfirmPage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
    this->label->setWidth(this->width);
    this->label->invalidate(true);

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight()
    );

    this->button->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        this->y + (this->height - style->CrashFrame.buttonHeight *3), 
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight
    );
    this->button->invalidate(false);
}

ConfirmPage::~ConfirmPage()
{
    delete this->label;
    delete this->button;
}