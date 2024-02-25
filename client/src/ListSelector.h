#ifndef FD__LISTSELECTOR_H
#define FD__LISTSELECTOR_H

#include "OpacityDependent.h"
#include "Renderer.h"

static const float LISTSELECTOR_INDIV_HEIGHTS = 0;

/*
 * This file defines a UI object that consists of a scrollable pane containing many list
 * "Items". An item may be selected by clicking on it. The item itself is an abstract class
 * which may be overridden with different implementations for different types of content that may
 * need to be represented in an item. One basic subclass of the ListSelectorItem is provided in
 * this file which contains a simple text string.
 */

class ListSelectorItem : public OpacityDependent {
 public:
    
    /* Default constructor to initialize fields as needed */
    ListSelectorItem();

    /* Virtual destructor so we can polymorphically delete subclasses via their destructors */
    virtual ~ListSelectorItem() {};

    /*
     * Set the height of this ListSelectorItem in pixels.
     * Typically called by parent container (ListSelector) to tell this item what height
     * it has been granted by the container.
     */
    virtual void setHeight(float height);

    /*
     * Set the width of this ListSelectorItem in pixels.
     * Should only be called by parent container (ListSelector) to share list width.
     */
    virtual void setWidth(float width);

    /* Returns the current height of this ListSelectorItem */
    float getHeight();

    /* Returns the current width of this ListSelectorItem */
    float getWidth();

    /* Render to screen (should be called once per game loop) */
    virtual void render(Renderer *renderer) = 0;  

 private:

    /* Height/Width of this ListSelectorItem in pixels */
    float height;
    float width;
};

class ListSelector : public OpacityDependent {
 public:

    /* Constructor - initialize fields */
    ListSelector();

    /* Destructor - free any memory created for this object */
    ~ListSelector();

    /*
     * Append the given ListSelectorItem to the end of the list.
     * The given item pointer must point to a valid (non-NULL) ListSelectorItem.
     * This collection type takes ownership of the ListSelectorItem so the caller should no longer
     * attempt to use its reference to the item or free the item.
     */
    void appendItem(ListSelectorItem *item);

    /* 
     * Sets the height of all items in this ListSelector (and all items added in the future).
     * If a value of LISTSELECTOR_INDIV_HEIGHTS is given, the height is controlled by each
     * individual ListSelectorItem, rather than using a consistent value across all items.
     */
    void setItemHeight(float height);

    /* Sets the width of the actual list pane (what visible width it occupies on-screen) in px */
    void setWidth(float width);

    /* Sets the height of the actual list pane (what visible height it occupies on-screen) in px */
    void setHeight(float height);

    /* Update any time-based state and animations */
    void update(double secs);

    /* Render to screen (should be called once per game loop) */
    void render(Renderer *renderer);

 private:

    /* The vector of ListSelectorItems currently in the list. */
    std::vector<ListSelectorItem*> items;

    /* The current height of the items in this ListSelector (in px) */
    float itemHeight;

    /* The current dimensions of the actual visible ListSelector pane on the screen, in px */
    float width;
    float height;
};

/* Include the most basic subclass of ListSelectorItem - containing only a string of text */
class ListSelectorTextItem : public ListSelectorItem {
 public:

    /* Constructor - requires text up front (other version for later where text is added late?) */
    ListSelectorTextItem(float fontHeightPct, std::string &&text);

    /* Sets the height of the Item and adjusts the font size accordingly */
    void setHeight(float height) override;

 private:

    /* String holding the text that should be shown as the content of this list item */
    std::string text;

    /* Font size as a percentage of the current height of the item */
    float fontHeightPct;

    /* The actual font size in pixels */
    float fontSize;
};

#endif 