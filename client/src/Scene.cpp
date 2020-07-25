#include "Scene.h"

void Scene::updateWindowSize(int newWidth, int newHeight)
{
    int oldWidth = width;
    int oldHeight = height;
    width = newWidth;
    height = newHeight;
    this->onSizeChangedFrom(oldWidth, oldHeight);
}

int Scene::getWidth()
{
    return width;
}

int Scene::getHeight()
{
    return height;
}
