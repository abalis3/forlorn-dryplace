#ifndef FD__MAINMENU_H
#define FD__MAINMENU_H

#include "Scene.h"

class MainMenu : public Scene {
 public:

    MainMenu();
    ~MainMenu();
    
    void update(double secs);

    void render(Renderer *renderer);

    void onSizeChangedFrom(int oldWidth, int oldHeight);

 private:

    void calculateBgSizeParams();

    void calculateTitleSizeParams();

    raylib::Texture *bgTexture;
    raylib::Texture *titleTexture;

    float bgSrcXMax;
    float bgSrcXPercent;
    float bgSrcXPos;
    float bgSrcWidth;
    bool bgSrcXPosIncreasing;

    float titleXPos;
    float titleYPos;
    float titleWidth;
    float titleHeight;
    float titleOpacity;
    float titleFadeStopwatch;

};

#endif