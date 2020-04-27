#include "Window.h"

#define WINDOW_TITLE "Forbidden Desert"
#define FPS_FONT_SIZE_DENOMINATOR 15   // (1 / x) of window height

/* Configurations for available windowed resolutions */
static const int NUM_WINDOWED_CONFIG_MODES = 12;
static const WindowConfiguration WINDOWED_CONFIG_MODES[NUM_WINDOWED_CONFIG_MODES] = {

    /*
     * IMPORTANT:
     * Keep above field NUM_WINDOWED_CONFIG_MODES equal to number of entries 
     * AND these entries MUST be kept in order of smallest to largest
     */
    {false, 1142, 658},
    {false, 1280, 720},
    {false, 1408, 792},
    {false, 1536, 864},
    {false, 1664, 936},
    {false, 1920, 1080},
    {false, 2176, 1274},
    {false, 2432, 1368},
    {false, 2560, 1440},
    {false, 3072, 1728},
    {false, 3584, 2016},
    {false, 3840, 2160}

};

int Window::getNumWindowedConfigModes()
{
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);
    int curModeIdx;

    for (curModeIdx = 0; curModeIdx < NUM_WINDOWED_CONFIG_MODES; curModeIdx++){
        WindowConfiguration config = WINDOWED_CONFIG_MODES[curModeIdx];
        if (config.windowWidth > monitorWidth || config.windowHeight > monitorHeight) {
            return curModeIdx;
        }
    }

    return NUM_WINDOWED_CONFIG_MODES;
}

const WindowConfiguration* Window::getWindowedConfigModes()
{
    return WINDOWED_CONFIG_MODES;
}

Window::Window()
{
    const struct WindowConfiguration defaultWindowConfig = {
        .isFullscreen = true,
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
    unsigned int windowFlags = FLAG_VSYNC_HINT;
    if (config.isFullscreen) windowFlags |= FLAG_FULLSCREEN_MODE;
    SetConfigFlags(windowFlags);

    /* TODO: Add this line to disable 'escape' key to close window */
    //SetExitKey(-1);

    SetTargetFPS(-1);
    raylibWindow = new raylib::Window(config.windowWidth,
            config.windowHeight, WINDOW_TITLE);

    currentScene = nullptr;
    showingFPS = false;
    recalculateSizeParams();
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
    if (raylibWindow->IsResized()) {
        recalculateSizeParams();
    }
    if (currentScene != nullptr) {
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

void Window::recalculateSizeParams()
{
    if (currentScene != nullptr) {
        currentScene->updateWindowSize(getWidth(), getHeight());
    }
    fpsFontSize = getHeight() / FPS_FONT_SIZE_DENOMINATOR;
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

void Window::updateConfiguration(const WindowConfiguration &config)
{
    if (config.isFullscreen != raylibWindow->IsFullscreen()){
        raylibWindow->ToggleFullscreen();
    }
    raylibWindow->SetSize(config.windowWidth, config.windowHeight);
    raylibWindow->SetPosition((GetMonitorWidth(0) - config.windowWidth) / 2,
            (GetMonitorHeight(0) - config.windowHeight) / 2);
    recalculateSizeParams();
}
