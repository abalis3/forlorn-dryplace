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

    /* Adds an item (in-order) to the BoxSelector with given text content. */
    void addItem(const std::string &content, bool selected);

    /*
     * Handle mouse position update from window - this function looks for
     * a collision with each arrow button to determine if a hover
     * zoom should take place
     */ 
    void onMousePosUpdate(const raylib::Vector2 &pos);

    /* Called once per game loop to update the animations of the box selector */
    void update(double secs); 

    /* Called once per game loop to make the BoxSelector render itself */
    void render(Renderer *renderer);

 private:

    /* The texture for the background rectangle */
    raylib::Texture *bgRectTexture;

    /* The texture for the arrow buttons on either side of the selector */
    raylib::Texture *arrowTexture;

    /* The font used to draw the content of each option in the box */
    raylib::Font *font;

    /*
     * This rectangle defines the bounding/collision box for the whole BoxSelector,
     * including the arrows on either side of the text content
     */
    raylib::Rectangle boundingBox;

    /* Rectangle defining desired draw destination of background rectangle */
    raylib::Rectangle bgRectDst;

    /* Defines the x/y size in pixels of arrow at different zoom levels */
    float arrowMaxDimension;
    float arrowMinDimension;
    float leftArrowCurDimension;
    float rightArrowCurDimension;
    float leftArrowTargetDimension;
    float rightArrowTargetDimension;

    /*
     * Defines the size in px of the space between the center of the arrow button and
     * the edge of the background rectangle texture
     */
    float arrowToBoxPadding;

    /* Font size params given the last update to the height of the BoxSelector */
    float fontSize;
    float fontSpacing;

    /* Struct representing a single entry in the BoxSelector */
    struct _item {
        /* The text content for this item */
        std::string text;

        /* The size of the rendered text on screen for this item*/
        raylib::Vector2 renderSize;

        /* The index in the list of items (to be returned by getSelectedIndex) */
        int index;

        /* Linked list pointer (points towards last item in BoxSelector) */
        struct _item *next;
    };

    /* Typedef to give the struct a nice name */
    typedef struct _item BoxSelectorItem;

    /* Singly queue of items in this BoxSelector (first item at head) */
    BoxSelectorItem *itemListHead, *itemListTail;

    /* Pointer to the currently selected item in the list */
    BoxSelectorItem *selectedItem;

    /* The next item added will get this index */
    int nextItemIndex;
};

#endif