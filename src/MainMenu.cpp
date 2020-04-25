#include "MainMenu.h"

#include "Util.h"

/* Filepaths for external resources */
static const char BACKGROUND_IMG_PATH[] = "MainMenu/desert-background.png";
static const char TITLE_IMG_PATH[] = "MainMenu/main-title.png";
static const char ZOOMSELECTOR_IMG_PATH[] = "MainMenu/zoomselector.png";

/* Background pan animation parameters */
static const float BG_MAX_SPEED = 0.02;
static const float BG_MIN_SPEED = 0.005;
static const float BG_SPEED_CUTOFF = 0.05;

/* Main title positioning parameters */
static const float TITLE_TOP_PADDING_PCT = 0.09;
static const float TITLE_HEIGHT_PCT = 0.275;

/* Timing parameters for initial fade-in animation */
static const float INIT_FADE_START_TIME = 0.5;
static const float INIT_FADE_END_TIME = 1.5;

/* Timing parameters for transition fade between submenus */
static const float TRANS_FADE_OUT_END_TIME = 0.15;
static const float TRANS_FADE_IN_END_TIME = 0.3;

/* Parameters common to all menu ZoomSelectors */
static const float ZS_HOVER_RATIO = 1.75;
static const float ZS_ITEM_PAD_PCT = 0.05;

/* Toplevel zoom selector parameters */
static const float TOPLEVEL_ZS_TOP_POS = 0.435;
static const float TOPLEVEL_ZS_BOT_POS = 0.93;

/* Settings submenu zoom selector parameters */
static const float SETTINGS_ZS_TOP_POS = 0.65;
static const float SETTINGS_ZS_BOT_POS = 0.8;

/* Definitions for positions of particular text entries on the ZoomSelector texture */
static const int ZS_TEXT_CENTER_Y = 60;
static const raylib::Rectangle ZS_TEXT_PLAY_LOCAL_GAME(0, 0, 894, 137);
static const raylib::Rectangle ZS_TEXT_PLAY_ONLINE(0, 137, 616, 137);
static const raylib::Rectangle ZS_TEXT_SETTINGS(0, 274, 445, 137);
static const raylib::Rectangle ZS_TEXT_EXIT(0, 411, 189, 137);

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

MainMenu::MainMenu()
{
    /* Initialize some generic fields */
    bgSrcXPercent = 0;
    bgSrcXPosIncreasing = true;
    titleOpacity = 0;
    fadeStopwatch = 0;
    currentState = State::INITIAL_FADE;
    nextReturnCode = ReturnCode::KEEP_RUNNING;

    /* Load textures for background and main title text */
    bgTexture = new raylib::Texture(Util::formResourcePath(BACKGROUND_IMG_PATH));
    bgTexture->GenMipmaps();
    titleTexture = new raylib::Texture(Util::formResourcePath(TITLE_IMG_PATH));
    titleTexture->GenMipmaps();

    /* Initialize top level menu zoom selector */
    toplevelZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    toplevelZoomSel->addItem(ZS_TEXT_PLAY_LOCAL_GAME, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_PLAY_ONLINE, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_SETTINGS, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_EXIT, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    toplevelZoomSel->setDependentOpacity(0);

    /* Initialize settings submenu zoom selector */
    settingsZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    settingsZoomSel->addItem(ZS_TEXT_EXIT, ZS_TEXT_CENTER_Y);
    settingsZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    settingsZoomSel->setDependentOpacity(0);
}

MainMenu::~MainMenu()
{
    delete bgTexture;
    delete titleTexture;
    delete toplevelZoomSel;
    delete settingsZoomSel;
}   

void MainMenu::update(double secs)
{
    /* Update velocity of background */
    float bgVelocity = bgSrcXPosIncreasing ? 1 : -1;
    if (bgSrcXPercent < BG_SPEED_CUTOFF) {
        /* Slowing down with x pos percent close to 0 */
        bgVelocity *= BG_MIN_SPEED + (bgSrcXPercent / BG_SPEED_CUTOFF) *
                (BG_MAX_SPEED - BG_MIN_SPEED);
    } else if (bgSrcXPercent > (1 - BG_SPEED_CUTOFF)) {
        /* Slowing down with x pos percent close to 1 */
        bgVelocity *= BG_MIN_SPEED + ((1 - bgSrcXPercent) / BG_SPEED_CUTOFF) *
                (BG_MAX_SPEED - BG_MIN_SPEED);
    } else {
        /* Going full speed between the upper and lower cutoffs */
        bgVelocity *= BG_MAX_SPEED;
    }

    /* Update position of background */
    bgSrcXPercent += bgVelocity * secs;
    if (bgSrcXPercent > 1) {
        bgSrcXPercent = 1;
        bgSrcXPosIncreasing = false;
    } else if (bgSrcXPercent < 0) {
        bgSrcXPercent = 0;
        bgSrcXPosIncreasing = true;
    }
    bgSrcXPos = bgSrcXMax * bgSrcXPercent;

    switch (currentState) {

    case State::INITIAL_FADE:
        /* Update opacity of main title for fade in */
        fadeStopwatch += secs;
        if (fadeStopwatch >= INIT_FADE_END_TIME) {
            titleOpacity = 1;
            setOpacityForState(State::SHOW_TOPLEVEL, 1);
            currentState = State::SHOW_TOPLEVEL;
        } else if (fadeStopwatch > INIT_FADE_START_TIME) {
            float opacity = (fadeStopwatch - INIT_FADE_START_TIME) /
                    (INIT_FADE_END_TIME - INIT_FADE_START_TIME);
            titleOpacity = opacity;
            setOpacityForState(State::SHOW_TOPLEVEL, opacity);
        }
        updateForState(State::SHOW_TOPLEVEL, secs);
        break;

    case State::FADE_TRANSITION:
        /* Update opacities of fade-from and fade-to submenu states */        
        float opacity;
        fadeStopwatch += secs;
        if (fadeStopwatch < TRANS_FADE_OUT_END_TIME) {
            opacity = 1 - (fadeStopwatch / TRANS_FADE_OUT_END_TIME);
            setOpacityForState(fadeFromState, opacity);
            setOpacityForState(fadeToState, 0);
        } else if (fadeStopwatch < TRANS_FADE_IN_END_TIME) {
            opacity = (fadeStopwatch - TRANS_FADE_OUT_END_TIME) /
                    (TRANS_FADE_IN_END_TIME - TRANS_FADE_OUT_END_TIME);
            setOpacityForState(fadeFromState, 0);
            setOpacityForState(fadeToState, opacity);
        } else {
            setOpacityForState(fadeToState, 1);
            currentState = fadeToState;
        }
        updateForState(fadeFromState, secs);
        updateForState(fadeToState, secs);
        break;

    default:
        updateForState(currentState, secs);
        break;
    }
}

void MainMenu::render(Renderer *renderer)
{
    /* Render background and title which are always present */
    renderer->drawTexture(bgTexture, bgSrcXPos, 0, bgSrcWidth,
            bgTexture->GetHeight(), 0, 0, getWidth(), getHeight());
    renderer->drawTexture(titleTexture, titleXPos, titleYPos, titleWidth,
            titleHeight, titleOpacity);

    switch (currentState) {
    
    case State::INITIAL_FADE:
        renderForState(State::SHOW_TOPLEVEL, renderer);
        break;
    
    case State::FADE_TRANSITION:
        renderForState(fadeFromState, renderer);
        renderForState(fadeToState, renderer);
        break;

    default:
        renderForState(currentState, renderer);
        break;
    }
}

void MainMenu::onSizeChangedFrom(int oldWidth, int oldHeight)
{
    calculateBgSizeParams();
    calculateTitleSizeParams();

    float zoomSelHeight, zoomSelYPos, zoomSelItemPadding;

    /* Update toplevel ZoomSelector size params */
    zoomSelHeight = (TOPLEVEL_ZS_BOT_POS - TOPLEVEL_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((TOPLEVEL_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    toplevelZoomSel->updatePosition(zoomSelHeight, getWidth() / 2, zoomSelYPos, zoomSelItemPadding);

    /* Update toplevel ZoomSelector size params */
    zoomSelHeight = (SETTINGS_ZS_BOT_POS - SETTINGS_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((SETTINGS_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    settingsZoomSel->updatePosition(zoomSelHeight, getWidth() / 2, zoomSelYPos, zoomSelItemPadding);
}

void MainMenu::calculateBgSizeParams()
{
    bgSrcWidth = (bgTexture->GetHeight() / (float) getHeight()) * getWidth();
    bgSrcXMax = bgTexture->GetWidth() - bgSrcWidth;
    bgSrcXPos = bgSrcXMax * bgSrcXPercent;
}

void MainMenu::calculateTitleSizeParams()
{
    titleYPos = getHeight() * TITLE_TOP_PADDING_PCT;
    titleHeight = getHeight() * TITLE_HEIGHT_PCT;
    titleWidth = (titleHeight / titleTexture->GetHeight()) *
            titleTexture->GetWidth();
    titleXPos = (getWidth() - titleWidth) / 2;
}

void MainMenu::onMousePosUpdate(const raylib::Vector2 &pos)
{
    switch (currentState) {
    
    case State::INITIAL_FADE:
        updateMousePosForState(State::SHOW_TOPLEVEL, pos);
        break;

    case State::FADE_TRANSITION:
        updateMousePosForState(fadeFromState, pos);
        updateMousePosForState(fadeToState, pos);
        break;

    default:
        updateMousePosForState(currentState, pos);
        break;
    }
}

MainMenu::ReturnCode MainMenu::getReturnCode()
{
    return nextReturnCode;
}

void MainMenu::onMouseButtonPressed(int button, const raylib::Vector2 &pos)
{
    /* Handle left button press event per state of the menu */
    if (button == MOUSE_LEFT_BUTTON) {
        switch (currentState) {
        case State::INITIAL_FADE:
            break;
        case State::SHOW_TOPLEVEL:
            toplevelZoomSel->onMousePressed();
            break;
        case State::SHOW_SETTINGS:
            settingsZoomSel->onMousePressed();
            break;
        case State::FADE_TRANSITION:
            break;
        }
    }
}

void MainMenu::onZoomSelectorClicked(ZoomSelector *source, int index)
{
    if (source == toplevelZoomSel) {
        /* Handle click for top level menu zoom selector */
        switch(index) {

        /* Play Local Game */
        case 0:
            break;

        /* Play Online */
        case 1:
            break;

        /* Settings */
        case 2:
            initiateFadeToState(State::SHOW_SETTINGS);
            break;

        /* Exit */
        case 3:
            nextReturnCode = ReturnCode::EXIT_PROGRAM;
            break;

        default:
            break;
        }
    } else if (source == settingsZoomSel) {
        /* Handle click for settings submenu zoom selector */
        switch(index) {

        /* Exit */
        case 0:
            initiateFadeToState(State::SHOW_TOPLEVEL);
            break;

        default:
            break;
        }
    }
}

void MainMenu::initiateFadeToState(State nextState)
{
    fadeStopwatch = 0;
    fadeFromState = currentState;
    fadeToState = nextState;
    currentState = State::FADE_TRANSITION;
}

void MainMenu::setOpacityForState(State menuState, float opacity)
{
    switch (menuState) {
    
    case State::SHOW_TOPLEVEL:
        toplevelZoomSel->setDependentOpacity(opacity);
        break;
    
    case State::SHOW_SETTINGS:
        settingsZoomSel->setDependentOpacity(opacity);
        break;

    default:
        break;
    }
}

void MainMenu::updateForState(State menuState, double secs)
{
    switch(menuState) {
    
    case State::SHOW_TOPLEVEL:
        toplevelZoomSel->update(secs);
        break;

    case State::SHOW_SETTINGS:
        settingsZoomSel->update(secs);
        break;

    default:
        break;
    }
}

void MainMenu::updateMousePosForState(State menuState, const raylib::Vector2 &pos)
{
    switch(menuState) {

    case State::SHOW_TOPLEVEL:
        toplevelZoomSel->onMousePosUpdate(pos);
        break;

    case State::SHOW_SETTINGS:
        settingsZoomSel->onMousePosUpdate(pos);
        break;

    default:
        break;
    }
}

void MainMenu::renderForState(State menuState, Renderer *renderer)
{
    switch(menuState) {
    
    case State::SHOW_TOPLEVEL:
        toplevelZoomSel->render(renderer);
        break;

    case State::SHOW_SETTINGS:
        settingsZoomSel->render(renderer);
        break;
    
    default:
        break;
    }
}
