#ifndef FD__MAINMENU_H
#define FD__MAINMENU_H

#include <functional>

#include "Window.h"
#include "Scene.h"
#include "ZoomSelector.h"
#include "BoxSelector.h"
#include "SelectableButton.h"
#include "MenuTextBox.h"
#include "LoadingSpinner.h"
#include "ServerSession.h"

const int MENU_RECON_TIME_STR_BUF_SIZE = 4; /* Max # chars (+ nullterm) for rem secs string rep */

class MainMenu : public Scene {
 public:

    /* States that will be given to the game runner to tell it when to move from this scene */
    enum class ReturnCode {
        KEEP_RUNNING,
        EXIT_PROGRAM,
    };

    /*
     * Constructor - inits necessary values and loads textures.
     * window is a reference to the current window at menu load time
     */ 
    MainMenu(Window &window);

    /* Destructor - frees any memory used by MainMenu */
    ~MainMenu();
    
    /* Updates the animations based on the time passed */
    void update(double secs) override;

    /* Renders the main menu to the screen */
    void render(Renderer *renderer) override;

    /* 
     * Adjusts necessary positioning of objects within main menu
     * if screen size is changed
     */
    void onSizeChangedFrom(int oldWidth, int oldHeight) override;

    /*
     * Receives a mouse postiion update from the Window and
     * forwards the info onto relevant sub-objects
     */
    void onMousePosUpdate(const raylib::Vector2 &pos) override;

    /*
     * Receives a mouse button press event from the window and forwards
     * it to relevant sub-objects
     */
    void onMouseButtonPressed(int button, const raylib::Vector2 &pos) override;

    /*
     * Receives a notification that a key with given key code has been depressed
     * so that it may be forwarded to relevant sub-objects
     */
    void onKeyPressed(int key) override;

    /* Called by the Game runner to see if the scene should keep running or transition */
    ReturnCode getReturnCode();

    /*
     * Can be called by the owner of this MainMenu object to render a callback. The
     * callback will be called when the settings submenu applies changes requesting
     * a new graphics settings configuration.
     */
    void setWindowRequestCallback(std::function<void(const WindowConfiguration&)> callback);

 private:

    /* Texture for the background desert image */
    raylib::Texture *bgTexture;

    /* Texture for the main title "Forbidden Desert" image */
    raylib::Texture *titleTexture;

    /* Texture for text labels on various screens in the menu */
    raylib::Texture *textLabelTexture;

    /* Texture holding the contents of all SelectableButton objects */
    raylib::Texture *selButtonTexture;

    /* Texture holding the background gradient for the reconnecting overlay */
    raylib::Texture *reconOverlayBgTexture;

    /* Texture holding the "Attempting to Reconnect..." text for the reconnecting overlay */
    raylib::Texture *reconTextTexture;

    /* Marvel font */
    raylib::Font *marvelFont;

    /* Stores the next update that will be conveyed to the game runner */
    ReturnCode nextReturnCode;

    /* The callback to call when a change in graphics/resolution is required */
    std::function<void(const WindowConfiguration&)> windowRequestCallback;

    /************************************************************
     * Fields for tracking for background texture pan animation *
     ************************************************************/

    /*
     * The max x coordinate to sample from off the background image
     * without the render extending past the right border of the image
     */
    float bgSrcXMax;
    
    /* Float between 0 and 1 - how far along is background image in its pan */
    float bgSrcXPercent;

    /* Current x coordinate of source background texture to render frame off */
    float bgSrcXPos;

    /* 
     * The width in pixels that will be rendered from the background texture
     * and mapped to fill the width of the screen when rendered
     */
    float bgSrcWidth;
    
    /* True if background appears sliding to the left, false if going right */
    bool bgSrcXPosIncreasing;

    /*********************************************************************
     * Fields for tracking size and fade-in animation of main title text *
     *********************************************************************/

    /* 
     * These 4 properties specify the destination rectangle on the screen
     * that the main title text will be rendered to occupy
     */
    float titleXPos;
    float titleYPos;
    float titleWidth;
    float titleHeight;
    
    /* Between 0 and 1 - The current opacity of the main title */
    float titleOpacity;

    /* Time elapsed accumulator to track how far into title fade-in we are */
    float fadeStopwatch;

    /* 
     * Calculates the size/position parameters for the background image
     * pan animation. Called when scene dimensions are set or updated
     */
    void calculateBgSizeParams();

    /*
     * Calculates the size parameters for the main title image.
     * Called when scene dimensions are set or updated
     */
    void calculateTitleSizeParams();

    /*********************************************************************
     * Fields for tracking items on the reconnecting overlay             *
     *********************************************************************/

    /* 
     * These 4 properties specify the destination rectangle on the screen
     * that the "Attempting to reconnect" text will appear in.
     */
    float reconTextXPos;
    float reconTextYPos;
    float reconTextWidth;
    float reconTextHeight;

    /* Last time we checked the remaining connection "suspended" time, what was it? */
    int reconLastRemTime;

    /* Stringified representation of most recent "suspended" time */
    char reconRemTime[MENU_RECON_TIME_STR_BUF_SIZE];

    /* Specifies the font size that should be used to render the remaining suspended time*/
    float reconTimeFontSize;

    /* Specifies the width that would be occupied on-screen when rendering recon suspended time */
    float reconTimeRenderWidth;

    /* Calculates the size params related to items on the reconnecting overlay */
    void calculateReconOverlaySizeParams();

    /* Calculates the size of the recon overlay render width (when text changes or sizing) */
    void calculateReconTimeTextSize();

    /****************************************************************
     * Fields for dealing with submenus and the content within them *
     ****************************************************************/

    /* States corresponding to the current sub-menu being displayed by the main menu */\
    enum class State {
        INITIAL_FADE,
        FADE_TRANSITION,
        SHOW_TOPLEVEL,
        SHOW_SETTINGS,
        SHOW_ONLINE_NAME_INPUT,
        SHOW_HOST_JOIN,
    };

    /* Stores the current State being shown by the menu */
    State currentState;

    /* 
     * When in the transition fade state, we are fading to and from another state.
     * These fields store these states so we can update their objects accordingly
     */
    State fadeFromState;
    State fadeToState;

    /* Zoom selector for top level menu options */
    ZoomSelector *toplevelZoomSel;

    /* Zoom selector for settings submenu "Apply" button */
    ZoomSelector *settingsApplyZoomSel;

    /* Zoom selector for settings submenu "Back" button */
    ZoomSelector *settingsBackZoomSel;

    /* Rectangles to define where on screen the settings screen text labels should render to */
    raylib::Rectangle settingsTLWindowModeDst;
    raylib::Rectangle settingsTLResolutionDst;

    /* Opacity variable for current opacity of text labels on settings submenu */
    float settingsTLOpacity;

    /* SelectableButtons for settings page window mode options */
    SelectableButton *windowedSelButton;
    SelectableButton *fullscreenSelButton;

    /* BoxSelector for resolution option on settings menu */
    BoxSelector *resolutionBoxSel;

    /* Rectangle defining where on screen the name-input "Enter a name" text label should render */
    raylib::Rectangle olNameTLPromptDst;

    /* Text box to enter name within on online name-input screen */
    MenuTextBox *olNameTextBox;

    /* LoadingSpinner for online name-input screen. Boolean tracks if it's visible */
    LoadingSpinner *olNameLoadSpinner;
    bool olNameLoading;

    /* Rectangle for destination on screen of 'name already taken' text label */
    bool olNameShowTakenErr; /* Should the 'taken' error be visible? */
    raylib::Rectangle olNameTLTakenDst;

    /* Rectangle for destination on screen of 'failed to connect' text label */
    bool olNameShowConnectErr; /* Should the 'connect failed' error be visible? */
    raylib::Rectangle olNameTLConnectFailed;

    /* Zoom selector for online name-input screen "Submit" button */
    ZoomSelector *olNameSubmitZoomSel;

    /* Zoom selector for online name-input screen "Back" button */
    ZoomSelector *olNameBackZoomSel;

    /* Opacity variable for current opacity of text labels on the online name-input submenu */
    float olNameTLOpacity;

    /* Online Session instance */
    ServerSession *session;

    /* Callback functions to be used by the server session to notify of events */
    void onSessionEvent(ServerSession::Event event);

    /* Stopwatch to count how long loading wheel has been spinning */
    float nameSubmitStopwatch;

    /* Zoom selector for host/join screen options */
    ZoomSelector *hostJoinZoomSel;

    /* Bool to track whether the conn is suspended and we are trying to reconnect */
    bool reconnecting;

    /* LoadingSpinner shown when connection suspended and trying to reconnect */
    LoadingSpinner *reconLoadSpinner;

    /* Callback to be registered for when a ZoomSelector gets clicked */
    void onZoomSelectorClicked(ZoomSelector *source, int index);

    /* Callback to be registered for when a SelectableButton gets clicked */
    void onSelectableButtonSelected(SelectableButton *source);

    /*
     * Initiate the fade to another submenu state. This includes setting the from and to states,
     * and entering the FADE_TRANSITION state
     */
    void initiateFadeToState(State nextState);

    /*
     * Set the opacity of all OpacityDependent subobjects from the provided menu state.
     * This is used to fade all objects in a given submenu in unison
     */
    void setOpacityForState(State menuState, float opacity);

    /*
     * Update all the objects in a given state (specifically submenu states)
     */
    void updateForState(State menuState, double secs);

    /* Update the mouse position for a given state (specifically submenu states) */
    void updateMousePosForState(State menuState, const raylib::Vector2 &pos);

    /* Renders all objects necessary for the given state (specifically submenu states) */
    void renderForState(State menuState, Renderer *renderer);

    /* Initiates the sequence of validating the input online-name */
    void triggerNameSubmission();

    /* 
     * Holds the currently queued name submission result for while the loading wheel is
     * still spinning. Holds Event::NONE if no event queued.
     */
    ServerSession::Event olNameSubmitResult;

    /* 
     * Triggered when name submission is complete (and loading wheel has spun minimum).
     * This function takes the next visual step (displaying error, moving to next screen, etc.)
     */
    void handleNameSubmissionResult();

    /* Triggered when name submission is complete and fail (and loading wheel has spun maximum) */
    void handleNameSubmissionFail();

};

#endif