#include "LoadingSpinner.h"

#include "Util.h"

static const char SPINNER_IMG_PATH[] = "LoadingSpinner/spinner.png";

static const float ROTATION_SPEED = 250; /* Rotation speed in degrees per second */

LoadingSpinner::LoadingSpinner()
{
    /* Load necessary resources */
    spinnerTex = new raylib::Texture(Util::formResourcePath(SPINNER_IMG_PATH));
    spinnerTex->GenMipmaps();

    rotation = 0;
}

LoadingSpinner::~LoadingSpinner()
{
    delete spinnerTex;
}

void LoadingSpinner::setSize(float px)
{
    this->size = px;
}

void LoadingSpinner::setX(float xPos)
{
    centerPos.x = xPos;
}

void LoadingSpinner::setY(float yPos)
{
    centerPos.y = yPos;
}

float LoadingSpinner::getSize()
{
    return size;
}

void LoadingSpinner::update(double secs)
{
    rotation -= ROTATION_SPEED * secs;
    while (rotation < 0) {
        rotation += 360;
    }
}

void LoadingSpinner::render(Renderer *renderer)
{
    raylib::Rectangle srcRect = 
            Rectangle{0,0,(float)spinnerTex->GetWidth(),(float)spinnerTex->GetHeight()};

    raylib::Rectangle dstRect;
    dstRect.width = dstRect.height = size;
    dstRect.x = centerPos.x;
    dstRect.y = centerPos.y;

    raylib::Vector2 originPt = Vector2{dstRect.width / 2, dstRect.height / 2};

    raylib::Color renderColor = Color{255,255,255,(unsigned char)(getDependentOpacity()*255)};

    renderer->drawTexture(spinnerTex, srcRect, dstRect, originPt, rotation, renderColor);
}
