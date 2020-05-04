#include "Renderer.h"

Renderer::Renderer()
{
    /* Start off with Black */
    currentColor = new raylib::Color(BLACK);
}

Renderer::~Renderer()
{
    delete currentColor;
}

void Renderer::start()
{
    BeginDrawing();
}

void Renderer::stop()
{
    EndDrawing();
}

void Renderer::setColor(Color color)
{
    delete currentColor;
    currentColor = new raylib::Color(color);
}

void Renderer::clearBackground()
{
    currentColor->ClearBackground();
}

void Renderer::drawRectangle(int x, int y, int w, int h)
{
    currentColor->DrawRectangle(x, y, w, h);
}

void Renderer::drawText(const char *text, int x, int y, int fontSize)
{
    currentColor->DrawText(text, x, y, fontSize);
}

void Renderer::drawText(std::string text, int x, int y, int fontSize)
{
    drawText(text.c_str(), x, y, fontSize);
}

int Renderer::measureText(const char *text, int fontSize)
{
    return MeasureText(text, fontSize);
}

int Renderer::measureText(std::string text, int fontSize)
{
    return measureText(text.c_str(), fontSize);
}

void Renderer::drawTexture(raylib::Texture *tex, float srcX, float srcY,
        float srcW, float srcH, float dstX, float dstY, float dstW, float dstH)
{
    tex->Draw(Rectangle{srcX,srcY,srcW,srcH}, Rectangle{dstX,dstY,dstW,dstH},
            Vector2{0,0}, 0, WHITE);
}

void Renderer::drawTexture(raylib::Texture *tex, float dstX, float dstY,
        float dstW, float dstH, float opacity)
{
    tex->Draw(Rectangle{0,0,(float)tex->GetWidth(),(float)tex->GetHeight()},
            Rectangle{dstX,dstY,dstW,dstH}, Vector2{0,0}, 0,
            Color{255,255,255,(unsigned char)(255*opacity)});
}

void Renderer::drawTexture(raylib::Texture *tex, float dstX, float dstY,
        float dstW, float dstH, float opacity, bool flipX)
{
    float srcW = (float) tex->GetWidth();
    if (flipX) srcW *= -1;
    tex->Draw(Rectangle{0,0,srcW,(float)tex->GetHeight()},
            Rectangle{dstX,dstY,dstW,dstH}, Vector2{0,0}, 0,
            Color{255,255,255,(unsigned char)(255*opacity)});
}

void Renderer::drawTexture(raylib::Texture *tex, const raylib::Rectangle &src,
        const raylib::Rectangle &dst)
{
    tex->Draw(src, dst, Vector2{0,0}, 0, WHITE);
}

void Renderer::drawTexture(raylib::Texture *tex, const raylib::Rectangle &src,
        const raylib::Rectangle &dst, float opacity)
{
    tex->Draw(src, dst, Vector2{0,0}, 0, Color{255,255,255,(unsigned char)(255*opacity)});
}

void Renderer::drawTexture(raylib::Texture *tex, const raylib::Rectangle &dst, float opacity)
{
    Rectangle src = {0, 0, (float) tex->GetWidth(), (float) tex->GetHeight()};
    tex->Draw(src, dst, Vector2{0,0}, 0, Color{255,255,255,(unsigned char)(255*opacity)});
}