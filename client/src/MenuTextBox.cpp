#include "MenuTextBox.h"

MenuTextBox::MenuTextBox(int maxChars)
{
    this->maxChars = maxChars;
    
    /* Initialize and alloc char array and set to empty string */
    content = new char[maxChars + 1];
    content[0] = '\0';
    numChars = 0;
}

MenuTextBox::~MenuTextBox()
{
    delete content;
}

void MenuTextBox::setSize(float width, float height)
{
    dstRect.width = width;
    dstRect.height = height;
    fontSize = height;
}

void MenuTextBox::setX(float xPos)
{
    dstRect.x = xPos;
}

void MenuTextBox::setY(float yPos)
{
    dstRect.y = yPos;
}

float MenuTextBox::getWidth()
{
    return dstRect.width;
}

float MenuTextBox::getHeight()
{
    return dstRect.height;
}

void MenuTextBox::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void MenuTextBox::render(Renderer *renderer)
{

}