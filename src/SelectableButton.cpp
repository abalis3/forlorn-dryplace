#include "SelectableButton.h"

SelectableButton::SelectableButton(raylib::Texture *tex, raylib::Rectangle &selectedSrc,
        raylib::Rectangle &unselectedSrc, bool selected)
{
    contentTexture = tex;
    this->selectedSrc = selectedSrc;
    this->unselectedSrc = unselectedSrc;
    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.width = 0;
    dstRect.height = 0;
    this->selected = selected;
    callback = nullptr;
}

void SelectableButton::setHeight(float height)
{
    dstRect.height = height;
    dstRect.width = height * 3;
}

void SelectableButton::setX(float xPos)
{
    dstRect.x = xPos;
}

void SelectableButton::setY(float yPos)
{
    dstRect.y = yPos;
}

float SelectableButton::getWidth()
{
    return dstRect.width;
}

void SelectableButton::setSelected(bool selected)
{
    this->selected = selected;
}

void SelectableButton::setCallback(std::function<void(SelectableButton*)> cb)
{
    callback = cb;
}


void SelectableButton::onMousePressed(const raylib::Vector2 &pos)
{
    bool wasSelected = selected;
    
    if(dstRect.CheckCollision(pos)) {
        selected = true;
    }

    if (!wasSelected && selected && callback != nullptr) {
        callback(this);
    }
}

void SelectableButton::render(Renderer *renderer)
{
    if (selected) {
        renderer->setColor(GREEN);
    } else {
        renderer->setColor(RED);
    }

    renderer->drawRectangle(dstRect.x, dstRect.y, dstRect.width, dstRect.height);
}