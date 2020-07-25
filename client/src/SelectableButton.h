#ifndef FD__SELECTABLEBUTTON_H
#define FD__SELECTABLEBUTTON_H

#include <functional>

#include "OpacityDependent.h"
#include "Renderer.h"

/*
 * This class defines a UI component for a button that has two distinct states - selected and
 * unselected. It can be selected by being clicked on and will notify via a callback function
 * when it is selected. Its size is defined by a specific height in px and its width is
 * adjusted accordingly based on the height given and source texture aspect ratio
 */
class SelectableButton : public OpacityDependent {
 public:

    /*
     * Constructor - requires texture with image to draw, and rectangles that tell where
     * on the given source texture the selected and unselected texture content can be found.
     */
    SelectableButton(raylib::Texture *tex, const raylib::Rectangle &selectedSrc,
            const raylib::Rectangle &unselectedSrc, bool selected);

    /* Called to set the on-screen height of the SelectableButton, which decides its width too */
    void setHeight(float height);

    /* Called to set the position of the top left corner of the SelectableButton */
    void setX(float xPos);
    void setY(float yPos);

    /* Called to retrieve the on-screen width of this object, based on the height that was set */
    float getWidth();

    /* Set if this button is currently selected or not */
    void setSelected(bool selected);

    /* Get if the button is currently selected or not */
    bool isSelected();

    /*
     * Registers the given function as a callback for when this button gets selected.
     * The given function will receive a pointer to this button that generated the event.
     */
    void setCallback(std::function<void(SelectableButton*)> cb);

    /* Function to alert this SelectableButton of a mouse click so it can detect collisions */
    void onMousePressed(const raylib::Vector2 &pos);

    /* Render function to render to screen once per game loop */
    void render(Renderer *renderer);

 private:

    /* The texture holding the contents of this button's selected and unselected states */
    raylib::Texture *contentTexture;

    /* The rectangles holding the location in the content texture for the Button's appearance */
    raylib::Rectangle selectedSrc;
    raylib::Rectangle unselectedSrc;

    /*
     * The location on the screen that this SelectableButton occupes. This is where the texture
     * will be rendered and will also be used as the bounding box to detect click collisions
     */
    raylib::Rectangle dstRect;

    /* Is this SelectableButton currently selected? */
    bool selected;

    /* Callback function that will be called when the button is selected */
    std::function<void(SelectableButton*)> callback;
};

#endif