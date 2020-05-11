#include "MainMenu.h"

#include "Util.h"

/* Filepaths for external resources */
static const char BACKGROUND_IMG_PATH[] = "MainMenu/desert-background.png";
static const char TITLE_IMG_PATH[] = "MainMenu/main-title.png";
static const char ZOOMSELECTOR_IMG_PATH[] = "MainMenu/zoomselector.png";
static const char TEXT_LABEL_PATHS[] = "MainMenu/text-labels.png";
static const char SELECTABLEBUTTON_IMG_PATH[] = "MainMenu/selectablebutton.png";

/* Background pan animation parameters */
static const float BG_MAX_SPEED = 0.02;
static const float BG_MIN_SPEED = 0.005;
static const float BG_SPEED_CUTOFF = 0.05;

/* Main title positioning parameters */
static const float TITLE_TOP_PADDING_PCT = 0.09;
static const float TITLE_HEIGHT_PCT = 0.275;

/* Timing parameters for initial fade-in animation */
static const float INIT_FADE_START_TIME = 0.5;
static const float INIT_FADE_END_TIME = 1.2;

/* Timing parameters for transition fade between submenus */
static const float TRANS_FADE_OUT_END_TIME = 0.2;
static const float TRANS_FADE_IN_END_TIME = 0.4;

/* Parameters common to all menu ZoomSelectors */
static const float ZS_HOVER_RATIO = 1.75;
static const float ZS_ITEM_PAD_PCT = 0.05;

/* Toplevel zoom selector parameters */
static const float TOPLEVEL_ZS_TOP_POS = 0.435;
static const float TOPLEVEL_ZS_BOT_POS = 0.93;

/* Settings submenu parameters */
static const float SETTINGS_ZS_TOP_POS = 0.73;
static const float SETTINGS_ZS_BOT_POS = 0.9;
static const float SETTINGS_ZS_DIST_FROM_CTR_HEIGHT_PCT = 1.2;      // relative to height of ZS
static const float SETTINGS_TL_HEIGHT_PCT = 0.04;
static const float SETTINGS_TL_WINDOW_MODE_TOP_POS = 0.48;
static const float SETTINGS_TL_RESOLUTION_TOP_POS = 0.62;
static const float SETTINGS_TL_DIST_FROM_CTR_PCT = 0.3;   // relative to scene height
static const float SETTINGS_SB_TOP_POS = 0.465;
static const float SETTINGS_SB_HEIGHT_PCT = 0.06;
static const float SETTINGS_SB_DIST_FROM_CTR_PCT = 0.00;    // relative to scene height
static const float SETTINGS_SB_PADDING_BETWEEN = 0.05;      // relative to scene height
static const float SETTINGS_BS_RESOLUTION_TOP_POS = 0.605;  // relative to scene height
static const float SETTINGS_BS_HEIGHT_PCT = 0.06;           // relative to scene height

/* Definitions for positions of particular entries on zoom selectors textures */
static const int ZS_TEXT_CENTER_Y = 60;
static const raylib::Rectangle ZS_TEXT_PLAY_LOCAL_GAME(0, 0, 894, 137);
static const raylib::Rectangle ZS_TEXT_PLAY_ONLINE(0, 137, 616, 137);
static const raylib::Rectangle ZS_TEXT_SETTINGS(0, 274, 445, 137);
static const raylib::Rectangle ZS_TEXT_EXIT(0, 411, 189, 137);
static const raylib::Rectangle ZS_TEXT_APPLY(0, 548, 295, 137);
static const raylib::Rectangle ZS_TEXT_BACK(0, 685, 232, 137);

/* Definitions for positions of particular entries on SelectableButton texture */
static const raylib::Rectangle SB_WINDOWED_UNSELECTED(0, 0, 788, 192);
static const raylib::Rectangle SB_WINDOWED_SELECTED(0, 192, 788, 192);
static const raylib::Rectangle SB_FULLSCREEN_UNSELECTED(788, 0, 832, 192);
static const raylib::Rectangle SB_FULLSCREEN_SELECTED(788, 192, 832, 192);

/* Definitions for positions of particular entries in the text label texture */
static const raylib::Rectangle TL_WINDOW_MODE(0, 0, 772, 137);
static const raylib::Rectangle TL_RESOLUTION(0, 137, 608, 137);

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

MainMenu::MainMenu(bool windowIsFullscreen)
{
    /* Initialize some generic fields */
    bgSrcXPercent = 0;
    bgSrcXPosIncreasing = true;
    titleOpacity = 0;
    fadeStopwatch = 0;
    currentState = State::INITIAL_FADE;
    nextReturnCode = ReturnCode::KEEP_RUNNING;

    /* Load textures for background and main title text and text labels */
    bgTexture = new raylib::Texture(Util::formResourcePath(BACKGROUND_IMG_PATH));
    bgTexture->GenMipmaps();
    titleTexture = new raylib::Texture(Util::formResourcePath(TITLE_IMG_PATH));
    titleTexture->GenMipmaps();
    textLabelTexture = new raylib::Texture(Util::formResourcePath(TEXT_LABEL_PATHS));
    textLabelTexture->GenMipmaps();
    selButtonTexture = new raylib::Texture(Util::formResourcePath(SELECTABLEBUTTON_IMG_PATH));
    selButtonTexture->GenMipmaps();
    settingsTLOpacity = 0;

    /* Initialize top level menu zoom selector */
    toplevelZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    toplevelZoomSel->addItem(ZS_TEXT_PLAY_LOCAL_GAME, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_PLAY_ONLINE, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_SETTINGS, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->addItem(ZS_TEXT_EXIT, ZS_TEXT_CENTER_Y);
    toplevelZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    toplevelZoomSel->setDependentOpacity(0);

    /* Initialize settings submenu zoom selectors */
    settingsApplyZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    settingsApplyZoomSel->addItem(ZS_TEXT_APPLY, ZS_TEXT_CENTER_Y);
    settingsApplyZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    settingsApplyZoomSel->setDependentOpacity(0);
    settingsBackZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    settingsBackZoomSel->addItem(ZS_TEXT_BACK, ZS_TEXT_CENTER_Y);
    settingsBackZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    settingsBackZoomSel->setDependentOpacity(0);

    /* Initialize settings submenu selectable buttons */
    windowedSelButton = new SelectableButton(selButtonTexture, SB_WINDOWED_SELECTED,
            SB_WINDOWED_UNSELECTED, !windowIsFullscreen);
    fullscreenSelButton = new SelectableButton(selButtonTexture, SB_FULLSCREEN_SELECTED,
            SB_FULLSCREEN_UNSELECTED, windowIsFullscreen);
    windowedSelButton->setCallback(std::bind(&MainMenu::onSelectableButtonSelected, this, _1));
    fullscreenSelButton->setCallback(std::bind(&MainMenu::onSelectableButtonSelected, this, _1));
    windowedSelButton->setDependentOpacity(0);
    fullscreenSelButton->setDependentOpacity(0);

    /* Initialize settings submenu box selector */
    resolutionBoxSel = new BoxSelector();
    /* Enter resolutions, starting with highest. Highest mode will be selected as first entry */
    for (int i = Window::getNumWindowedConfigModes() - 1; i >= 0; i--) {
        const WindowConfiguration cfg = Window::getWindowedConfigModes()[i];
        resolutionBoxSel->addItem(std::to_string(cfg.windowWidth) + " x " +
                std::to_string(cfg.windowHeight), false);
    }
    resolutionBoxSel->setDependentOpacity(0);
}

MainMenu::~MainMenu()
{
    delete bgTexture;
    delete titleTexture;
    delete textLabelTexture;
    delete selButtonTexture;
    delete toplevelZoomSel;
    delete windowedSelButton;
    delete fullscreenSelButton;
    delete settingsApplyZoomSel;
    delete settingsBackZoomSel;
    delete resolutionBoxSel;
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
    float textLabelHeight, textLabelMaxX;
    float distFromCenter;
    float selButtonXPos;
    float ctrStartCoord, ctrEndCoord;

    /* Update toplevel ZoomSelector size params */
    zoomSelHeight = (TOPLEVEL_ZS_BOT_POS - TOPLEVEL_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((TOPLEVEL_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    toplevelZoomSel->updatePosition(zoomSelHeight, getWidth() / 2, zoomSelYPos, zoomSelItemPadding);

    /* Update settings ZoomSelectors size params */
    zoomSelHeight = (SETTINGS_ZS_BOT_POS - SETTINGS_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((SETTINGS_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    distFromCenter = zoomSelHeight * SETTINGS_ZS_DIST_FROM_CTR_HEIGHT_PCT;
    settingsApplyZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) - distFromCenter,
            zoomSelYPos, zoomSelItemPadding);
    settingsBackZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) + distFromCenter,
            zoomSelYPos, zoomSelItemPadding);

    /* Update settings menu text labels size params */
    textLabelHeight = getHeight() * SETTINGS_TL_HEIGHT_PCT;
    textLabelMaxX = (getWidth() / 2) - (getHeight() * SETTINGS_TL_DIST_FROM_CTR_PCT);
    settingsTLWindowModeDst.height = textLabelHeight;
    settingsTLWindowModeDst.y = getHeight() * SETTINGS_TL_WINDOW_MODE_TOP_POS;
    settingsTLWindowModeDst.width = (textLabelHeight / TL_WINDOW_MODE.height) *
            TL_WINDOW_MODE.width;
    settingsTLWindowModeDst.x = textLabelMaxX - settingsTLWindowModeDst.width;
    settingsTLResolutionDst.height = textLabelHeight;
    settingsTLResolutionDst.y = getHeight() * SETTINGS_TL_RESOLUTION_TOP_POS;
    settingsTLResolutionDst.width = (textLabelHeight / TL_RESOLUTION.height) * TL_RESOLUTION.width;
    settingsTLResolutionDst.x = textLabelMaxX - settingsTLResolutionDst.width;

    /* Update settings SelectableButtons size params */
    windowedSelButton->setHeight(getHeight() * SETTINGS_SB_HEIGHT_PCT);
    fullscreenSelButton->setHeight(getHeight() * SETTINGS_SB_HEIGHT_PCT);
    windowedSelButton->setY(getHeight() * SETTINGS_SB_TOP_POS);
    fullscreenSelButton->setY(getHeight() * SETTINGS_SB_TOP_POS);
    distFromCenter = SETTINGS_SB_DIST_FROM_CTR_PCT * getHeight();
    selButtonXPos = (getWidth() / 2) + distFromCenter;
    ctrStartCoord = selButtonXPos;
    windowedSelButton->setX(selButtonXPos);
    selButtonXPos += windowedSelButton->getWidth();
    selButtonXPos += SETTINGS_SB_PADDING_BETWEEN * getHeight();
    fullscreenSelButton->setX(selButtonXPos);
    ctrEndCoord = selButtonXPos + fullscreenSelButton->getWidth();

    /* Update settings BoxSelectors size params */
    resolutionBoxSel->setYPos(getHeight() * SETTINGS_BS_RESOLUTION_TOP_POS);
    resolutionBoxSel->setHeight(getHeight() * SETTINGS_BS_HEIGHT_PCT);
    distFromCenter = getHeight() * SETTINGS_TL_DIST_FROM_CTR_PCT;
    resolutionBoxSel->setXPos((ctrStartCoord + ctrEndCoord - resolutionBoxSel->getWidth()) / 2);
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

void MainMenu::setWindowRequestCallback(std::function<void(const WindowConfiguration&)> callback)
{
    windowRequestCallback = callback;
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
            settingsApplyZoomSel->onMousePressed();
            settingsBackZoomSel->onMousePressed();
            windowedSelButton->onMousePressed(pos);
            fullscreenSelButton->onMousePressed(pos);
            if (windowedSelButton->isSelected()){
                resolutionBoxSel->onMousePressed(pos);
            }
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
    } else if (source == settingsApplyZoomSel) {
        /* Handle click for settings submenu "apply" button */
        if (fullscreenSelButton->isSelected()) {
            WindowConfiguration cfg = {
                .isFullscreen = true,
                .windowWidth = 0,
                .windowHeight = 0,
            };
            windowRequestCallback(cfg);
        } else {
            int selectedIdx = resolutionBoxSel->getSelectedIndex();
            if (selectedIdx != -1) {
                selectedIdx = Window::getNumWindowedConfigModes() - 1 - selectedIdx;    // In reverse order
                windowRequestCallback(Window::getWindowedConfigModes()[selectedIdx]);
            }
        }
    } else if (source == settingsBackZoomSel) {
        /* Handle click for settings submenu "back" button */
        initiateFadeToState(State::SHOW_TOPLEVEL);
    }
}

void MainMenu::onSelectableButtonSelected(SelectableButton *source)
{
    if (source == windowedSelButton) {
        fullscreenSelButton->setSelected(false);
    } else if (source == fullscreenSelButton) {
        windowedSelButton->setSelected(false);
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
        settingsBackZoomSel->setDependentOpacity(opacity);
        settingsApplyZoomSel->setDependentOpacity(opacity);
        settingsTLOpacity = opacity;
        windowedSelButton->setDependentOpacity(opacity);
        fullscreenSelButton->setDependentOpacity(opacity);
        resolutionBoxSel->setDependentOpacity(opacity);
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
        settingsApplyZoomSel->update(secs);
        settingsBackZoomSel->update(secs);
        resolutionBoxSel->update(secs);
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
        settingsApplyZoomSel->onMousePosUpdate(pos);
        settingsBackZoomSel->onMousePosUpdate(pos);
        resolutionBoxSel->onMousePosUpdate(pos);
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
        renderer->drawTexture(textLabelTexture, TL_WINDOW_MODE, settingsTLWindowModeDst,
                settingsTLOpacity);
        windowedSelButton->render(renderer);
        fullscreenSelButton->render(renderer);
        if (windowedSelButton->isSelected()) {
            renderer->drawTexture(textLabelTexture, TL_RESOLUTION, settingsTLResolutionDst,
                    settingsTLOpacity);
            resolutionBoxSel->render(renderer);
        }
        settingsApplyZoomSel->render(renderer);
        settingsBackZoomSel->render(renderer);
        break;
    
    default:
        break;
    }
}
