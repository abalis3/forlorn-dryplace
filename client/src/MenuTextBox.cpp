#include "MenuTextBox.h"

#include "Util.h"

static const char BG_RECT_IMG_PATH[] = "MenuTextBox/background.png";
static const char MARVEL_FONT_PATH[] = "Fonts/Marvel-Regular.ttf";

static const int BASE_FONT_SIZE = 175;
static const float FONT_SIZE_SCALE = 0.8; /* pct of height of box for font height */
static const float FONT_SPACING_PCT = 0.05; /* pct of height of box for font spacing */


MenuTextBox::MenuTextBox(int maxChars)
{
    /* Load necessary resources */
    bgRectTexture = new raylib::Texture(Util::formResourcePath(BG_RECT_IMG_PATH));
    bgRectTexture->GenMipmaps();
    font = new raylib::Font(Util::formResourcePath(MARVEL_FONT_PATH), BASE_FONT_SIZE, nullptr, 95);
    GenTextureMipmaps(&font->texture);

    /* Initialize and alloc char array and set to empty string */
    content = new char[maxChars + 1];
    content[0] = '\0';
    numChars = 0;

    this->maxChars = maxChars;
    enabled = true;
    fontSize = BASE_FONT_SIZE;
    fontSpacing = 0;
    recalculateContentSize();
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
    fontSize = height * FONT_SIZE_SCALE;
    fontSpacing = height * FONT_SPACING_PCT;
    recalculateContentSize();
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

std::string MenuTextBox::getContent()
{
    return std::string(content);
}

void MenuTextBox::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void MenuTextBox::onKeyPressed(int key)
{
    if (!enabled) {
        return;
    }

    /* Handle presses of 0-9, A-Z, and Backspace */
    if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z')) {
        if (numChars < maxChars) {
            content[numChars++] = (char) key;
            content[numChars] = '\0';
        }
    } else if (key == KEY_BACKSPACE && numChars > 0) {
        content[--numChars] = '\0';
    }

    recalculateContentSize();
}

void MenuTextBox::render(Renderer *renderer)
{
    Color renderColor;

    if (enabled) {
        renderColor = Color {255, 255, 255, (unsigned char)(getDependentOpacity()*255)};
    } else {
        renderColor = Color{200, 200, 200, (unsigned char)(getDependentOpacity()*255)};
    }

    /* Draw left edge of bg rect */
    renderer->drawTexture(bgRectTexture, 0, 0, bgRectTexture->height / 2, bgRectTexture->height, 
            dstRect.x, dstRect.y, dstRect.height / 2, dstRect.height, renderColor);

    /* Draw middle segment of bg rect, stretched to fill space*/
    renderer->drawTexture(bgRectTexture, bgRectTexture->height / 2, 0,
            bgRectTexture->width - bgRectTexture->height, bgRectTexture->height,
            dstRect.x + (dstRect.height / 2), dstRect.y, dstRect.width - dstRect.height,
            dstRect.height, renderColor);

    /* Draw right edge of bg rect */
    renderer->drawTexture(bgRectTexture, bgRectTexture->width - (bgRectTexture->height / 2), 0,
            bgRectTexture->height / 2, bgRectTexture->height,
            dstRect.x + dstRect.width - (dstRect.height / 2), dstRect.y, dstRect.height / 2,
            dstRect.height, renderColor);

    renderer->setColor(renderColor);
    renderer->drawText(*font, content, dstRect.x + ((dstRect.width - contentSize.x) / 2),
            dstRect.y + ((dstRect.height - contentSize.y) / 2), fontSize, fontSpacing,
            getDependentOpacity());
}

void MenuTextBox::recalculateContentSize()
{
    if (numChars == 0) {
        contentSize.x = 0;
        contentSize.y = 0;
    } else {
        contentSize = font->MeasureText(content, fontSize, fontSpacing);
    }
}