#ifndef FD__MENUTEXTBOX_H
#define FD__MENUTEXTBOX_H

#include "OpacityDependent.h"
#include "Renderer.h"

/*
 * This class defines a UI component for a textbox on the main menu. It will have an outline
 * to identify the area it occupies and allow the user to type alphanumeric characters within it.
 * The API should be able to configure the height and width of the textbox and allow the font
 * size to be determined by virtue of the height. The caller must also set the maximum number
 * of characters that can be input at once.
 */
class MenuTextBox : public OpacityDependent {
 public:

    /*
     * Constructor - requires texture with image to draw, and rectangles that tell where
     * on the given source texture the selected and unselected texture content can be found.
     */
    MenuTextBox(int maxChars);

    /* Destructor to clean up memory */
    ~MenuTextBox();

    /* Called to set the on-screen size of the text box */
    void setSize(float width, float height);

    /* Called to set the position of the top left corner of the text box */
    void setX(float xPos);
    void setY(float yPos);

    /* Called to retrieve the on-screen size of this object */
    float getWidth();
    float getHeight();

    /* Get the current string contained in the text box */
    std::string getContent();

    /* Set if this text box is currently enabled or not */
    void setEnabled(bool enabled);

    /* Called by the main menu scene to notify the textbox of a relevant keypress event */
    void onKeyPressed(int key);

    /* Render function to render to screen once per game loop */
    void render(Renderer *renderer);

 private:

    /* The texture for the background rectangle */
    raylib::Texture *bgRectTexture;

    /* The font used to draw the content inside the text box */
    raylib::Font *font;

    /*
     * The location on the screen that this text box occupes. This is where the
     * background box will be rendered with the text contained inside.
     */
    raylib::Rectangle dstRect;

    /* The buffer holding the current number of characters in the text box */
    char *content;

    /* The number of characters (not including \0 terminator) in the text input currently */
    int numChars;

    /* The current on-screen width that will be occupied by content with current textbox size */
    raylib::Vector2 contentSize;

    /* The font size / spacing parameters to render with */
    float fontSize;
    float fontSpacing;

    /* The maximum number of characters this text box may hold */
    int maxChars;

    /* Is this text box currently enabled? */
    bool enabled;

    /* Helper function to call to recalculate the size of the rendered text content */
    void recalculateContentSize();
};

#endif