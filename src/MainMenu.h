#ifndef FD__MAINMENU_H
#define FD__MAINMENU_H

#include "Scene.h"
#include "ZoomSelector.h"

class MainMenu : public Scene {
 public:

    /* States that will be given to the game runner to tell it when to move from this scene */
    enum class ReturnCode {
        KEEP_RUNNING,
        EXIT_PROGRAM,
    };

    /* Default constructor - inits necessary values and loads textures */ 
    MainMenu();

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

    /* Called by the Game runner to see if the scene should keep running or transition */
    ReturnCode getReturnCode();

 private:

    /* States corresponding to the current sub-menu being displayed by the main menu */\
    enum class State {
        INITIAL_FADE,
        FADE_TRANSITION,
        SHOW_TOPLEVEL,
    };

    /* Stores the current State being shown by the menu */
    State currentState;

    /* Stores the next execution update that will be conveyed to the game runner */
    ReturnCode nextReturnCode;

    /* Texture for the background desert image */
    raylib::Texture *bgTexture;

    /* Texture for the main title "Forbidden Desert" image */
    raylib::Texture *titleTexture;

    /* Zoom selector for top level menu options */
    ZoomSelector *toplevelZoomSel;

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

    /* Callback to be registered for when a ZoomSelector gets clicked */
    void onZoomSelectorClicked(ZoomSelector *source, int index);

};

#endif