#ifndef FD__LISTSELECTOR_H
#define FD__LISTSELECTOR_H

#include <functional>

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
    void setHeight(float height);

    /*
     * Set the width of this ListSelectorItem in pixels.
     * Should only be called by parent container (ListSelector) to share list width.
     */
    void setWidth(float width);

    /* Returns the current height of this ListSelectorItem */
    float getHeight();

    /* Returns the current width of this ListSelectorItem */
    float getWidth();

    /* Set the current y pos relative to the beginning of the list */
    void setYPosInList(float y);

    /* Get the current y pos relative to the beginning of the list */
    float getYPosInList();

    /* Sets whether or not this ListSelectorItem is hovered by the mouse */
    void setHover(bool hover);

    /* Checks whether or not this ListSelectorItem is hovered by the mouse */
    bool getHover();

    /* Sets whether or not this ListSelectorItem is selected within the list */
    void setSelected(bool selected);

    /* Checks whether or not this ListSelectorItem is selected within the list */
    bool getSelected();

    /* Sets the current index in the ListSelector (should only be used by ListSelector) */
    void setIdxInList(int idx);

    /* Gets the current index of this item in the ListSelector */
    int getIdxInList();

    /* 
     * Render to screen (should be called once per game loop).
     * absoluteX, absoluteY is the point on-screen where the item should render itself.
     * This value is computed by the ListSelector.
     */
    virtual void render(Renderer *renderer, float absoluteX, float absoluteY) = 0;  

 protected:

    /* 
     * Function called after height of this ListSelectorItem is changed. Can be overridden by
     * subclasses if they need to implement class-specific functionality in reponse to a height
     * change. When this function is called, the internal height will already reflect the new
     * height.
     */
    virtual void heightChanged() {}

 private:

    /* Height/Width of this ListSelectorItem in pixels */
    float height;
    float width;

    /* The current y pos relative to the start of the list */
    float yPosInList;

    /* Tracks whether or not this List Selector Item is currently being hovered by the mouse */
    bool hovered;

    /* Tracks whether or not this List Selector Item is currently selected */
    bool selected;

    /* Tracks the index of this item within the ListSelector (first item is 0) */
    int idxInList;
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

    /* Sets the on-screen position (in px) of the top left corner of this ListSelector */
    void setX(float x);
    void setY(float y);

    /* Sets the width of the actual list pane (what visible width it occupies on-screen) in px */
    void setWidth(float width);

    /* Sets the height of the actual list pane (what visible height it occupies on-screen) in px */
    void setHeight(float height);

    /* Getters for the width and height */
    float getWidth();
    float getHeight();

    /*
     * Sets the height of the separator rendered between each Item.
     * itemHeightNext should be true ONLY if the setItemHeight() function call will immediately
     * follow this one. Otherwise it should be false.
     */
    void setSeparatorHeight(float height, bool itemHeightNext);

    /* 
     * Sets the height of all items in this ListSelector (and all items added in the future).
     * If a value of LISTSELECTOR_INDIV_HEIGHTS is given, the height is controlled by each
     * individual ListSelectorItem, rather than using a consistent value across all items.
     */
    void setItemHeight(float height);

    /*
     * Registers the given function as a callback for when the currently selected item changes.
     * The given function will receive a pointer to the ListSelector that generated the
     * event and the index of the item that was selected (or -1 if no item is selected now).
     */
    void setSelectionCallback(std::function<void(ListSelector*, int)> cb);

    /*
     * Called to update the scroll state of this ListSelector. Should be called by the containing
     * scene or context. The amount is the amount of scroll that occurred, and the pos is the
     * current position of the mouse.
     */
    void onMouseWheelMoved(float amount, const raylib::Vector2 &pos);

    /*
     * Handle mouse position update from window - this function looks for
     * a collision with each list selector item to determine if a hover graphic needs to take place
     */ 
    void onMousePosUpdate(const raylib::Vector2 &pos);

    /*
     * Called by the parent context when a mouse press occurs. The mouse press is assumed to have
     * occurred at the last mouse pos received in onMousePosUpdate.
     */
    void onMousePressed();

    /* Returns the index of the currently selected item, or -1 if no item is currently selected */
    int getSelectedItemIdx();

    /* Unselect all items in the ListSelector */
    void deselectAll();

    /* Move scroll position to the top of the ListSelector. If instant is false, animate it */
    void scrollToTop(bool instant);

    /* Override OpacityDependent variant of function so we can notify items in list of opacity */
    void setDependentOpacity(float opacity);

    /* Update any time-based state and animations */
    void update(double secs);

    /* Render to screen (should be called once per game loop) */
    void render(Renderer *renderer);

 private:

    /* The vector of ListSelectorItems currently in the list. */
    std::vector<ListSelectorItem*> items;

    /* The current height of the items in this ListSelector (in px) */
    float itemHeight;

    /* The current position of the top left corner of the ListSelector pane on the screen */
    float x;
    float y;

    /* The current dimensions of the actual visible ListSelector pane on the screen, in px */
    float width;
    float height;

    /* The current height of the separator rendered between each list element */
    float separatorHeight;

    /* The current total height of the list (including elements scrolled off screen) */
    float totalListHeight;

    /* How many pixels from the top is the ListSelector scrolled currently */
    float scrollAmount;

    /* What is the target scrollAmount? (We animate towards this value) */
    float targetScroll;

    /* A pointer to the currently hovered ListSelectorItem (or nullptr if none are hovered) */
    ListSelectorItem *hoveredItem;

    /* A pointer to the currently selected ListSelectorItem (or nullptr if none are selected) */
    ListSelectorItem *selectedItem;

    /* Callback function that will be called when the selection changes */
    std::function<void(ListSelector*, int)> selectionCallback;

    /* 
     * A helper function to find the associated ListSelectorItem based on a mouse pos.
     * Returns a pointer to the item if the mouse pos is within an item, else returns nullptr.
     */
    ListSelectorItem *getItemForMousePos(const raylib::Vector2 &pos);
};

/* Include the most basic subclass of ListSelectorItem - containing only a string of text */
class ListSelectorTextItem : public ListSelectorItem {
 public:

    /* Constructor - requires text up front (other version for later where text is added late?) */
    ListSelectorTextItem(float fontHeightPct, std::string &&text);

    /* Virtual destructor so we can polymorphically delete subclasses via their destructors */
    virtual ~ListSelectorTextItem();

    void render(Renderer *renderer, float absoluteX, float absoluteY) override;

 protected:

    /* Adjusts the font size according to the new height */
    void heightChanged() override;

 private:

    /* String holding the text that should be shown as the content of this list item */
    std::string text;

    /* Font size as a percentage of the current height of the item */
    float fontHeightPct;

    /* The current font size and font spacing in pixels */
    float fontSize;
    float fontSpacing;

    /* The current on-screen dimensions that will be occupied by text given the current size */
    raylib::Vector2 contentSize;

    /* The font used to draw the text inside this list item */
    raylib::Font *font;

    /* The textures used to draw this ListSelector to the screen */
    raylib::Texture *hoverTex;
    raylib::Texture *selectedTex;

    /* Helper function to call to recalculate the size of the rendered text content */
    void recalculateContentSize();
};

#endif