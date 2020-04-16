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