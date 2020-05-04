#include "BoxSelector.h"

#include "Util.h"

static const char BG_RECT_IMG_PATH[] = "BoxSelector/background-rect.png";
static const char ARROW_IMG_PATH[] = "BoxSelector/arrow.png";

/*
 * Parameters defining dimensions of certain items in BoxSelectors.
 * These values are all multiplicative factors relative to the bounding box height
 * excelt the arrow zoom speed animation.
 */
static const float PAD_BETWEEN_ARROWS_AND_BOX = 1.0; // center of arrow to edge of box
static const float ARROW_BUTTON_MAX_DIMENSION = 0.65; // width and height are the same
static const float ARROW_BUTTON_MIN_DIMENSION = 0.5; // width and height are the same
static const float ARROW_ZOOM_ANIM_SPEED = 3.0; // higher = faster

BoxSelector::BoxSelector()
{
    bgRectTexture = new raylib::Texture(Util::formResourcePath(BG_RECT_IMG_PATH));
    bgRectTexture->GenMipmaps();
    arrowTexture = new raylib::Texture(Util::formResourcePath(ARROW_IMG_PATH));
    arrowTexture->GenMipmaps();
}

BoxSelector::~BoxSelector()
{
    delete bgRectTexture;
    delete arrowTexture;
}

void BoxSelector::setHeight(float height)
{
    bgRectDst.height = height;
    bgRectDst.width = (height / bgRectTexture->GetHeight()) * bgRectTexture->GetWidth();

    arrowMaxDimension = ARROW_BUTTON_MAX_DIMENSION * height;
    arrowMinDimension = ARROW_BUTTON_MIN_DIMENSION * height;
    arrowToBoxPadding = PAD_BETWEEN_ARROWS_AND_BOX * height;
    leftArrowCurDimension = leftArrowTargetDimension = arrowMinDimension;
    rightArrowCurDimension = rightArrowTargetDimension = arrowMinDimension;

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

void BoxSelector::onMousePosUpdate(const raylib::Vector2 &pos)
{
    raylib::Rectangle collisionLeft(boundingBox.x,
            boundingBox.y + (boundingBox.height - arrowMaxDimension) / 2,
            arrowMaxDimension, arrowMaxDimension);
    raylib::Rectangle collisionRight(boundingBox.x + boundingBox.width - arrowMaxDimension,
            boundingBox.y + (boundingBox.height - arrowMaxDimension) / 2,
            arrowMaxDimension, arrowMaxDimension);

    if (collisionLeft.CheckCollision(pos)) {
        leftArrowTargetDimension = arrowMaxDimension;
    } else {
        leftArrowTargetDimension = arrowMinDimension;
    }

    if (collisionRight.CheckCollision(pos)) {
        rightArrowTargetDimension = arrowMaxDimension;
    } else {
        rightArrowTargetDimension = arrowMinDimension;
    }
}

void BoxSelector::update(double secs)
{
    /* Animate the left arrow zoom */
    if (leftArrowCurDimension < leftArrowTargetDimension) {
        leftArrowCurDimension += secs * ARROW_ZOOM_ANIM_SPEED * arrowMaxDimension;
        if (leftArrowCurDimension > leftArrowTargetDimension) {
            leftArrowCurDimension = leftArrowTargetDimension;
        }
    } else if (leftArrowCurDimension > leftArrowTargetDimension) {
        leftArrowCurDimension -= secs * ARROW_ZOOM_ANIM_SPEED * arrowMaxDimension;
        if (leftArrowCurDimension < leftArrowTargetDimension) {
            leftArrowCurDimension = leftArrowTargetDimension;
        }
    }

    /* Animate the right arrow zoom */
    if (rightArrowCurDimension < rightArrowTargetDimension) {
        rightArrowCurDimension += secs * ARROW_ZOOM_ANIM_SPEED * arrowMaxDimension;
        if (rightArrowCurDimension > rightArrowTargetDimension) {
            rightArrowCurDimension = rightArrowTargetDimension;
        }
    } else if (rightArrowCurDimension > rightArrowTargetDimension) {
        rightArrowCurDimension -= secs * ARROW_ZOOM_ANIM_SPEED * arrowMaxDimension;
        if (rightArrowCurDimension < rightArrowTargetDimension) {
            rightArrowCurDimension = rightArrowTargetDimension;
        }
    }
}

void BoxSelector::render(Renderer *renderer)
{
    /* Draw the background rectangle */
    renderer->drawTexture(bgRectTexture, bgRectDst, getDependentOpacity());
    
    /* Draw the arrow buttons */
    renderer->drawTexture(arrowTexture,
        boundingBox.x + (arrowMaxDimension - leftArrowCurDimension) / 2,
        boundingBox.y + (boundingBox.height - leftArrowCurDimension) / 2,
        leftArrowCurDimension, leftArrowCurDimension, getDependentOpacity(), true);
    renderer->drawTexture(arrowTexture,
            boundingBox.x + boundingBox.width - (arrowMaxDimension + rightArrowCurDimension) / 2,
            boundingBox.y + (boundingBox.height - rightArrowCurDimension) / 2,
            rightArrowCurDimension, rightArrowCurDimension, getDependentOpacity(), false);
}