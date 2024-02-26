#include "ListSelector.h"

/* Let's initialize our dimensions to non-zero to protect against uninitialized val errors */
static const float INIT_DIMENSION = 1;

/* Implementation of ListSelectorItem (abstract class) */

ListSelectorItem::ListSelectorItem() {
    height = INIT_DIMENSION;
    width = INIT_DIMENSION;
}

void ListSelectorItem::setHeight(float height) {
    this->height = height;
}

void ListSelectorItem::setWidth(float width) {
    this->width = width;
}

float ListSelectorItem::getHeight() {
    return this->height;
}

float ListSelectorItem::getWidth() {
    return this->width;
}

/* Implementation of ListSelector */

ListSelector::ListSelector() {
    itemHeight = LISTSELECTOR_INDIV_HEIGHTS;
    width = INIT_DIMENSION;
    height = INIT_DIMENSION;
    x = 0;
    y = 0;
    scrollAmount = 0;
}

ListSelector::~ListSelector() {
    for (int i = 0; i < items.size(); i++) {
        delete items[i];
    }
}

void ListSelector::appendItem(ListSelectorItem *item) {
    items.push_back(item);
    item->setWidth(width);
    if (itemHeight != LISTSELECTOR_INDIV_HEIGHTS) {
        item->setHeight(itemHeight);
    }
}

void ListSelector::setItemHeight(float height) {
    itemHeight = height;
    if (itemHeight != LISTSELECTOR_INDIV_HEIGHTS) {
        for (int i = 0; i < items.size(); i++) {
            items[i]->setHeight(itemHeight);
        }
    }
}

void ListSelector::setWidth(float width) {
    this->width = width;
    for (int i = 0; i < items.size(); i++) {
        items[i]->setWidth(width);
    }
}

void ListSelector::setHeight(float height) {
    this->height = height;
}

void ListSelector::update(double secs) {
    /* TODO */
}

void ListSelector::render(Renderer *renderer) {
    BeginScissorMode(x, y, width, height);
    float curY = 0;
    int i;

    /* Skip items that are not visible because we have scrolled past them */
    for (i = 0; i < items.size(); i++) {
        curY += items[i]->getHeight();
        if (curY > scrollAmount) {
            /* This item is in the visible pane. Start rendering */
            curY -= items[i]->getHeight();
            break;
        }
    }

    /* Render items as long as we are still in the pane */
    for (; i < items.size(); i++) {
        curY += items[i]->getHeight();
        items[i]->render(renderer);
        if (curY > scrollAmount + height) {
            /* That item ended past the bottom of our pane. Done rendering. */
            break;
        }
    }
    EndScissorMode();
}

/* Implementation of ListSelectorTextItem */

ListSelectorTextItem::ListSelectorTextItem(float fontHeightPct, std::string &&text) {
    this->fontHeightPct = fontHeightPct;
    this->text = text;

    fontSize = fontHeightPct * getHeight();
}

void ListSelectorTextItem::setHeight(float height) {
    ListSelectorItem::setHeight(height);
    fontSize = fontHeightPct * height;
}