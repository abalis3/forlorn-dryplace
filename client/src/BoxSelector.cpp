#include "BoxSelector.h"

#include "Util.h"

static const char BG_RECT_IMG_PATH[] = "BoxSelector/background-rect.png";
static const char ARROW_IMG_PATH[] = "BoxSelector/arrow.png";
static const char MARVEL_FONT_PATH[] = "Fonts/Marvel-Regular.ttf";

static const int BASE_FONT_SIZE = 175;

/*
 * Parameters defining dimensions of certain items in BoxSelectors.
 * These values are all multiplicative factors relative to the bounding box height
 * excelt the arrow zoom speed animation.
 */
static const float PAD_BETWEEN_ARROWS_AND_BOX = 1.0; // center of arrow to edge of box
static const float ARROW_BUTTON_MAX_DIMENSION = 0.65; // width and height are the same
static const float ARROW_BUTTON_MIN_DIMENSION = 0.5; // width and height are the same
static const float ARROW_ZOOM_ANIM_SPEED = 3.0; // higher = faster
static const float FONT_HEIGHT_PCT = 0.8;
static const float FONT_SPACING_PCT = 0.05;

BoxSelector::BoxSelector()
{
    /* Load necessary resources */
    bgRectTexture = new raylib::Texture(Util::formResourcePath(BG_RECT_IMG_PATH));
    bgRectTexture->GenMipmaps();
    arrowTexture = new raylib::Texture(Util::formResourcePath(ARROW_IMG_PATH));
    arrowTexture->GenMipmaps();
    font = new raylib::Font(Util::formResourcePath(MARVEL_FONT_PATH), BASE_FONT_SIZE, nullptr, 95);
    GenTextureMipmaps(&font->texture);

    /* Initialize variables as needed */
    itemListHead = itemListTail = nullptr;
    selectedItem = nullptr;
    nextItemIndex = 0;
    fontSize = BASE_FONT_SIZE;
    fontSpacing = 0;
}

BoxSelector::~BoxSelector()
{
    delete bgRectTexture;
    delete arrowTexture;
    delete font;

    /* Clear out list of items */
    while(itemListHead != nullptr) {
        itemListTail = itemListHead;
        itemListHead = itemListHead->next;
        delete itemListTail;
    }
}

void BoxSelector::setHeight(float height)
{
    bgRectDst.height = height;
    bgRectDst.width = (height / bgRectTexture->GetHeight()) * bgRectTexture->GetWidth();

    /* Update properties used for arrows */
    arrowMaxDimension = ARROW_BUTTON_MAX_DIMENSION * height;
    arrowMinDimension = ARROW_BUTTON_MIN_DIMENSION * height;
    arrowToBoxPadding = PAD_BETWEEN_ARROWS_AND_BOX * height;
    leftArrowCurDimension = leftArrowTargetDimension = arrowMinDimension;
    rightArrowCurDimension = rightArrowTargetDimension = arrowMinDimension;

    /* Update font size and text dimensions for all items in list */
    fontSize = FONT_HEIGHT_PCT * height;
    fontSpacing = FONT_SPACING_PCT * height;
    BoxSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        curItem->renderSize = font->MeasureText(curItem->text, fontSize, fontSpacing);
        curItem = curItem->next;
    }

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

void BoxSelector::addItem(const std::string &content, bool selected)
{
    BoxSelectorItem *newItem = new BoxSelectorItem();
    
    newItem->text = content;
    newItem->index = nextItemIndex++;
    newItem->renderSize = font->MeasureText(content, fontSize, fontSpacing);

    newItem->next = nullptr;
    if (itemListTail == nullptr) {
        itemListHead = newItem;
        selectedItem = newItem;
    } else {
        itemListTail->next = newItem;
        if (selected) selectedItem = newItem;
    }
    itemListTail = newItem;
}

int BoxSelector::getSelectedIndex()
{
    if (selectedItem == nullptr) return -1;
    return selectedItem->index;
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

void BoxSelector::onMousePressed(const raylib::Vector2 &pos)
{
    raylib::Rectangle collisionLeft(boundingBox.x,
            boundingBox.y + (boundingBox.height - arrowMaxDimension) / 2,
            arrowMaxDimension, arrowMaxDimension);
    raylib::Rectangle collisionRight(boundingBox.x + boundingBox.width - arrowMaxDimension,
            boundingBox.y + (boundingBox.height - arrowMaxDimension) / 2,
            arrowMaxDimension, arrowMaxDimension);

    if (collisionLeft.CheckCollision(pos)) {
        /* IMPROVEMENT: make this a doubly linked list so we can step back one */
        if (selectedItem == nullptr) return;
        int targetIndex = selectedItem->index - 1;
        if (targetIndex == -1) {
            selectedItem = itemListTail;
            return;
        }
        selectedItem = itemListHead;
        while (selectedItem->index != targetIndex) {
            selectedItem = selectedItem->next;
        }
    } else if (collisionRight.CheckCollision(pos)) {
        selectedItem = selectedItem->next;
        if (selectedItem == nullptr) selectedItem = itemListHead;
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
    
    /* Draw the text for the selected item in the rectangle */
    if (selectedItem != nullptr) {
        int posX = bgRectDst.x + (bgRectDst.width - selectedItem->renderSize.x) / 2;
        int posY = bgRectDst.y + (bgRectDst.height - selectedItem->renderSize.y) / 2;
        renderer->setColor(WHITE);
        renderer->drawText(*font, selectedItem->text, posX, posY, fontSize, fontSpacing,
                getDependentOpacity());
    }

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