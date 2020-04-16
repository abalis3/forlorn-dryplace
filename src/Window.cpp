#include "Window.h"

#define WINDOW_TITLE "Forbidden Desert"

Window::Window()
{
    const struct WindowConfiguration defaultWindowConfig = {
        .isFullscreen = true,
        .vsyncEnabled = true,
        .targetFPS = -1,
        .windowWidth = 0,
        .windowHeight = 0
    };

    constructWindow(&defaultWindowConfig);
}

Window::Window(const struct WindowConfiguration *config)
{
    constructWindow(config);
}

void Window::constructWindow(const struct WindowConfiguration *config)
{
    unsigned int windowFlags = 0;
    if (config->isFullscreen) windowFlags |= FLAG_FULLSCREEN_MODE;
    if (config->vsyncEnabled) windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);

    /* TODO: Add this line to disable 'escape' key to close window */
    //SetExitKey(-1);

    SetTargetFPS(config->targetFPS);
    raylibWindow = new raylib::Window(config->windowWidth,
            config->windowHeight, WINDOW_TITLE);

    currentScene = nullptr;
}

Window::~Window()
{
    delete raylibWindow;
}

int Window::getWidth()
{
    return raylibWindow->GetScreenWidth();
}

int Window::getHeight()
{
    return raylibWindow->GetScreenHeight();
}

int Window::getFPS()
{
    return raylibWindow->GetFPS();
}

bool Window::shouldClose()
{
    return raylibWindow->ShouldClose();
}

void Window::flipToScene(Scene *newScene)
{
    currentScene = newScene;
    currentScene->updateWindowSize(getWidth(), getHeight());
}

void Window::update(double secs)
{
    /* TODO: check if window resized and notify scene if so */
}

void Window::renderFrame()
{
    renderer.start();
    if (currentScene != nullptr) {
        currentScene->render(&renderer);
    }
    renderer.stop();
}