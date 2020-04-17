#include "MainMenu.h"

#include <iostream>

#define BACKGROUND_IMG_PATH "./res/MainMenu/desert-background.png"

#define BG_MAX_SPEED 0.02
#define BG_MIN_SPEED 0.005
#define BG_SPEED_CUTOFF 0.05

MainMenu::MainMenu()
{
    bgTexture = new raylib::Texture(BACKGROUND_IMG_PATH);
    bgSrcXPercent = 0;
    bgSrcXPos = 0;
    bgSrcXPosIncreasing = true;
}

MainMenu::~MainMenu()
{
    delete bgTexture;
}

void MainMenu::update(double secs)
{
    /* Update velocity of background */
    float bgVelocity = bgSrcXPosIncreasing ? 1 : -1;
    if (bgSrcXPercent < BG_SPEED_CUTOFF) {
        /* Slowing down with x pos percent close to 0 */
        bgVelocity *= BG_MIN_SPEED +
                (bgSrcXPercent / BG_SPEED_CUTOFF) *
                (BG_MAX_SPEED - BG_MIN_SPEED);
    } else if (bgSrcXPercent > (1 - BG_SPEED_CUTOFF)) {
        /* Slowing down with x pos percent close to 1 */
        bgVelocity *= BG_MIN_SPEED +
                ((1 - bgSrcXPercent) / BG_SPEED_CUTOFF) *
                (BG_MAX_SPEED - BG_MIN_SPEED);
    } else {
        /* Going full speed between the upper and lower cutoffs */
        bgVelocity *= BG_MAX_SPEED;
    }

    /* Update position of background */
    bgSrcXPercent += bgVelocity * secs;
    if (bgSrcXPercent > 1) {
        bgSrcXPercent = 1;
        bgSrcXPosIncreasing = false;
    } else if (bgSrcXPercent < 0) {
        bgSrcXPercent = 0;
        bgSrcXPosIncreasing = true;
    }
    bgSrcXPos = bgSrcXMax * bgSrcXPercent;
}

void MainMenu::render(Renderer *renderer)
{
    renderer->drawTexture(bgTexture, bgSrcXPos, 0, bgSrcWidth,
            bgTexture->GetHeight(), 0, 0, getWidth(), getHeight());
}

void MainMenu::onSizeChangedFrom(int oldWidth, int oldHeight)
{
    calculateBgSizeParams();
}

void MainMenu::calculateBgSizeParams()
{
    bgSrcWidth = (bgTexture->GetHeight() / (float) getHeight()) * getWidth();
    bgSrcXMax = bgTexture->GetWidth() - bgSrcWidth;
    bgSrcXPos = bgSrcXMax * bgSrcXPercent;
}