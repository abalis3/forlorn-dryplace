#include "MenuTextBox.h"

#include "Util.h"

static const char BG_RECT_IMG_PATH[] = "MenuTextBox/background.png";
static const char MARVEL_FONT_PATH[] = "Fonts/Marvel-Regular.ttf";

static const int BASE_FONT_SIZE = 175;



MenuTextBox::MenuTextBox(int maxChars)
{
    this->maxChars = maxChars;
    enabled = true;
    
    /* Initialize and alloc char array and set to empty string */
    content = new char[maxChars + 1];
    content[0] = '\0';
    numChars = 0;

    /* Load necessary resources */
    bgRectTexture = new raylib::Texture(Util::formResourcePath(BG_RECT_IMG_PATH));
    bgRectTexture->GenMipmaps();
    font = new raylib::Font(Util::formResourcePath(MARVEL_FONT_PATH), BASE_FONT_SIZE, nullptr, 95);
    GenTextureMipmaps(&font->texture);
}

MenuTextBox::~MenuTextBox()
{
    delete content;
    delete bgRectTexture;
    delete font;
}

void MenuTextBox::setSize(float width, float height)
{
    /* Force the width to be at least as large as the height */
    if (width >= height) {
        dstRect.width = width;
    } else {
        dstRect.width = height;
    }
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

void MenuTextBox::onKeyPressed(int key)
{
    /* Handle presses of 0-9, A-Z, and Backspace */
    if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z')) {
        if (numChars < maxChars) {
            content[numChars++] = (char) key;
            content[numChars] = '\0';
        }
    } else if (key == KEY_BACKSPACE && numChars > 0) {
        content[--numChars] = '\0';
    }
}

void MenuTextBox::render(Renderer *renderer)
{
    /* Draw left edge of bg rect */
    renderer->drawTexture(bgRectTexture, 0, 0, bgRectTexture->height / 2, bgRectTexture->height, 
            dstRect.x, dstRect.y, dstRect.height / 2, dstRect.height, getDependentOpacity());

    /* Draw middle segment of bg rect, stretched to fill space*/
    renderer->drawTexture(bgRectTexture, bgRectTexture->height / 2, 0,
            bgRectTexture->width - bgRectTexture->height, bgRectTexture->height,
            dstRect.x + (dstRect.height / 2), dstRect.y, dstRect.width - dstRect.height,
            dstRect.height, getDependentOpacity());

    /* Draw right edge of bg rect */
    renderer->drawTexture(bgRectTexture, bgRectTexture->width - (bgRectTexture->height / 2), 0,
            bgRectTexture->height / 2, bgRectTexture->height,
            dstRect.x + dstRect.width - (dstRect.height / 2), dstRect.y, dstRect.height / 2,
            dstRect.height, getDependentOpacity());

    renderer->setColor(WHITE);
    renderer->drawText(*font, content, dstRect.x, dstRect.y, fontSize, 0,
            getDependentOpacity());
}