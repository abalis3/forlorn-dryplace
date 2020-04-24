#include "ZoomSelector.h"

#include "Util.h"

#define CARAT_IMG_PATH "ZoomScroll/carat.png"

#define ANIMATION_DURATION 0.1
#define CARAT_HEIGHT_FRACTION 0.15
#define CARAT_DISTANCE_FRACTION 0.8

ZoomSelector::ZoomSelector(const std::string &texturePath, float hoverZoomRatio)
{
    contentTexture = new raylib::Texture(Util::formResourcePath(texturePath));
    contentTexture->GenMipmaps();
    caratTexture = new raylib::Texture(Util::formResourcePath(CARAT_IMG_PATH));
    caratTexture->GenMipmaps();
    itemListHead = itemListTail = nullptr;
    this->hoverZoomRatio = hoverZoomRatio;
    this->minItemPadding = 0;
    this->maxItemPadding = 0;
    this->maxHeight = 0;
    this->centerXPos = 0;
    this->centerYPos = 0;
    this->focusedIndex = -1;
    this->animStopwatch = ANIMATION_DURATION;
    this->callback = nullptr;
    numItems = 0;
}

ZoomSelector::~ZoomSelector()
{
    delete contentTexture;
    delete caratTexture;

    /* Clear out list of items */
    while(itemListHead != nullptr) {
        itemListTail = itemListHead;
        itemListHead = itemListHead->next;
        delete itemListTail;
    }
}

void ZoomSelector::updatePosition(float maxHeight, float centerXPos,
        float centerYPos, float minItemPadding)
{
    this->maxHeight = maxHeight;
    this->centerXPos = centerXPos;
    this->centerYPos = centerYPos;
    this->minItemPadding = minItemPadding;
    this->expansionCenter = centerYPos;
    recalculateSizeParams();
}

void ZoomSelector::addItem(const raylib::Rectangle &srcRect, float itemCenterY)
{
    ZoomSelectorItem *newItem = new ZoomSelectorItem();
    
    newItem->srcRect = srcRect;
    newItem->itemCenterY = itemCenterY;
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

void ZoomSelector::setCallback(std::function<void(ZoomSelector*, int)> cb)
{
    callback = cb;
}

void ZoomSelector::recalculateSizeParams()
{
    if (numItems == 0) return;

    maxItemPadding = hoverZoomRatio * minItemPadding;

    float halfZoomRatio = 1 + ((hoverZoomRatio - 1) / 2);

    /* Measure the max height in unit of min item heights */
    float heightInMinItemHeights = hoverZoomRatio; // Hovered item
    float maxPaddingHeight = maxItemPadding;
    if (numItems <= 3) {
        /* Adjacent to hovered items */
        heightInMinItemHeights += halfZoomRatio * (numItems - 1);
        maxPaddingHeight += halfZoomRatio * (numItems - 1) * minItemPadding;
    } else {
        heightInMinItemHeights += halfZoomRatio * 2; // Adjacent to hovered
        maxPaddingHeight += halfZoomRatio * 2 * minItemPadding;
        heightInMinItemHeights += numItems - 3; // Not zoomed from hover
        maxPaddingHeight += minItemPadding * (numItems - 3);
    }

    /* Get available max height to occupy, minus space for padding */
    float heightWithoutPadding = maxHeight - maxPaddingHeight;

    /* Now extract min item and max item heights */
    itemMinHeight = heightWithoutPadding / heightInMinItemHeights;
    itemMaxHeight = itemMinHeight * hoverZoomRatio;

    /* Set up constant size-dependent values for each item in the item list */
    maxWidth = 0;
    float totalMinHeight = numItems * (itemMinHeight + minItemPadding);
    float curYPos = centerYPos - (totalMinHeight / 2);
    ZoomSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        
        curItem->maxW = (itemMaxHeight / curItem->srcRect.height) * curItem->srcRect.width;
        curItem->minW = (itemMinHeight / curItem->srcRect.height) * curItem->srcRect.width;
        curItem->minX = centerXPos - (curItem->maxW / 2);
        curItem->maxX = centerXPos - (curItem->minW / 2);

        curItem->collisionY = curYPos;
        curItem->collisionH = itemMinHeight + minItemPadding;
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
    /* Should we update animations? */
    bool animate = false;
    /* What pct to move from val to targetVal? 0 = none, 1 = all the way */
    double animationFraction = 0;
    if (animStopwatch < ANIMATION_DURATION) {
        animate = true;
        double lastTime = animStopwatch;
        animStopwatch += secs;
        if (animStopwatch >= ANIMATION_DURATION) {
            animationFraction = 1;
        } else {
            animationFraction = secs / (ANIMATION_DURATION - lastTime);
        }
    }
    
    /* Animate the expansionCenter */
    if (animate) {
        expansionCenter += animationFraction * (targetExpansionCenter - expansionCenter);
    }

    /* Loop through list to calculate zoom and size of each item */
    ZoomSelectorItem *curItem = itemListHead;
    while (curItem != nullptr) {
        
        /* Update animation for zoom percent for this item */
        if (animate) {
            curItem->zoomPercent += animationFraction *
                    (curItem->targetZoomPercent - curItem->zoomPercent);
        }
        
        /* Update destination/size parameters based on zoom. Must do dst Y later */
        curItem->dstRect.x = curItem->maxX -
                (curItem->maxX - curItem->minX) * curItem->zoomPercent;
        curItem->dstRect.width = curItem->minW +
                (curItem->maxW - curItem->minW) * curItem->zoomPercent;
        curItem->dstRect.height = itemMinHeight +
                (itemMaxHeight - itemMinHeight) * curItem->zoomPercent;
        curItem->padding = minItemPadding +
                (maxItemPadding - minItemPadding) * curItem->zoomPercent;

        curItem = curItem->next;
    }

    /* Populate dst Y values now that we have item heights and expansion center */
    populateItemListDstYValues(itemListHead, false, 0);
}

float ZoomSelector::populateItemListDstYValues(ZoomSelectorItem *listHead,
        bool prevNodeSolved, float yPos)
{
    /* Recursion base case. This return shouldn't ever get used */
    if (listHead == nullptr) return -1;

    if (prevNodeSolved) {
        /* From here on out it's easy, everything is solved. */
        /* We got the bottom of the previous node's padding */
        listHead->dstRect.y = yPos + listHead->padding / 2;
        return populateItemListDstYValues(listHead->next, true,
                yPos + listHead->dstRect.height + listHead->padding);
    } else {
        /* Is the expansionCenter in this node (including its padding)? */
        if (expansionCenter >= listHead->collisionY &&
                (expansionCenter <= listHead->collisionY + listHead->collisionH)) {

            float totalItemHeight = listHead->dstRect.height + listHead->padding;
            float itemBeforeExpCtr = (expansionCenter - listHead->collisionY) *
                    (totalItemHeight / (listHead->collisionH));
            float topOfItem = expansionCenter - itemBeforeExpCtr;
            listHead->dstRect.y = topOfItem + listHead->padding / 2;
            float bottomOfItem = topOfItem + listHead->dstRect.height + listHead->padding;


            /* Call forward and return our top y coord (before our padding starts) */
            populateItemListDstYValues(listHead->next, true, bottomOfItem);
            return topOfItem;
        
        } else {
            /* We don't have the expansionCenter and it's in a later node */
            float nextY = populateItemListDstYValues(listHead->next, false, 0);
            /* BUG? Do we need to make sure we found the expansionCenter by checking for -1? */
            nextY -= listHead->dstRect.height + listHead->padding;
            listHead->dstRect.y = nextY + listHead->padding / 2;
            return nextY;
        }
    }
}

void ZoomSelector::onMousePosUpdate(const raylib::Vector2 &pos)
{
    raylib::Rectangle hitbox;
    hitbox.width = maxWidth + maxItemPadding;
    hitbox.x = centerXPos - (hitbox.width / 2);

    /* Did the item being hovered change? If so we will start an animation */
    int lastFocusedIndex = focusedIndex;
    int curIndex = 0;
    focusedIndex = -1;

    targetExpansionCenter = centerYPos;
    bool shouldHalfZoomNext = false;
    ZoomSelectorItem *curItem = itemListHead;
    ZoomSelectorItem *prevItem = nullptr;
    while (curItem != nullptr) {

        /* Calibrate the hitbox to the current item */
        hitbox.y = curItem->collisionY;
        hitbox.height = curItem->collisionH;
        
        if (hitbox.CheckCollision(pos)) {
            /* For a found collision, zoom current and last item and set the expansionCenter */
            focusedIndex = curIndex;
            curItem->targetZoomPercent = 1.0;
            targetExpansionCenter = curItem->collisionY +
                (curItem->collisionH / 2);
            if (prevItem != nullptr) {
                prevItem->targetZoomPercent = 0.5;
            }
            shouldHalfZoomNext = true;
        } else if (shouldHalfZoomNext) {
            /* The last item was focused, half-zoom this one */
            curItem->targetZoomPercent = 0.5;
            shouldHalfZoomNext = false;
        } else {
            curItem->targetZoomPercent = 0;
        }

        curIndex++;
        prevItem = curItem;
        curItem = curItem->next;
    }

    /* Focus/hover changed - start animating to the new targets */
    if (lastFocusedIndex != focusedIndex) {
        animStopwatch = 0;
    }

}

void ZoomSelector::onMousePressed()
{
    if (callback != nullptr && focusedIndex >= 0) {
        callback(this, focusedIndex);
    }
}

void ZoomSelector::render(Renderer *renderer)
{
    int curIndex = 0;
    ZoomSelectorItem *curItem = itemListHead;

    while (curItem != nullptr) {

        renderer->drawTexture(contentTexture, curItem->srcRect,
                curItem->dstRect);

        if(curIndex++ == focusedIndex) {

            /* This is the selected item. calculate where to draw carats */    
            float caratDist = CARAT_DISTANCE_FRACTION * itemMaxHeight;
            float caratHeight = CARAT_HEIGHT_FRACTION * itemMaxHeight;
            float caratStartY = curItem->dstRect.y  + 
                    (curItem->itemCenterY / curItem->srcRect.height) * curItem->dstRect.height -
                    caratHeight / 2;
            float caratWidth = caratTexture->width * (caratHeight / caratTexture->height);
            float caratX1 = curItem->dstRect.x - caratWidth - caratDist;
            float caratX2 = curItem->dstRect.x + curItem->dstRect.width + caratDist;

            /* Render the carats - note the first one with negative width to flip the image */
            renderer->drawTexture(caratTexture, caratX1, caratStartY, caratWidth, caratHeight,
                    true);
            renderer->drawTexture(caratTexture, caratX2, caratStartY, caratWidth, caratHeight,
                    false);
        }

        curItem = curItem->next;
    }
}