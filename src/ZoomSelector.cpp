#include "ZoomSelector.h"

#include "Util.h"

ZoomSelector::ZoomSelector(const std::string &texturePath, float hoverZoomRatio)
{
    contentTexture = new raylib::Texture(Util::formResourcePath(texturePath));
    contentTexture->GenMipmaps();
    itemListHead = itemListTail = nullptr;
    this->hoverZoomRatio = hoverZoomRatio;
    this->itemPadding = 0;
    this->maxHeight = 0;
    this->centerXPos = 0;
    this->centerYPos = 0;
    numItems = 0;
}

ZoomSelector::~ZoomSelector()
{
    delete contentTexture;

    /* Clear out list of items */
    while(itemListHead != nullptr) {
        itemListTail = itemListHead;
        itemListHead = itemListHead->next;
        delete itemListTail;
    }
}

void ZoomSelector::updatePosition(float maxHeight, float centerXPos,
        float centerYPos, float itemPadding)
{
    this->maxHeight = maxHeight;
    this->centerXPos = centerXPos;
    this->centerYPos = centerYPos;
    this->itemPadding = itemPadding;
    recalculateSizeParams();
}

void ZoomSelector::addItem(int srcX, int srcY, int srcW, int srcH)
{
    ZoomSelectorItem *newItem = new ZoomSelectorItem();
    
    newItem->srcX = srcX;
    newItem->srcY = srcY;
    newItem->srcW = srcW;
    newItem->srcH = srcH;

    newItem->next = nullptr;
    if (itemListTail == nullptr) {
        itemListHead = newItem;
    } else {
        itemListTail->next = newItem;
    }
    itemListTail = newItem;

    numItems++;
    recalculateSizeParams();
}

void ZoomSelector::recalculateSizeParams()
{
    if (numItems == 0) return;

    float halfZoomRatio = 1 + ((hoverZoomRatio - 1) / 2);

    /* Measure the max height in unit of min item heights */
    float heightInMinItemHeights = hoverZoomRatio; // Hovered item
    if (numItems <= 3) {
        /* Adjacent to hovered items */
        heightInMinItemHeights += halfZoomRatio * (numItems - 1);
    } else {
        heightInMinItemHeights += halfZoomRatio * 2; // Adjacent to hovered
        heightInMinItemHeights += numItems - 3; // Not zoomed from hover
    }

    /* Get available max height to occupy, minus space for padding */
    float heightWithoutPadding = maxHeight;
    if (numItems > 1) {
        heightWithoutPadding -= itemPadding * (numItems - 1);
    }

    /* Now extract min item and max item heights */
    itemMinHeight = heightWithoutPadding / heightInMinItemHeights;
    itemMaxHeight = itemMinHeight * hoverZoomRatio;

    /* Set up constant size-dependent values for each item in the item list */
    ZoomSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        curItem->maxW = (itemMaxHeight / curItem->srcH) * curItem->srcW;
        curItem->minW = (itemMinHeight / curItem->srcH) * curItem->srcW;
        curItem->minX = centerXPos - (curItem->maxW / 2);
        curItem->maxX = centerXPos - (curItem->minW / 2);
        curItem = curItem->next;
    }
}

void ZoomSelector::update(double secs)
{
    float currentHeight = numItems * itemMinHeight;
    if (numItems > 1) {
        currentHeight += (numItems - 1) * itemPadding;
    }
    float curYPos = centerYPos - (currentHeight / 2);

    ZoomSelectorItem *curItem = itemListHead;

    while (curItem != nullptr) {
        curItem->dstY = curYPos;
        curItem->dstX = curItem->maxX;
        curItem->dstW = curItem->minW;
        curItem->dstH = itemMinHeight;
        curYPos += itemMinHeight;
        curYPos += itemPadding;
        curItem = curItem->next;
    }
}

void ZoomSelector::render(Renderer *renderer)
{
    ZoomSelectorItem *curItem = itemListHead;

    while (curItem != nullptr) {
        renderer->drawTexture(contentTexture, curItem->srcX, curItem->srcY,
                curItem->srcW, curItem->srcH, curItem->dstX, curItem->dstY,
                curItem->dstW, curItem->dstH);
        
        /*renderer->drawTexture(contentTexture, curItem->srcX, curItem->srcY,
                curItem->srcW, curItem->srcH, curItem->dstX, curItem->dstY, 200, 50);
        */curItem = curItem->next;
    }
}