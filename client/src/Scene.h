#ifndef FD__SCENE_H
#define FD__SCENE_H

#include "Renderer.h"

/* 
 * Abstract class to represent a distinct screen in the game intended to
 * be shown as one whole entity (not multiple scenes at once).
 * It will be triggered to render and update and it is its responsibility to
 * render and update itself and any sub-objects it may contain at that time.
 */
class Scene {
 public:
    
    /* 
     * Called once per game loop. This function must be
     * overridden by specific scenes to update the scene and its objects
     * given the amount of time that has passed since the last loop.
     */
    virtual void update(double secs) = 0;

    /* 
     * Called once per game loop. This function must be overridden by
     * specific scenes to render the scene and any objects it
     * may contain using the given renderer context
     */
    virtual void render(Renderer *renderer) = 0;

    /*
     * Callback that can be overridden by specific scenes subclassing Scene
     * to perform additional updates when the scene has its size changed.
     * The getWidth() and getHeight() functions will return the new size
     * values when this function gets called. The parameters are the old size
     */
    virtual void onSizeChangedFrom(int oldWidth, int oldHeight) {}

    /*
     * Called on each frame by the Window on the focused Scene so that it can
     * handle any mouse input to it
     */
    virtual void onMousePosUpdate(const raylib::Vector2 &pos) {}

    /*
     * To be called by the window if a mouse button was pressed to
     * notify the focused scene so it can handle it
     */
    virtual void onMouseButtonPressed(int button, const raylib::Vector2 &pos) {}

    /*
     * To be called by the window if a mouse button was released to
     * notify the focused scene so it can handle it
     */
    virtual void onMouseButtonReleased(int button,
            const raylib::Vector2 &pos) {}

    /*
     * To be called by the window when the scene is first shown
     * and whenever the window gets resized, to ensure the scene
     * adjusts its size accordingly
     */
    void updateWindowSize(int newWidth, int newHeight);

 protected:

    /* Get the current width of this Scene */
    int getWidth();

    /* Get the current height of this Scene */
    int getHeight();

private:

    /* The current width of this Scene */
    int width;

    /* The current height of this Scene */
	int height;
};

#endif