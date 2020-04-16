#ifndef FD__RENDERER_H
#define FD__RENDERER_H

#include <raylib/raylib.hpp>

/*
 * This class represents the render context for a window
 * and exposes functions for rendering different objects
 */
class Renderer {
 public:
    
    /* No parameter constructor, initializes values */
    Renderer();

    /* Destructor, releases objects used */
    ~Renderer();

    /* Should be called at the start of each frame's render cycle */
    void start();

    /* Should be callled at the end of each frame's render cycle */
    void stop();

    /* Sets the color to be used by the renderer object */
    void setColor(Color color);

    /* Clears the whole window with the selected color */
    void clearBackground();

    /* Draws a rectangle with given x,y coordinates, width, and height */
    void drawRectangle(int x, int y, int w, int h);

    /* Draws the given string of text at position x,y with given font size */
    void drawText(const char *text, int x, int y, int fontSize);

    /* Draws the given std::string at position x,y with given font size */
    void drawText(std::string text, int x, int y, int fontSize);

    /* Gets the width in pixels of the rendered text with given font size */
    int measureText(const char *text, int fontSize);

    /* Gets the width in pixels of the rendered text with given font size */
    int measureText(std::string text, int fontSize);

 private:

    /* The current color that will be used in drawing operations */
    raylib::Color *currentColor;
};

#endif