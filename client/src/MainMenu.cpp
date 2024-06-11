#include "MainMenu.h"

#include "Util.h"
#include <stdio.h>

/* Filepaths for external resources */
static const char BACKGROUND_IMG_PATH[] = "MainMenu/desert-background.png";
static const char TITLE_IMG_PATH[] = "MainMenu/main-title.png";
static const char ZOOMSELECTOR_IMG_PATH[] = "MainMenu/zoomselector.png";
static const char TEXT_LABEL_PATHS[] = "MainMenu/text-labels.png";
static const char SELECTABLEBUTTON_IMG_PATH[] = "MainMenu/selectablebutton.png";
static const char MARVEL_FONT_PATH[] = "Fonts/Marvel-Regular.ttf";
static const char RECON_OVERLAY_BG_PATH[] = "recon-overlay-bg.png";
static const char RECON_TEXT_PATH[] = "recon-text.png";

/* Constants related to fonts */
static const int BASE_FONT_SIZE = 175;

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

/* Online name input submenu parameters */
static const float OL_NAME_ZS_TOP_POS = 0.73;
static const float OL_NAME_ZS_BOT_POS = 0.9;
static const float OL_NAME_ZS_DIST_FROM_CTR_HEIGHT_PCT = 1.2;      // relative to height of ZS
static const float OL_NAME_PROMPT_TL_HEIGHT_PCT = 0.05;
static const float OL_NAME_PROMPT_TL_TOP_POS = 0.45;
static const float OL_NAME_TB_TOP_POS = 0.55; /* Pct of screen height where tb top is */
static const float OL_NAME_TB_WIDTH_PCT = 0.6; /* Pct of screen height = width of tb */
static const float OL_NAME_TB_HEIGHT_PCT = 0.08; /* Pct of screen height = height of tb */
static const int OL_NAME_TB_MAX_CHARS = 15;
static const float OL_NAME_LS_VERT_POS = 0.70; /* Pct of screen height = y coord of ctr pt of ls */
static const float OL_NAME_LS_SIZE_PCT = 0.08; /* Pct of screen height = size of ls */
static const float OL_NAME_SUBMIT_MIN_LOAD_TIME = 1.0;
static const float OL_NAME_TB_ERROR_TL_HEIGHT_PCT = 0.04; /* Pct of screen ht = height of tl */
static const float OL_NAME_TB_ERROR_TL_TOP_POS = 0.68; /* Pct of screen ht = y coord of top of tl */

/* Host/join zoom selector parameters */
static const float HOST_JOIN_ZS_TOP_POS = 0.45;
static const float HOST_JOIN_ZS_BOT_POS = 0.89;

/* Saved games screen parameters */
static const float SAVED_GAMES_LS_TOP_POS = 0.40; /* Pct of screen height */
static const float SAVED_GAMES_LS_BOT_POS = 0.83; /* Pct of screen height */
static const float SAVED_GAMES_LS_WIDTH = 0.75; /* Pct of screen width */
static const float SAVED_GAMES_LS_ITEM_HEIGHT = 0.1; /* Pct of screen height = height each item */
static const float SAVED_GAMES_LS_SEP_HEIGHT = 0.002; /* Pct of screen height = height of separator */
static const float SAVED_GAMES_LS_CREATE_TEXT_HEIGHT = 0.6; /* Pct of item height = text height */
static const char SAVED_GAMES_LS_CREATE_TEXT[] = "Create New Game";
static const float SAVED_GAMES_ZS_TOP_POS = 0.83;
static const float SAVED_GAMES_ZS_BOT_POS = 1.0;
static const float SAVED_GAMES_ZS_DIST_FROM_CTR = 1.2; /* Fraction of height of ZS */

/* Create game online screen parameters */
static const float CREATE_GAME_OL_TL_TOP_POS = 0.45; /* Top of text label, as pct of screen height */
static const float CREATE_GAME_OL_TL_HEIGHT_PCT = 0.05; /* txt label height, pct of screen height */
static const float CREATE_GAME_OL_TB_TOP_POS = 0.55; /* Top of text box, as pct of screen height */
static const float CREATE_GAME_OL_TB_WIDTH_PCT = 1.4; /* Pct of screen height = width of tb */
static const float CREATE_GAME_OL_TB_HEIGHT_PCT = 0.08; /* Pct of screen height = height of tb */
static const float CREATE_GAME_OL_TB_MAX_CHARS = 35; /* Max number of chars for text box */
static const float CREATE_GAME_OL_ZS_TOP_POS = 0.73; /* Top of ZoomSels as pct of screen height */
static const float CREATE_GAME_OL_ZS_BOT_POS = 0.9; /* Bottom of ZoomSels as pct of screen height */
static const float CREATE_GAME_OL_ZS_DIST_FROM_CTR = 1.2; /* Fraction of height of ZS */

/* Overlay for 'reconnecting' parameters */
static const float RECON_TEXT_TOP_POS = 0.25; /* Pct of screen height = y coord of top of text */
static const float RECON_TEXT_HEIGHT_PCT = 0.06; /* Pct of screen height = height of text */
static const float RECON_TIME_TOP_POS = 0.35; /* Pct of screen height = y of top of time text */
static const float RECON_TIME_HEIGHT_PCT = 0.08; /* Pct of screen height = height of time text */
static const float RECON_TIME_SPACE_PCT = 0.07; /* Pct of time text height = spacing b/t chars */
static const float RECON_LS_VERT_POS = 0.53; /* Pct of screen height = y coord of ctr pt of ls */
static const float RECON_LS_SIZE_PCT = 0.12; /* Pct of screen height = size of ls */

/* Definitions for positions of particular entries on zoom selectors textures */
static const int ZS_TEXT_CENTER_Y = 60;
static const raylib::Rectangle ZS_TEXT_PLAY_LOCAL_GAME(0, 0, 894, 137);
static const raylib::Rectangle ZS_TEXT_PLAY_ONLINE(0, 137, 616, 137);
static const raylib::Rectangle ZS_TEXT_SETTINGS(0, 274, 445, 137);
static const raylib::Rectangle ZS_TEXT_EXIT(0, 411, 189, 137);
static const raylib::Rectangle ZS_TEXT_APPLY(0, 548, 295, 137);
static const raylib::Rectangle ZS_TEXT_BACK(0, 685, 232, 137);
static const raylib::Rectangle ZS_TEXT_SUBMIT(0, 822, 384, 137);
static const raylib::Rectangle ZS_TEXT_HOST_GAME(0, 959, 541, 137);
static const raylib::Rectangle ZS_TEXT_JOIN_GAME(0, 1096, 518, 137);
static const raylib::Rectangle ZS_TEXT_NEXT(0, 1233, 246, 137);

/* Definitions for positions of particular entries on SelectableButton texture */
static const raylib::Rectangle SB_WINDOWED_UNSELECTED(0, 0, 788, 192);
static const raylib::Rectangle SB_WINDOWED_SELECTED(0, 192, 788, 192);
static const raylib::Rectangle SB_FULLSCREEN_UNSELECTED(788, 0, 832, 192);
static const raylib::Rectangle SB_FULLSCREEN_SELECTED(788, 192, 832, 192);

/* Definitions for positions of particular entries in the text label texture */
static const raylib::Rectangle TL_WINDOW_MODE(0, 0, 772, 137);
static const raylib::Rectangle TL_RESOLUTION(0, 137, 608, 137);
static const raylib::Rectangle TL_ENTER_A_NAME(0, 274, 790, 137);
static const raylib::Rectangle TL_NAME_TAKEN(0, 411, 618, 75);
static const raylib::Rectangle TL_CONNECT_FAILED(0, 490, 833, 75);
static const raylib::Rectangle TL_NEW_GAME_SAVE_FILE_NAME(0, 570, 1503, 137);

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

MainMenu::MainMenu(Window &window)
{
    /* Initialize some generic fields */
    bgSrcXPercent = 0;
    bgSrcXPosIncreasing = true;
    titleOpacity = 0;
    fadeStopwatch = 0;
    currentState = State::INITIAL_FADE;
    nextReturnCode = ReturnCode::KEEP_RUNNING;
    olNameSubmitResult = ServerSession::Event::NONE;
    olNameShowConnectErr = false;
    olNameShowTakenErr = false;
    reconnecting = false;

    marvelFont = new raylib::Font(Util::formResourcePath(MARVEL_FONT_PATH), BASE_FONT_SIZE,
            nullptr, 95);
    GenTextureMipmaps(&marvelFont->texture);

    /* Create the ServerSession for use with online communication */
    session = new ServerSession();
    session->registerCallback(std::bind(&MainMenu::onSessionEvent, this, _1));

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
    olNameTLOpacity = 0;
    createGameOlTLOpacity = 0;

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
            SB_WINDOWED_UNSELECTED, !window.isFullscreen());
    fullscreenSelButton = new SelectableButton(selButtonTexture, SB_FULLSCREEN_SELECTED,
            SB_FULLSCREEN_UNSELECTED, window.isFullscreen());
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

    /* Initialize online name input submenu zoom selectors */
    olNameSubmitZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    olNameSubmitZoomSel->addItem(ZS_TEXT_SUBMIT, ZS_TEXT_CENTER_Y);
    olNameSubmitZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    olNameSubmitZoomSel->setEnabled(false); /* Disabled until text is entered */
    olNameSubmitZoomSel->setDependentOpacity(0);
    olNameBackZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    olNameBackZoomSel->addItem(ZS_TEXT_BACK, ZS_TEXT_CENTER_Y);
    olNameBackZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    olNameBackZoomSel->setDependentOpacity(0);

    /* Initialize online name input submenu textbox and loading spinner */
    olNameTextBox = new MenuTextBox(OL_NAME_TB_MAX_CHARS);
    olNameTextBox->setEmptyCallback(std::bind(&MainMenu::onTBEmptinessChanged, this, _1, _2));
    olNameLoadSpinner = new LoadingSpinner();

    /* Initialize host/join submenu zoom selector */
    hostJoinZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    hostJoinZoomSel->addItem(ZS_TEXT_HOST_GAME, ZS_TEXT_CENTER_Y);
    hostJoinZoomSel->addItem(ZS_TEXT_JOIN_GAME, ZS_TEXT_CENTER_Y);
    hostJoinZoomSel->addItem(ZS_TEXT_BACK, ZS_TEXT_CENTER_Y);
    hostJoinZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    hostJoinZoomSel->setDependentOpacity(0);

    /* Initialize saved games submenu list selector */
    savedGamesListSel = new ListSelector();
    savedGamesListSel->setSelectionCallback(std::bind(&MainMenu::onListSelectorChanged, this, _1, _2));
    ListSelectorTextItem *item = new ListSelectorTextItem(SAVED_GAMES_LS_CREATE_TEXT_HEIGHT,
            SAVED_GAMES_LS_CREATE_TEXT);
    savedGamesListSel->appendItem(item);

    /* Initialize saved games submenu zoom selectors */
    savedGamesBackZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    savedGamesBackZoomSel->addItem(ZS_TEXT_BACK, ZS_TEXT_CENTER_Y);
    savedGamesBackZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    savedGamesBackZoomSel->setDependentOpacity(0);
    savedGamesNextZoomSel = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    savedGamesNextZoomSel->addItem(ZS_TEXT_NEXT, ZS_TEXT_CENTER_Y);
    savedGamesNextZoomSel->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    savedGamesNextZoomSel->setDependentOpacity(0);
    savedGamesNextZoomSel->setEnabled(false); /* Disabled until option selected in ListSelector */

    /* Initialize create online game submenu text box */
    createGameOlTextBox = new MenuTextBox(CREATE_GAME_OL_TB_MAX_CHARS);
    createGameOlTextBox->setEmptyCallback(std::bind(&MainMenu::onTBEmptinessChanged, this, _1, _2));

    /* Initialize create online game submenu zoom selectors */
    createGameOlBackZS = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    createGameOlBackZS->addItem(ZS_TEXT_BACK, ZS_TEXT_CENTER_Y);
    createGameOlBackZS->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    createGameOlBackZS->setDependentOpacity(0);
    createGameOlNextZS = new ZoomSelector(ZOOMSELECTOR_IMG_PATH, ZS_HOVER_RATIO);
    createGameOlNextZS->addItem(ZS_TEXT_NEXT, ZS_TEXT_CENTER_Y);
    createGameOlNextZS->setCallback(std::bind(&MainMenu::onZoomSelectorClicked, this, _1, _2));
    createGameOlNextZS->setDependentOpacity(0);
    createGameOlNextZS->setEnabled(false); /* Disabled until text has been entered */

    /* Initialize objects for 'reconnecting' overlay */
    reconLoadSpinner = new LoadingSpinner();
    reconOverlayBgTexture = new raylib::Texture(Util::formResourcePath(RECON_OVERLAY_BG_PATH));
    reconOverlayBgTexture->GenMipmaps();
    reconTextTexture = new raylib::Texture(Util::formResourcePath(RECON_TEXT_PATH));
    reconTextTexture->GenMipmaps();
}

MainMenu::~MainMenu()
{
    delete bgTexture;
    delete titleTexture;
    delete textLabelTexture;
    delete selButtonTexture;
    delete reconOverlayBgTexture;
    delete reconTextTexture;
    delete reconLoadSpinner;
    delete marvelFont;
    delete toplevelZoomSel;
    delete windowedSelButton;
    delete fullscreenSelButton;
    delete settingsApplyZoomSel;
    delete settingsBackZoomSel;
    delete resolutionBoxSel;
    delete olNameTextBox;
    delete olNameSubmitZoomSel;
    delete olNameBackZoomSel;
    delete hostJoinZoomSel;
    delete savedGamesListSel;
    delete savedGamesBackZoomSel;
    delete savedGamesNextZoomSel;
    delete createGameOlTextBox;
    delete createGameOlBackZS;
    delete createGameOlNextZS;
    delete session;
}

void MainMenu::update(double secs)
{
    /* Poll online session */
    session->poll(secs);

    if (reconnecting) {
        
        /* Updating remaining time until connection disconnected */
        int newRemTime = session->getSuspendedTimeLeft();
        if (newRemTime != reconLastRemTime) {
            if ((newRemTime >= 0) && (newRemTime <= 120)) {
                reconLastRemTime = newRemTime;
                snprintf(reconRemTime, MENU_RECON_TIME_STR_BUF_SIZE, "%d", newRemTime);
                calculateReconTimeTextSize(); /* Recalculate rendered size w/ new string */
            } else {
                reconRemTime[0] = '\0';
            }
        }

    } else {
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
    }

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

    if (reconnecting) {
        /* If reconnecting, render additional content over the top */
        renderer->drawTexture(reconOverlayBgTexture, 0, 0, getWidth(), getHeight(), 1.0f);
        renderer->drawTexture(reconTextTexture, reconTextXPos, reconTextYPos, reconTextWidth,
                reconTextHeight, 1.0f);
        
        renderer->setColor(WHITE);
        renderer->drawText(*marvelFont, reconRemTime, (getWidth() - reconTimeRenderWidth) / 2,
            getHeight() * RECON_TIME_TOP_POS, reconTimeFontSize,
            reconTimeFontSize * RECON_TIME_SPACE_PCT, 1.0f);

        reconLoadSpinner->render(renderer);
    }
}

void MainMenu::onSizeChangedFrom(int oldWidth, int oldHeight)
{
    calculateBgSizeParams();
    calculateTitleSizeParams();
    calculateReconOverlaySizeParams();

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
    settingsBackZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) - distFromCenter,
            zoomSelYPos, zoomSelItemPadding);
    settingsApplyZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) + distFromCenter,
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

    /* Update online name input ZoomSelectors size params */
    zoomSelHeight = (OL_NAME_ZS_BOT_POS - OL_NAME_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((OL_NAME_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    distFromCenter = zoomSelHeight * OL_NAME_ZS_DIST_FROM_CTR_HEIGHT_PCT;
    olNameBackZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) - distFromCenter,
            zoomSelYPos, zoomSelItemPadding);
    olNameSubmitZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) + distFromCenter,
            zoomSelYPos, zoomSelItemPadding);

    /* Update online name input text labels, loading spinner, and textbox size params */
    textLabelHeight = getHeight() * OL_NAME_PROMPT_TL_HEIGHT_PCT;
    olNameTLPromptDst.height = textLabelHeight;
    olNameTLPromptDst.y = getHeight() * OL_NAME_PROMPT_TL_TOP_POS;
    olNameTLPromptDst.width = (textLabelHeight / TL_ENTER_A_NAME.height) * TL_ENTER_A_NAME.width;
    olNameTLPromptDst.x = (getWidth() - olNameTLPromptDst.width) / 2;
    textLabelHeight = getHeight() * OL_NAME_TB_ERROR_TL_HEIGHT_PCT;
    olNameTLTakenDst.height = textLabelHeight;
    olNameTLTakenDst.y = getHeight() * OL_NAME_TB_ERROR_TL_TOP_POS;
    olNameTLTakenDst.width = (textLabelHeight / TL_NAME_TAKEN.height) * TL_NAME_TAKEN.width;
    olNameTLTakenDst.x = (getWidth() - olNameTLTakenDst.width) / 2;
    olNameTLConnectFailed.height = textLabelHeight;
    olNameTLConnectFailed.y = getHeight() * OL_NAME_TB_ERROR_TL_TOP_POS;
    olNameTLConnectFailed.width = (textLabelHeight / TL_CONNECT_FAILED.height) * TL_CONNECT_FAILED.width;
    olNameTLConnectFailed.x = (getWidth() - olNameTLConnectFailed.width) / 2;
    olNameLoadSpinner->setSize(getHeight() * OL_NAME_LS_SIZE_PCT);
    olNameLoadSpinner->setY(getHeight() * OL_NAME_LS_VERT_POS);
    olNameLoadSpinner->setX(getWidth() / 2);
    olNameTextBox->setSize(getHeight() * OL_NAME_TB_WIDTH_PCT, getHeight() * OL_NAME_TB_HEIGHT_PCT);
    olNameTextBox->setX((getWidth() - olNameTextBox->getWidth()) / 2);
    olNameTextBox->setY(getHeight() * OL_NAME_TB_TOP_POS);

    /* Update host/join ZoomSelector size params */
    zoomSelHeight = (HOST_JOIN_ZS_BOT_POS - HOST_JOIN_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((HOST_JOIN_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    hostJoinZoomSel->updatePosition(zoomSelHeight, getWidth() / 2, zoomSelYPos, zoomSelItemPadding);

    /* Update saved games list ListSelector size params */
    savedGamesListSel->setY(getHeight() * SAVED_GAMES_LS_TOP_POS);
    savedGamesListSel->setHeight(getHeight() * (SAVED_GAMES_LS_BOT_POS - SAVED_GAMES_LS_TOP_POS));
    savedGamesListSel->setWidth(getWidth() * SAVED_GAMES_LS_WIDTH);
    distFromCenter = (savedGamesListSel->getWidth() / 2);
    savedGamesListSel->setX((getWidth() / 2) - distFromCenter);
    savedGamesListSel->setSeparatorHeight(getHeight() * SAVED_GAMES_LS_SEP_HEIGHT, true);
    savedGamesListSel->setItemHeight(getHeight() * SAVED_GAMES_LS_ITEM_HEIGHT);

    /* Update saved games submenu ZoomSelectors size params */
    zoomSelHeight = (SAVED_GAMES_ZS_BOT_POS - SAVED_GAMES_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((SAVED_GAMES_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    distFromCenter = zoomSelHeight * SAVED_GAMES_ZS_DIST_FROM_CTR;
    savedGamesBackZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) - distFromCenter,
            zoomSelYPos, zoomSelItemPadding);
    savedGamesNextZoomSel->updatePosition(zoomSelHeight, (getWidth() / 2) + distFromCenter,
            zoomSelYPos, zoomSelItemPadding);

    /* Update create game online submenu text label and text box size params */
    textLabelHeight = getHeight() * CREATE_GAME_OL_TL_HEIGHT_PCT;
    createGameOlTLPromptDst.height = textLabelHeight;
    createGameOlTLPromptDst.y = getHeight() * CREATE_GAME_OL_TL_TOP_POS;
    createGameOlTLPromptDst.width =
            (textLabelHeight / TL_NEW_GAME_SAVE_FILE_NAME.height) * TL_NEW_GAME_SAVE_FILE_NAME.width;
    createGameOlTLPromptDst.x = (getWidth() - createGameOlTLPromptDst.width) / 2;
    createGameOlTextBox->setSize(getHeight() * CREATE_GAME_OL_TB_WIDTH_PCT,
            getHeight() * CREATE_GAME_OL_TB_HEIGHT_PCT);
    createGameOlTextBox->setX((getWidth() - createGameOlTextBox->getWidth()) / 2);
    createGameOlTextBox->setY(getHeight() * CREATE_GAME_OL_TB_TOP_POS);

    /* Create game online submenu ZoomSelectors size params */
    zoomSelHeight = (CREATE_GAME_OL_ZS_BOT_POS - CREATE_GAME_OL_ZS_TOP_POS) * getHeight();
    zoomSelYPos = ((CREATE_GAME_OL_ZS_TOP_POS) * getHeight()) + (zoomSelHeight / 2);
    zoomSelItemPadding = ZS_ITEM_PAD_PCT * getHeight();
    distFromCenter = zoomSelHeight * CREATE_GAME_OL_ZS_DIST_FROM_CTR;
    createGameOlBackZS->updatePosition(zoomSelHeight, (getWidth() / 2) - distFromCenter,
            zoomSelYPos, zoomSelItemPadding);
    createGameOlNextZS->updatePosition(zoomSelHeight, (getWidth() / 2) + distFromCenter,
            zoomSelYPos, zoomSelItemPadding);

    /* Update 'reconnecting' overlay size params */
    reconLoadSpinner->setSize(getHeight() * RECON_LS_SIZE_PCT);
    reconLoadSpinner->setY(getHeight() * RECON_LS_VERT_POS);
    reconLoadSpinner->setX(getWidth() / 2);
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

void MainMenu::calculateReconOverlaySizeParams()
{
    reconTextYPos = getHeight() * RECON_TEXT_TOP_POS;
    reconTextHeight = getHeight() * RECON_TEXT_HEIGHT_PCT;
    reconTextWidth = (reconTextHeight / reconTextTexture->GetHeight()) *
            reconTextTexture->GetWidth();
    reconTextXPos = (getWidth() - reconTextWidth) / 2;

    reconTimeFontSize = getHeight() * RECON_TIME_HEIGHT_PCT;
    calculateReconTimeTextSize();
}

void MainMenu::calculateReconTimeTextSize()
{
    reconTimeRenderWidth = marvelFont->MeasureText(reconRemTime, reconTimeFontSize,
            reconTimeFontSize * RECON_TIME_SPACE_PCT).GetX();
}

void MainMenu::onMousePosUpdate(const raylib::Vector2 &pos)
{
    if (reconnecting) {
        /* Don't update mouse pos if reconnecting */
        return;
    }

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
    if (reconnecting) {
        /* Don't allow mouse presses if reconnecting */
        return;
    }

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
        case State::SHOW_ONLINE_NAME_INPUT:
            olNameSubmitZoomSel->onMousePressed();
            olNameBackZoomSel->onMousePressed();
            break;
        case State::SHOW_HOST_JOIN:
            hostJoinZoomSel->onMousePressed();
            break;
        case State::SHOW_SAVED_GAMES_LOCAL:
        case State::SHOW_SAVED_GAMES_ONLINE:
            savedGamesListSel->onMousePressed();
            savedGamesBackZoomSel->onMousePressed();
            savedGamesNextZoomSel->onMousePressed();
            break;
        case State::SHOW_CREATE_GAME_ONLINE:
            createGameOlBackZS->onMousePressed();
            createGameOlNextZS->onMousePressed();
            break;
        case State::FADE_TRANSITION:
            break;
        }
    }
}

void MainMenu::onMouseWheelMoved(float amount, const raylib::Vector2 &pos)
{
    if (reconnecting) {
        /* Don't allow wheel scroll to do anything if reconnecting */
        return;
    }

    /* Handle wheel scroll per state of the menu */
    switch (currentState) {
    case State::INITIAL_FADE:
    case State::SHOW_TOPLEVEL:
    case State::SHOW_SETTINGS:
    case State::SHOW_ONLINE_NAME_INPUT:
    case State::SHOW_HOST_JOIN:
        break;
    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->onMouseWheelMoved(amount, pos);
        break;
    case State::FADE_TRANSITION:
        break;
    }
}


void MainMenu::onKeyPressed(int key)
{
    if (reconnecting) {
        /* Don't allow keypresses if reconnecting */
        return;
    }

    switch(currentState) {
    case State::SHOW_ONLINE_NAME_INPUT:
        if (key == KEY_ENTER) {
            triggerNameSubmission();
        } else {
            olNameTextBox->onKeyPressed(key);
        }
        break;
    case State::SHOW_CREATE_GAME_ONLINE:
        if (key == KEY_ENTER) {
            
        } else {
            createGameOlTextBox->onKeyPressed(key);
        }
        break;
    default:
        break;
    }
}

void MainMenu::onZoomSelectorClicked(ZoomSelector *source, int index)
{
    if (source == toplevelZoomSel) {
        /* Handle click for top level menu zoom selector */
        switch(index) {

        /* Play Local Game */
        case 0:
            initiateFadeToState(State::SHOW_SAVED_GAMES_LOCAL);
            break;

        /* Play Online */
        case 1:
            initiateFadeToState(State::SHOW_ONLINE_NAME_INPUT);
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
                true,   /* isFullscreen */
                0,      /* windowWidth */
                0       /* windowHeight */
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
    } else if (source == olNameSubmitZoomSel) {
        /* Handle click for "submit" on online name selection screen */
        triggerNameSubmission();
    } else if (source == olNameBackZoomSel) {
        /* Handle click for "back" on online name selection screen */
        cancelNameSubmission();
        initiateFadeToState(State::SHOW_TOPLEVEL);
    } else if (source == hostJoinZoomSel) {
        /* Handle click for host/join screen zoom selector */
        switch(index) {

        /* Host */
        case 0:
            initiateFadeToState(State::SHOW_SAVED_GAMES_ONLINE);
            break;

        /* Join */
        case 1:
            break;

        /* Back */
        case 2:
            initiateFadeToState(State::SHOW_ONLINE_NAME_INPUT);
            break;

        default:
            break;
        }
    } else if (source == savedGamesBackZoomSel) {
        /* Handle click for "Back" on saved games selection screen */
        if (currentState == State::SHOW_SAVED_GAMES_LOCAL) {
            initiateFadeToState(State::SHOW_TOPLEVEL);
        } else if (currentState == State::SHOW_SAVED_GAMES_ONLINE) {
            initiateFadeToState(State::SHOW_HOST_JOIN);
        }
    } else if (source == savedGamesNextZoomSel) {
        /* Handle click for "Next" on saved games selection screen */
        if (currentState == State::SHOW_SAVED_GAMES_LOCAL) {

        } else if (currentState == State::SHOW_SAVED_GAMES_ONLINE) {
            initiateFadeToState(State::SHOW_CREATE_GAME_ONLINE);
        }
    } else if (source == createGameOlBackZS) {
        /* Handle click for "Back" on create online game screen */
        initiateFadeToState(State::SHOW_SAVED_GAMES_ONLINE);
    } else if (source == createGameOlNextZS) {
        /* Handle click for "Next" on create online game screen */
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

void MainMenu::onListSelectorChanged(ListSelector *source, int index) {
    if (source == savedGamesListSel) {
        /* Handle change of selection for list of saved games on saved games submenu */
        if (index == -1) {
            /* No item selected. Disable "Next" button on this screen */
            savedGamesNextZoomSel->setEnabled(false);
        } else {
            /* Some item was selected. Enable "Next" button on this screen */
            savedGamesNextZoomSel->setEnabled(true);
        }
    }
}

void MainMenu::onTBEmptinessChanged(MenuTextBox *source, bool is_empty) {
    if (source == olNameTextBox) {
        /* Enable/disable "Submit" button on online name entry screen */
        olNameSubmitZoomSel->setEnabled(!is_empty);
    } else if (source == createGameOlTextBox) {
        /* Enable/disable "Next" button on online game create screen */
        createGameOlNextZS->setEnabled(!is_empty);
    }
}

void MainMenu::initiateFadeToState(State nextState)
{
    /* Opportunity to add init/reset functionality before a state is shown */
    switch (nextState) {
    case State::SHOW_ONLINE_NAME_INPUT:
        olNameShowTakenErr = false;
        olNameShowConnectErr = false;
        session->close();
        olNameLoading = false;
        olNameTextBox->setEnabled(true);
        olNameSubmitZoomSel->setEnabled(olNameTextBox->getContentLen() > 0);
        break;

    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->deselectAll(); /* Will result in cb that disables "Next" button */
        savedGamesListSel->scrollToTop(true);
        break;

    case State::SHOW_CREATE_GAME_ONLINE:
        if (currentState == State::SHOW_SAVED_GAMES_ONLINE) {
            /* If we are progressing forward to this page, clear old text */
            createGameOlTextBox->clear();
        }
        break;

    default:
        break;
    }

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

    case State::SHOW_ONLINE_NAME_INPUT:
        olNameTLOpacity = opacity;
        olNameTextBox->setDependentOpacity(opacity);
        olNameLoadSpinner->setDependentOpacity(opacity);
        olNameSubmitZoomSel->setDependentOpacity(opacity);
        olNameBackZoomSel->setDependentOpacity(opacity);
        break;

    case State::SHOW_HOST_JOIN:
        hostJoinZoomSel->setDependentOpacity(opacity);
        break;

    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->setDependentOpacity(opacity);
        savedGamesBackZoomSel->setDependentOpacity(opacity);
        savedGamesNextZoomSel->setDependentOpacity(opacity);
        break;

    case State::SHOW_CREATE_GAME_ONLINE:
        createGameOlTLOpacity = opacity;
        createGameOlTextBox->setDependentOpacity(opacity);
        createGameOlBackZS->setDependentOpacity(opacity);
        createGameOlNextZS->setDependentOpacity(opacity);
        break;

    default:
        break;
    }
}

void MainMenu::updateForState(State menuState, double secs)
{
    if (reconnecting) {
        /* If reconnecting, pause other states processing */
        reconLoadSpinner->update(secs);
        return;
    }

    switch(menuState) {
    
    case State::SHOW_TOPLEVEL:
        toplevelZoomSel->update(secs);
        break;

    case State::SHOW_SETTINGS:
        settingsApplyZoomSel->update(secs);
        settingsBackZoomSel->update(secs);
        resolutionBoxSel->update(secs);
        break;

    case State::SHOW_ONLINE_NAME_INPUT:
        olNameLoadSpinner->update(secs);
        olNameSubmitZoomSel->update(secs);
        olNameBackZoomSel->update(secs);

        if (olNameLoading){
            nameSubmitStopwatch += secs;
        }

        /* Name submit done but need to display loading wheel for min time */
        if (olNameSubmitResult != ServerSession::Event::NONE) {
            if (nameSubmitStopwatch >= OL_NAME_SUBMIT_MIN_LOAD_TIME) {
                handleNameSubmissionResult();
                olNameSubmitResult = ServerSession::Event::NONE;
            }
        }

        break;

    case State::SHOW_HOST_JOIN:
        hostJoinZoomSel->update(secs);
        break;

    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->update(secs);
        savedGamesBackZoomSel->update(secs);
        savedGamesNextZoomSel->update(secs);
        break;

    case State::SHOW_CREATE_GAME_ONLINE:
        createGameOlBackZS->update(secs);
        createGameOlNextZS->update(secs);
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

    case State::SHOW_ONLINE_NAME_INPUT:
        olNameSubmitZoomSel->onMousePosUpdate(pos);
        olNameBackZoomSel->onMousePosUpdate(pos);
        break;

    case State::SHOW_HOST_JOIN:
        hostJoinZoomSel->onMousePosUpdate(pos);
        break;

    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->onMousePosUpdate(pos);
        savedGamesBackZoomSel->onMousePosUpdate(pos);
        savedGamesNextZoomSel->onMousePosUpdate(pos);
        break;

    case State::SHOW_CREATE_GAME_ONLINE:
        createGameOlBackZS->onMousePosUpdate(pos);
        createGameOlNextZS->onMousePosUpdate(pos);
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

    case State::SHOW_ONLINE_NAME_INPUT:
        renderer->drawTexture(textLabelTexture, TL_ENTER_A_NAME, olNameTLPromptDst,
                olNameTLOpacity);
        olNameTextBox->render(renderer);
        if (olNameLoading) {
            olNameLoadSpinner->render(renderer);
        }
        olNameSubmitZoomSel->render(renderer);
        olNameBackZoomSel->render(renderer);
        if (olNameShowConnectErr) {
            renderer->drawTexture(textLabelTexture, TL_CONNECT_FAILED, olNameTLConnectFailed,
                    olNameTLOpacity);
        } else if (olNameShowTakenErr) {
            renderer->drawTexture(textLabelTexture, TL_NAME_TAKEN, olNameTLTakenDst,
                    olNameTLOpacity);
        }
        break;

    case State::SHOW_HOST_JOIN:
        hostJoinZoomSel->render(renderer);
        break;

    case State::SHOW_SAVED_GAMES_LOCAL:
    case State::SHOW_SAVED_GAMES_ONLINE:
        savedGamesListSel->render(renderer);
        savedGamesBackZoomSel->render(renderer);
        savedGamesNextZoomSel->render(renderer);
        break;

    case State::SHOW_CREATE_GAME_ONLINE:
        renderer->drawTexture(textLabelTexture, TL_NEW_GAME_SAVE_FILE_NAME, createGameOlTLPromptDst,
                createGameOlTLOpacity);
        createGameOlTextBox->render(renderer);
        createGameOlBackZS->render(renderer);
        createGameOlNextZS->render(renderer);
        break;

    default:
        break;
    }
}

void MainMenu::onSessionEvent(ServerSession::Event event)
{
    switch (event) {
    case ServerSession::Event::CONNECTION_FAILED:
    case ServerSession::Event::NAME_ACCEPTED:
    case ServerSession::Event::NAME_REJECTED:
        olNameSubmitResult = event;
        break;

    case ServerSession::Event::CONNECTION_SUSPENDED:
        reconnecting = true;
        break;

    case ServerSession::Event::CONNECTION_RESUMED:
        reconnecting = false;
        break;

    case ServerSession::Event::CONNECTION_LOST:
        reconnecting = false;
        initiateFadeToState(State::SHOW_ONLINE_NAME_INPUT);
        break;

    default:
        break;
    }
}

void MainMenu::triggerNameSubmission()
{
    if (olNameTextBox->getContent() != "") {
        olNameLoading = true;
        olNameTextBox->setEnabled(false);
        olNameSubmitZoomSel->setEnabled(false);
        olNameShowConnectErr = false;
        olNameShowTakenErr = false;
        nameSubmitStopwatch = 0;
        session->open(olNameTextBox->getContent());
    }
}

void MainMenu::cancelNameSubmission()
{
    session->close();
    olNameLoading = false;
    olNameTextBox->setEnabled(true);
    olNameSubmitZoomSel->setEnabled(olNameTextBox->getContentLen() > 0);
    olNameShowConnectErr = false;
    olNameShowTakenErr = false;    
}

void MainMenu::handleNameSubmissionResult()
{
    switch(olNameSubmitResult) {
    case ServerSession::Event::CONNECTION_FAILED:
        olNameLoading = false;
        olNameTextBox->setEnabled(true);
        olNameSubmitZoomSel->setEnabled(olNameTextBox->getContentLen() > 0);
        olNameShowConnectErr = true;
        break;
    
    case ServerSession::Event::NAME_REJECTED:
        olNameLoading = false;
        olNameTextBox->setEnabled(true);
        olNameSubmitZoomSel->setEnabled(olNameTextBox->getContentLen() > 0);
        olNameShowTakenErr = true;
        break;

    case ServerSession::Event::NAME_ACCEPTED:
        initiateFadeToState(State::SHOW_HOST_JOIN);
        break;

    default:
        break;
    }
}
