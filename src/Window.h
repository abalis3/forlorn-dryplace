#ifndef FD__WINDOW_H
#define FD__WINDOW_H

#include <raylib/raylib.hpp>

#include "Scene.h"

/*
 * Used to represent window configuration settings
 * for saving/loading window configuration
 */
struct WindowConfiguration
{
    /* Is the window fullscreen (true) or windowed (false) */
    bool isFullscreen;

    /* Is vsync enabled for the window */
    bool vsyncEnabled;

    /* What is the FPS cap (-1 for uncapped) */
    int targetFPS;

    /* The width of the window, in pixels (0 for monitor resolution) */
    int windowWidth;

    /* The height of the window, in pixels (0 for monitor resolution) */
    int windowHeight;
};

class Window {
 public:
    /*
     * Default constructor - create window for fullscreen display
     * on primary monitor with native monitor resolution
     */
    Window();

    /*
     * Constructor to create window with specified configuration
     */
    Window(const struct WindowConfiguration *config);

    /* Default destructor - destroy this window */
    ~Window();

    /* Return the current width of this window in pixels */
    int getWidth();

    /* Return the current height of this window in pixels */
    int getHeight();

    /* Get the current approximate FPS of the window */
    int getFPS();

    /* 
     * Return true when window has received user input request
     * to exit (like clicking the X), otherwise false.
     */
    bool shouldClose();

    /*
     * Change the window to focus the given scene, triggering an update
     * of its size to match the window's, and rendering it for each
     * frame as long as it is the scene set in this window */
    void flipToScene(Scene *newScene);

    /*
     * Should be called once per game loop to update window
     */
    void update(double secs);

    /*
     * Should be called once per game loop to render window
     * and the current scene, if one is present
     */
    void renderFrame();

 private:
    
    /* Meat and potatoes of both possible constructors lives here */
    void constructWindow(const struct WindowConfiguration *config);

    /* The underlying raylib window instance */
    raylib::Window *raylibWindow;

    /* The scene currently being shown in the window */
    Scene *currentScene;

    /* The Renderer context used by the window */
    Renderer renderer;
};

#endif