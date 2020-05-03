#ifndef FD__BOXSELECTOR_H
#define FD__BOXSELECTOR_H

#include "Renderer.h"
#include "OpacityDependent.h"

/*
 * This class describes an interface element that consists of a rectangular box with
 * text drawn inside it. It can be used to select between multiple items. The box displays one
 * item at a time. Items can be switched between by hovering the mouse over the selector
 * and scrolling the mouse wheel or by clicking two arrows on either side of the box.
 */
class BoxSelector : public OpacityDependent {
 public:

    /* Default constructor - prepares textures, init variables */
    BoxSelector();

    /* Destructor - free any memory created for this object */
    ~BoxSelector();

    /* 
     * Sets the height of this BoxSelector in px. This object is defined primarily by its height.
     * Setting the height triggers a recalculation of width and so the width should be
     * queried if this affects the owning object.
     */
    void setHeight(float height);

    /* Returns the on-screen width in pixels of the whole BoxSelector including arrow buttons */
    float getWidth();

    /* Set the on-screen position of the top-left corner of the BoxSelector */
    void setXPos(float xPos);
    void setYPos(float yPos);

    /* Called once per game loop to make the BoxSelector render itself */
    void render(Renderer *renderer);

 private:

    /* The texture for the background rectangle */
    raylib::Texture *bgRectTexture;

    /*
     * This rectangle defines the bounding/collision box for the whole BoxSelector,
     * including the arrows on either side of the text content
     */
    raylib::Rectangle boundingBox;

    /* Rectangle defining desired draw destination of background rectangle */
    raylib::Rectangle bgRectDst;

    /* Defines the max x/y size in pixels of arrow when at max zoom and when at min zoom */
    float arrowMaxDimension;
    float arrowMinDimension;

    /*
     * Defines the size in px of the space between the center of the arrow button and
     * the edge of the background rectangle texture
     */
    float arrowToBoxPadding;
};

#endif