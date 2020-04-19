#include "MainMenu.h"

#include "Util.h"

#define BACKGROUND_IMG_PATH "MainMenu/desert-background.png"
#define TITLE_IMG_PATH "MainMenu/main-title.png"

#define BG_MAX_SPEED 0.02
#define BG_MIN_SPEED 0.005
#define BG_SPEED_CUTOFF 0.05

#define TITLE_TOP_PADDING_PCT 0.09
#define TITLE_HEIGHT_PCT 0.275
#define TITLE_FADE_IN_START 0.5
#define TITLE_FADE_IN_END 3

MainMenu::MainMenu()
{
    bgTexture = new raylib::Texture(
            Util::formResourcePath(BACKGROUND_IMG_PATH));
    titleTexture = new raylib::Texture(
            Util::formResourcePath(TITLE_IMG_PATH));
    bgSrcXPercent = 0;
    bgSrcXPosIncreasing = true;
    titleOpacity = 0;
    titleFadeStopwatch = 0;
}

MainMenu::~MainMenu()
{
    delete bgTexture;
    delete titleTexture;
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

    /* Update opacity of main title for fade in */
    if (titleFadeStopwatch < TITLE_FADE_IN_END) {
        titleFadeStopwatch += secs;
        if (titleFadeStopwatch >= TITLE_FADE_IN_END) {
            titleOpacity = 1;
        } else if (titleFadeStopwatch > TITLE_FADE_IN_START) {
            titleOpacity = (titleFadeStopwatch - TITLE_FADE_IN_START) /
                    (TITLE_FADE_IN_END - TITLE_FADE_IN_START);
        }
    } else {
        titleOpacity = 1;
    }
}

void MainMenu::render(Renderer *renderer)
{
    renderer->drawTexture(bgTexture, bgSrcXPos, 0, bgSrcWidth,
            bgTexture->GetHeight(), 0, 0, getWidth(), getHeight());

    renderer->drawTexture(titleTexture, titleXPos, titleYPos, titleWidth,
            titleHeight, titleOpacity);
}

void MainMenu::onSizeChangedFrom(int oldWidth, int oldHeight)
{
    calculateBgSizeParams();
    calculateTitleSizeParams();
}

void MainMenu::calculateBgSizeParams()
{
    bgSrcWidth = (bgTexture->GetHeight() / (float) getHeight()) * getWidth();
    bgSrcXMax = bgTexture->GetWidth() - bgSrcWidth;
    bgSrcXPos = bgSrcXMax * bgSrcXPercent;
}

void MainMenu::calculateTitleSizeParams()
{
    titleYPos = getHeight() * TITLE_TOP_PADDING_PCT;
    titleHeight = getHeight() * TITLE_HEIGHT_PCT;
    titleWidth = (titleHeight / titleTexture->GetHeight()) *
            titleTexture->GetWidth();
    titleXPos = (getWidth() - titleWidth) / 2;
}