#ifndef FD__LOADINGSPINNER_H
#define FD__LOADINGSPINNER_H

#include "OpacityDependent.h"
#include "Renderer.h"

/*
 * This class defines a UI component for a spinning loading icon. It can be set a size which
 * will be used as the width and height (square dimension) and an x/y position. It will also
 * require to be updated each game loop with its update function to animate the spin.
 */
class LoadingSpinner : public OpacityDependent {
 public:

    /* Default, parameter-less constructor */
    LoadingSpinner();

    /* Destructor to clean up memory */
    ~LoadingSpinner();

    /* Called to set the on-screen size of the loading spinner in px */
    void setSize(float px);

    /* Called to set the position of the center point of the loading spinner */
    void setX(float xPos);
    void setY(float yPos);

    /* Called to retrieve the on-screen size of this object */
    float getSize();

    void update(double secs);

    /* Render function to render to screen once per game loop */
    void render(Renderer *renderer);

 private:

    /* The texture for the spinner */
    raylib::Texture *spinnerTex;

    /*
     * The location on the screen that this loading spinner occupies.
     */
    raylib::Vector2 centerPos;
    float size;

    /* The current rotation of this spinner */
    float rotation;
};

#endif