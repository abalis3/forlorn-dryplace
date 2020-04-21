#ifndef FD__ZOOMSELECTOR_H
#define FD__ZOOMSELECTOR_H

#include <string>
#include <raylib/raylib.hpp>

#include "Renderer.h"

/*
 * ZoomSelector is a class describing a group of clickable options that will
 * provide a nice zoom effect when each is hovered over. It is much like the
 * magnify effect implemented by the dock on the bottom of osx desktops.
 * It is intended for use on each screen of the main menu in Forbidden Desert
 */
class ZoomSelector {
 public:

    /* 
     * Constructor - file at texturePath contains image with all strings that
     * will be displayed as options within this ZoomSelector.
     * hoverZoomRatio - ratio of the largest an item gets displayed to the
     * smallest (1.0 means no difference, 2.0 means twice as big).
     */
    ZoomSelector(const std::string &texturePath, float hoverZoomRatio);

    /* Destructor - frees all resources in use by this ZoomSelector */
    ~ZoomSelector();

    /* 
     * Call to set the position parameters of the Zoom Selector
     * maxHeight - the sizing of the ZoomSelector should be such that this is
     * the maximum height possible (occurs when hovering an item in the middle)
     * itemPadding - the padding (in pixels) between each item in the selector.
     */
    void updatePosition(float maxHeight, float centerXPos, float centerYPos,
            float itemPadding);

    /* 
     * Adds an item (in-order) to the zoom selector whose content in the
     * original texturePath (see constructor) occupies the rectangle defined
     * by these parameters
     */
    void addItem(const raylib::Rectangle &srcRect);

    /* Update the ZoomSelector with elapsed time since last call */
    void update(double secs);

    /*
     * Handle mouse position update from window - this function looks for
     * a collision with each ZoomScroll item to determine if a hover
     * zoom should take place
     */ 
    void onMousePosUpdate(const raylib::Vector2 &pos);

    /* Renders the ZoomSelector to the screen */
    void render(Renderer *renderer);

private:

    /* Texture containing the content of all items in this ZoomSelector */
    raylib::Texture *contentTexture;

    /* The max height for the whole ZoomSelector (with hover active) */
    float maxHeight;

    /* The max width of the whole ZoomSelector (used for mouse collisions) */
    float maxWidth;

    /* The ratio specifying optionMaxHeight : optionMinHeight */
    float hoverZoomRatio;

    /* The number of pixels of padding between two items on the screen */
    float itemPadding;

    /* The screen coordinates to render the CENTER of the ZoomSelector at */
    float centerXPos;
    float centerYPos;

    /* Struct representing a single item in the ZoomSelector */
    struct _item {
        
        /*
         * The rectangle containing the content in the
         * source texture of this ZoomSelector
         */
        raylib::Rectangle srcRect;

        /* The destination rectangle on the screen to render the item to */
        raylib::Rectangle dstRect;

        /* The collision vertical component on screen for mouse collisions */
        float collisionY;
        float collisionH;

        /* When this item is its max size (hovered) */
        float maxW;
        float minX;

        /* When this item is its min size (no hover) */
        float minW;
        float maxX;

        /* Keeps track of the target zoom. 0 = min size, 1 = max size */
        float targetZoomPercent;

        /* Keeps track of the current zoom. 0 = min size, 1 = max size */
        float zoomPercent;

        /* Linked list pointer (points towards last item in ZoomSelector) */
        struct _item *next;

    };
    /* Typedef to give the struct a nice name */
    typedef struct _item ZoomSelectorItem;

    /* Singly queue of items in this ZoomSelector (first item at head) */
    ZoomSelectorItem *itemListHead, *itemListTail;

    /* The number of items in this ZoomSelector */
    int numItems;

    /* The min height for an item (not near a hovered item) */
    float itemMinHeight;

    /* The max height for an item (this item is being hovered) */
    float itemMaxHeight;

    /* The animation destination value of expansionCenter */
    float targetExpansionCenter;

    /* The vertical coordinate from which zoomed items expand outward */
    float expansionCenter;

    /* Calculate sizing of items based on # items, max height, and zoom ratio */
    void recalculateSizeParams();

    /* 
     * Recursive helper that steps through item list to assign item Y positions
     * based on current expansion center and heights of each item in list.
     * If a node exists before listHead AND the bottom Y position of that node
     * is known, it will be placed in yPos and prevNodeSolved == true.
     * Returns the determined dstY of listHead.
     */
    float populateItemListDstYValues(ZoomSelectorItem *listHead,
            bool prevNodeSolved, float yPos);
};

#endif