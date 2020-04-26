#include "Window.h"

#define WINDOW_TITLE "Forbidden Desert"
#define FPS_FONT_SIZE_DENOMINATOR 15   // (1 / x) of window height

Window::Window()
{
    const struct WindowConfiguration defaultWindowConfig = {
        .isFullscreen = true,
        .vsyncEnabled = true,
        .targetFPS = -1,
        .windowWidth = 0,
        .windowHeight = 0
    };

    constructWindow(defaultWindowConfig);
}

Window::Window(const struct WindowConfiguration &config)
{
    constructWindow(config);
}

void Window::constructWindow(const struct WindowConfiguration &config)
{
    unsigned int windowFlags = 0;
    if (config.isFullscreen) windowFlags |= FLAG_FULLSCREEN_MODE;
    if (config.vsyncEnabled) windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);

    /* TODO: Add this line to disable 'escape' key to close window */
    //SetExitKey(-1);

    SetTargetFPS(config.targetFPS);
    raylibWindow = new raylib::Window(config.windowWidth,
            config.windowHeight, WINDOW_TITLE);

    currentScene = nullptr;
    showingFPS = false;
    fpsFontSize = getHeight() / FPS_FONT_SIZE_DENOMINATOR;
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
    if (currentScene != nullptr) {
        if (raylibWindow->IsResized()) {
            currentScene->updateWindowSize(getWidth(), getHeight());
            fpsFontSize = getHeight() / FPS_FONT_SIZE_DENOMINATOR;
        }
        const Vector2 mousePos = mouse.GetPosition();
        currentScene->onMousePosUpdate(mousePos);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScene->onMouseButtonPressed(MOUSE_LEFT_BUTTON, mousePos);
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            currentScene->onMouseButtonPressed(MOUSE_RIGHT_BUTTON, mousePos);
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentScene->onMouseButtonReleased(MOUSE_LEFT_BUTTON, mousePos);
        }
        if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            currentScene->onMouseButtonReleased(MOUSE_RIGHT_BUTTON, mousePos);
        }
    }

    if (IsKeyPressed(KEY_ZERO)) {
        showingFPS = !showingFPS;
    }
}

void Window::renderFrame()
{
    renderer.start();
    if (currentScene != nullptr) {
        currentScene->render(&renderer);
    }
    if (showingFPS) renderFPS();
    renderer.stop();
}

void Window::renderFPS()
{
    int textInset = fpsFontSize / 2;
    int boxPadding = fpsFontSize / 8;
    int boxLocation = textInset - boxPadding;
    renderer.setColor(BLUE);
    renderer.drawRectangle(boxLocation, textInset,
            renderer.measureText(std::to_string(getFPS()), fpsFontSize) +
            2*boxPadding, fpsFontSize);
    renderer.setColor(GREEN);
	renderer.drawText(std::to_string(getFPS()), textInset, textInset, fpsFontSize);
}