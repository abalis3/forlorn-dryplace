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

    raylib::Texture *bgTexture;

    float bgSrcXMax;
    float bgSrcXPercent;
    float bgSrcXPos;
    float bgSrcWidth;
    bool bgSrcXPosIncreasing;

};

#endif