#include "ZoomSelector.h"

#include "Util.h"

#include <iostream>

#define ZOOM_ANIM_SPEED 5.0
#define EXP_CTR_ANIM_SPEED 10

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
    this->expansionCenter = centerYPos;
    recalculateSizeParams();
}

void ZoomSelector::addItem(const raylib::Rectangle &srcRect)
{
    ZoomSelectorItem *newItem = new ZoomSelectorItem();
    
    newItem->srcRect = srcRect;
    newItem->targetZoomPercent = 0;
    newItem->zoomPercent = 0;

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
    maxWidth = 0;
    float totalMinHeight = numItems * (itemMinHeight + itemPadding);
    float curYPos = centerYPos - (totalMinHeight / 2) - (itemPadding / 2);
    ZoomSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        
        curItem->maxW = (itemMaxHeight / curItem->srcRect.height) * curItem->srcRect.width;
        curItem->minW = (itemMinHeight / curItem->srcRect.height) * curItem->srcRect.width;
        curItem->minX = centerXPos - (curItem->maxW / 2);
        curItem->maxX = centerXPos - (curItem->minW / 2);

        curItem->collisionY = curYPos;
        curItem->collisionH = itemMinHeight + itemPadding;
        curYPos += curItem->collisionH;

        /* Find overall width of the zoom selector */
        if (curItem->maxW > maxWidth) {
            maxWidth = curItem->maxW;
        }

        curItem = curItem->next;
    }
}

void ZoomSelector::update(double secs)
{
    /* TODO: make this vary with size */
    /*if (expansionCenter > targetExpansionCenter) {
        expansionCenter -= EXP_CTR_ANIM_SPEED * secs;
        if (expansionCenter < targetExpansionCenter) expansionCenter = targetExpansionCenter;
    } else if (expansionCenter < targetExpansionCenter) {
        expansionCenter += EXP_CTR_ANIM_SPEED * secs;
        if (expansionCenter > targetExpansionCenter) expansionCenter = targetExpansionCenter;
    }*/
    expansionCenter = targetExpansionCenter;
    std::cout << expansionCenter << std::endl;

    ZoomSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        
        /* Update zoom percent */
        if (curItem->zoomPercent > curItem->targetZoomPercent) {
            curItem->zoomPercent -= ZOOM_ANIM_SPEED * secs;
            if (curItem->zoomPercent < curItem->targetZoomPercent)
                    curItem->zoomPercent = curItem->targetZoomPercent;
        } else if (curItem->zoomPercent < curItem->targetZoomPercent) {
            curItem->zoomPercent += ZOOM_ANIM_SPEED * secs;
            if (curItem->zoomPercent > curItem->targetZoomPercent)
                    curItem->zoomPercent = curItem->targetZoomPercent;
        }
        
        /* Update destination parameters based on zoom. Must do dst Y later */
        curItem->dstRect.x = curItem->maxX;// -
                //(curItem->maxX - curItem->minX) * curItem->zoomPercent;
        curItem->dstRect.width = curItem->minW;// +
                //(curItem->maxW - curItem->minW) * curItem->zoomPercent;
        curItem->dstRect.height = itemMinHeight;// +
                //(itemMaxHeight - itemMinHeight) * curItem->zoomPercent;

        curItem = curItem->next;
    }

    itemListHead->next->dstRect.height = itemMaxHeight;
    itemListHead->next->dstRect.x = itemListHead->next->minX;
    itemListHead->next->dstRect.width = itemListHead->next->maxW;

    /* Populate dst Y values now that we have item heights and expansion center */
    populateItemListDstYValues(itemListHead, false, 0);
}

float ZoomSelector::populateItemListDstYValues(ZoomSelectorItem *listHead,
        bool prevNodeSolved, float yPos)
{
    /* Recursion base case. This return shouldn't ever get used */
    if (listHead == nullptr) return -1;

    if (prevNodeSolved) {
        /* From here on out it's easy, everything is solved */
        listHead->dstRect.y = yPos + itemPadding; // We got the BOTTOM of prev
        return populateItemListDstYValues(listHead->next, true,
                listHead->dstRect.y + listHead->dstRect.height);
    } else {
        /* Is the expansionCenter in this node? */
        if (expansionCenter >= listHead->collisionY &&
                (expansionCenter <= listHead->collisionY + listHead->collisionH)) {

            /* Got it! Let's work outward from here */
            float bottomOfItem;
            if (expansionCenter - listHead->collisionY < itemPadding / 2) {

                /* It's in the padding before this item */
                listHead->dstRect.y = listHead->collisionY + (itemPadding / 2);
                bottomOfItem = listHead->dstRect.y + listHead->dstRect.height;

            } else if (expansionCenter > listHead->collisionY + listHead->dstRect.height) {

                /* It's in the padding after this item */
                bottomOfItem = listHead->collisionY + listHead->collisionH - (itemPadding / 2);
                listHead->dstRect.y = bottomOfItem - listHead->dstRect.height;

            } else {

                /* Rats - it's in the middle of the scaled image */
                float imgStart = listHead->collisionY + (itemPadding / 2);
                float imgBeforeExpCtr = (expansionCenter - imgStart) *
                        (listHead->dstRect.height / itemMinHeight);
                listHead->dstRect.y = expansionCenter - imgBeforeExpCtr;
                bottomOfItem = listHead->dstRect.y + listHead->dstRect.height;

            }

            /* Call forward and return our value, now those around can propagate the info */
            populateItemListDstYValues(listHead->next, true, bottomOfItem);
            return listHead->dstRect.y;
        
        } else {
            /* We don't have the expansionCenter and it's in a later node */
            float nextY = populateItemListDstYValues(listHead->next, false, 0);
            /* BUG? Do we need to make sure we found the expansionCenter by checking for -1? */
            nextY -= listHead->dstRect.height + itemPadding;
            listHead->dstRect.y = nextY;
            return nextY;
        }
    }
}

void ZoomSelector::onMousePosUpdate(const raylib::Vector2 &pos)
{
    raylib::Rectangle hitbox;
    hitbox.x = centerXPos - (maxWidth / 2);
    hitbox.width = maxWidth;

    targetExpansionCenter = centerYPos;
    bool shouldHalfZoomNext = false;
    ZoomSelectorItem *curItem = itemListHead;
    ZoomSelectorItem *prevItem = nullptr;
    while (curItem != nullptr) {

        /* Calibrate the hitbox to the current item */
        hitbox.y = curItem->collisionY;
        hitbox.height = curItem->collisionH;
        
        /* For a found collision, zoom current item, prev, and next */
        if (hitbox.CheckCollision(pos)) {
            curItem->targetZoomPercent = 1.0;
            targetExpansionCenter = curItem->collisionY +
                (curItem->collisionH / 2);
            if (prevItem != nullptr) {
                prevItem->targetZoomPercent = 0.5;
            }
            shouldHalfZoomNext = true;
        } else if (shouldHalfZoomNext) {
            curItem->targetZoomPercent = 0.5;
            shouldHalfZoomNext = false;
        } else {
            curItem->targetZoomPercent = 0;
        }

        prevItem = curItem;
        curItem = curItem->next;
    }
}

void ZoomSelector::render(Renderer *renderer)
{
    ZoomSelectorItem *curItem = itemListHead;

    while (curItem != nullptr) {
        DrawLine(0, 600, 1920, 600, GREEN);
        renderer->drawTexture(contentTexture, curItem->srcRect,
                curItem->dstRect);
        curItem = curItem->next;
    }
}