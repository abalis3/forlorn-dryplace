#include "BoxSelector.h"

#include "Util.h"

static const char BG_RECT_IMG_PATH[] = "BoxSelector/background-rect.png";

/*
 * Parameters defining dimensions of certain items in BoxSelectors.
 * These values are all multiplicative factors relative to the bounding box height.
 */
static const float PAD_BETWEEN_ARROWS_AND_BOX = 0.5; // center of arrow to edge of box
static const float ARROW_BUTTON_MAX_DIMENSION = 0.8; // width and height are the same
static const float ARROW_BUTTON_MIN_DIMENSION = 0.5; // width and height are the same


BoxSelector::BoxSelector()
{
    bgRectTexture = new raylib::Texture(Util::formResourcePath(BG_RECT_IMG_PATH));
}

BoxSelector::~BoxSelector()
{
    delete bgRectTexture;
}

void BoxSelector::setHeight(float height)
{
    bgRectDst.height = height;
    bgRectDst.width = (height / bgRectTexture->GetHeight()) * bgRectTexture->GetWidth();

    arrowMaxDimension = ARROW_BUTTON_MAX_DIMENSION * height;
    arrowMinDimension = ARROW_BUTTON_MIN_DIMENSION * height;
    arrowToBoxPadding = PAD_BETWEEN_ARROWS_AND_BOX * height;

    boundingBox.height = height;
    boundingBox.width = arrowMaxDimension + 2*arrowToBoxPadding + bgRectDst.width;
}

float BoxSelector::getWidth()
{
    return boundingBox.width;
}

void BoxSelector::setXPos(float xPos)
{
    boundingBox.x = xPos;
    bgRectDst.x = xPos + (arrowMaxDimension / 2) + arrowToBoxPadding;
}

void BoxSelector::setYPos(float yPos)
{
    boundingBox.y = yPos;
    bgRectDst.y = yPos;
}

void BoxSelector::render(Renderer *renderer)
{
    boundingBox.Draw(GREEN);
    bgRectDst.Draw(RED);
}