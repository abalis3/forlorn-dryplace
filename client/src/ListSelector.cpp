#include "ListSelector.h"

#include "Util.h"

/* Let's initialize our dimensions to non-zero to protect against uninitialized val errors */
static const float INIT_DIMENSION = 1;

/* How long should it take to animate the scroll to "halfway" to the destination? */
static const float SCROLL_HALF_LIFE_SEC = 0.05;

/* Minimum speed cap (% of pane height / sec) to scroll as we approach the target scrollAmount */
static const float SCROLL_MIN_SPEED = (0.3 / 1); /* 50% height per 1 second */ 

/* What percent of the pane height should we scroll per mouse wheel tick? */
static const float SCROLL_SPEED_FACTOR = 0.08;

/* Filepath to the Marvel font used by ListSelectorTextItem */
static const char MARVEL_FONT_PATH[] = "Fonts/Marvel-Regular.ttf";

/* Filepath to the Textures used by ListSelectorTextItem */
static const char HOVER_BG_IMG_PATH[] = "ListSelector/background-hover.png";
static const char SELECTED_BG_IMG_PATH[] = "ListSelector/background-selected.png";

/* The base font size at which to load the font. Can be scaled for rendering */
static const int BASE_FONT_SIZE = 175;

static const float FONT_SPACING_PCT = 0.05; /* pct of font size to use as font spacing */

/* Implementation of ListSelectorItem (abstract class) */

ListSelectorItem::ListSelectorItem() {
    height = INIT_DIMENSION;
    width = INIT_DIMENSION;
    yPosInList = 0;
    hovered = false;
    selected = false;
}

void ListSelectorItem::setHeight(float height) {
    this->height = height;
    heightChanged();
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

void ListSelectorItem::setYPosInList(float y) {
    this->yPosInList = y;
}

float ListSelectorItem::getYPosInList() {
    return yPosInList;
}

void ListSelectorItem::setHover(bool hover) {
    hovered = hover;
}

bool ListSelectorItem::getHover() {
    return hovered;
}

void ListSelectorItem::setSelected(bool selected) {
    this->selected = selected;
}

bool ListSelectorItem::getSelected() {
    return selected;
}

void ListSelectorItem::setIdxInList(int idx) {
    idxInList = idx;
}

int ListSelectorItem::getIdxInList() {
    return idxInList;
}

/* Implementation of ListSelector */

ListSelector::ListSelector() {
    itemHeight = LISTSELECTOR_INDIV_HEIGHTS;
    width = INIT_DIMENSION;
    height = INIT_DIMENSION;
    separatorHeight = 0;
    totalListHeight = 0;
    x = 0;
    y = 0;
    targetScroll = 0;
    scrollAmount = 0;
    selectionCallback = nullptr;
    hoveredItem = nullptr;
    selectedItem = nullptr;
}

ListSelector::~ListSelector() {
    for (int i = 0; i < items.size(); i++) {
        delete items[i];
    }
}

void ListSelector::appendItem(ListSelectorItem *item) {
    float separator = items.empty() ? 0 : separatorHeight;

    item->setWidth(width);
    if (itemHeight != LISTSELECTOR_INDIV_HEIGHTS) {
        item->setHeight(itemHeight);
    }

    item->setHover(false);
    item->setSelected(false);;
    item->setYPosInList(totalListHeight + separator);
    item->setIdxInList(items.size());
    items.push_back(item);
    totalListHeight += separator + item->getHeight();
}

void ListSelector::setX(float x) {
    this->x = x;
}

void ListSelector::setY(float y) {
    this->y = y;
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

float ListSelector::getWidth() {
    return width;
}

float ListSelector::getHeight() {
    return height;
}

void ListSelector::setSeparatorHeight(float height, bool itemHeightNext) {
    separatorHeight = height;
    if (!itemHeightNext) {
        totalListHeight = 0;
        for (int i = 0; i < items.size(); i++) {
            float separator = (i == 0) ? 0 : separatorHeight;
            items[i]->setYPosInList(totalListHeight + separator);
            totalListHeight += separator + items[i]->getHeight();
        }
    }
}

void ListSelector::setItemHeight(float height) {
    itemHeight = height;
    if (itemHeight != LISTSELECTOR_INDIV_HEIGHTS) {
        totalListHeight = 0;
        for (int i = 0; i < items.size(); i++) {
            float separator = (i == 0) ? 0 : separatorHeight;
            items[i]->setYPosInList(totalListHeight + separator);
            items[i]->setHeight(itemHeight);
            totalListHeight += separator + items[i]->getHeight();
        }
    }
}

void ListSelector::setSelectionCallback(std::function<void(ListSelector*, int)> cb)
{
    selectionCallback = cb;
}

void ListSelector::onMouseWheelMoved(float amount, const raylib::Vector2 &pos) {
    raylib::Rectangle hitbox(x, y, width, height);
    
    /* If the mouse is within the bounds of the on-screen pane, scroll. */
    if (hitbox.CheckCollision(pos)) {
        targetScroll -= amount * SCROLL_SPEED_FACTOR * height;
    }
}

void ListSelector::onMousePosUpdate(const raylib::Vector2 &pos) {
    ListSelectorItem *mousedOverItem = getItemForMousePos(pos);
    
    if (hoveredItem != mousedOverItem) {
        if (hoveredItem != nullptr) {
            hoveredItem->setHover(false);
        }
        hoveredItem = mousedOverItem;
        if (hoveredItem != nullptr) {
            hoveredItem->setHover(true);
        }
    }
}

void ListSelector::onMousePressed() {
    if (hoveredItem == nullptr) {
        /* Mouse isn't over any of our items. Clicking is a no-op */
        return;
    }

    if (selectedItem != hoveredItem) {
        /* We are clicking an item not already selected! */
        if (selectedItem != nullptr) {
            selectedItem->setSelected(false);
        }
        selectedItem = hoveredItem;
        selectedItem->setSelected(true);
        if (selectionCallback != nullptr) {
            selectionCallback(this, getSelectedItemIdx());
        }
    }
}

int ListSelector::getSelectedItemIdx() {
    if (selectedItem == nullptr) {
        return -1;
    }
    return selectedItem->getIdxInList();
}

void ListSelector::deselectAll() {
    bool changing = (selectedItem != nullptr);

    if (changing) {
        selectedItem->setSelected(false);
    }
    selectedItem = nullptr;

    if (changing && selectionCallback != nullptr) {
        selectionCallback(this, -1);
    }
}

void ListSelector::scrollToTop(bool instant) {
    targetScroll = 0;
    if (instant) {
        scrollAmount = 0;
    }
}

void ListSelector::setDependentOpacity(float opacity) {
    OpacityDependent::setDependentOpacity(opacity);

    /* Update opacity of items in the list */
    for (int i = 0; i < items.size(); i++) {
        items[i]->setDependentOpacity(opacity);
    }
}

void ListSelector::update(double secs) {
    if (targetScroll == scrollAmount) {
        /* Fast path for common case where we aren't scrolling */
        return;
    }

    if (height > totalListHeight) {
        /* Fast path for if the list isn't long enough to scroll at all */
        targetScroll = scrollAmount = 0;
        return;
    }

    float scrollSpeed = (targetScroll - scrollAmount) / (2 * SCROLL_HALF_LIFE_SEC);
    float scrollSpeedCap = height * SCROLL_MIN_SPEED;

    if ((scrollSpeed > 0) && (scrollSpeed < scrollSpeedCap)) {
        scrollSpeed = scrollSpeedCap;
    } else if ((scrollSpeed < 0) && (scrollSpeed > -scrollSpeedCap)) {
        scrollSpeed = -scrollSpeedCap;
    }

    float scrollDelta = secs * scrollSpeed;

    if (abs(scrollDelta) > abs(targetScroll - scrollAmount)) {
        /* We would scroll past the target. Just snap to it */
        scrollAmount = targetScroll;
    } else {
        scrollAmount += scrollDelta;
    }

    if (scrollAmount < 0) {
        targetScroll = scrollAmount = 0;
    } else if ((scrollAmount + height) > totalListHeight) {
        targetScroll = scrollAmount = totalListHeight - height;
    }

}

void ListSelector::render(Renderer *renderer) {
    BeginScissorMode(x, y, width, height);
    int i;

    /* Skip items that are not visible because we have scrolled past them */
    for (i = 0; i < items.size(); i++) {
        if ((items[i]->getYPosInList() + items[i]->getHeight()) > scrollAmount) {
            /* This item is in the visible pane. Start rendering */
            break;
        }
    }

    /* Render items as long as we are still in the pane */
    for (; i < items.size(); i++) {
        if ((i > 0) && (separatorHeight > 0)) {
            /* Before all list items other than the first one, draw the separator */
            renderer->setColor(Color{255,255,255,(unsigned char)(255*getDependentOpacity())});
            renderer->drawRectangle(x,
                    (y + items[i]->getYPosInList() - scrollAmount) - separatorHeight,
                    width, separatorHeight);
        }
        items[i]->render(renderer, x, y + items[i]->getYPosInList() - scrollAmount);
        if ((items[i]->getYPosInList() + items[i]->getHeight()) > scrollAmount + height) {
            /* That item ended past the bottom of our pane. Done rendering. */
            break;
        }

    }
    EndScissorMode();
}

ListSelectorItem * ListSelector::getItemForMousePos(const raylib::Vector2 &pos) {
    raylib::Rectangle hitbox(x, y, width, height);
    int i;

    if (!hitbox.CheckCollision(pos)) {
        /* Not even in our pane. No collision taking place here */
        return nullptr;
    }

    /* Skip items that are not visible because we have scrolled past them */
    for (i = 0; i < items.size(); i++) {
        if ((items[i]->getYPosInList() + items[i]->getHeight()) > scrollAmount) {
            /* This item is in the visible pane. Start checking hitboxes */
            break;
        }
    }

    /* Start checking item hitboxes as long as we are still in the pane */
    for (; i < items.size(); i++) {
        hitbox.SetY(y + items[i]->getYPosInList() - scrollAmount);
        hitbox.SetHeight(items[i]->getHeight());
        if (hitbox.CheckCollision(pos)) {
            /* We found our collision item */
            return items[i];
        }
        if ((items[i]->getYPosInList() + items[i]->getHeight()) > scrollAmount + height) {
            /* That item ended past the bottom of our pane. Done checking for collisions. */
            break;
        }
    }

    return nullptr;
}

/* Implementation of ListSelectorTextItem */

ListSelectorTextItem::ListSelectorTextItem(float fontHeightPct, std::string &&text) {
    this->fontHeightPct = fontHeightPct;
    this->text = text;
    font = new raylib::Font(Util::formResourcePath(MARVEL_FONT_PATH), BASE_FONT_SIZE, nullptr, 95);
    hoverTex = new raylib::Texture(Util::formResourcePath(HOVER_BG_IMG_PATH));
    hoverTex->GenMipmaps();
    selectedTex = new raylib::Texture(Util::formResourcePath(SELECTED_BG_IMG_PATH));
    selectedTex->GenMipmaps();
    recalculateContentSize();
}

ListSelectorTextItem::~ListSelectorTextItem() {
    delete font;
    delete hoverTex;
    delete selectedTex;
}

void ListSelectorTextItem::heightChanged() {
    recalculateContentSize();
}

void ListSelectorTextItem::recalculateContentSize()
{
    fontSize = fontHeightPct * getHeight();
    fontSpacing = fontSize * FONT_SPACING_PCT;
    contentSize = font->MeasureText(text, fontSize, fontSpacing);
}

void ListSelectorTextItem::render(Renderer *renderer, float absoluteX, float absoluteY) {
    if (getSelected()) {
        renderer->drawTexture(selectedTex, absoluteX, absoluteY, getWidth(), getHeight(), getDependentOpacity());
    } else if (getHover()) {
        renderer->drawTexture(hoverTex, absoluteX, absoluteY, getWidth(), getHeight(), getDependentOpacity());
    }
    renderer->setColor(WHITE);
    renderer->drawText(*font, text, absoluteX + ((getWidth() - contentSize.x) / 2),
            absoluteY + ((getHeight() - contentSize.y) / 2), fontSize, fontSpacing,
            getDependentOpacity());
}