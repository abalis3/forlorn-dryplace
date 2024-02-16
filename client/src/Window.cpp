#include "Window.h"

#include <fstream>

#include "Util.h"
#include "pbuf/generated/WindowConfiguration.pb.h"

static const char CFG_PREFS_FILE[] = "window_cfg.dat";
static const char WINDOW_TITLE[] = "Forbidden Desert";
static const int FPS_FONT_SIZE_DENOMINATOR = 15; // (1 / x) of window height

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
    struct WindowConfiguration windowConfig = {
        true,   /* isFullscreen */
        0,      /* windowWidth */
        0       /* windowHeight */
    };

    /* If there are saved preferences for window cofig, load them in using pbufs */
    std::fstream cfgFileStream(Util::formPrefsPath(CFG_PREFS_FILE),
            std::ios::in | std::ios::binary);
    if (cfgFileStream.is_open()) {
        pbuf::WindowConfiguration cfgPbuf;
        cfgPbuf.ParseFromIstream(&cfgFileStream);
        windowConfig.isFullscreen = cfgPbuf.fullscreen();
        windowConfig.windowWidth = cfgPbuf.window_width();
        windowConfig.windowHeight = cfgPbuf.window_height();
        cfgFileStream.close();
    }

    constructWindow(windowConfig);
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
    return raylibWindow->GetWidth();
}

int Window::getHeight()
{
    return raylibWindow->GetHeight();
}

int Window::getFPS()
{
    return raylibWindow->GetFPS();
}

bool Window::isFullscreen()
{
    return raylibWindow->IsFullscreen();
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

        /* Characters for typing */
        int key = GetKeyPressed();
        while (key > 0) {
            /* Make keys universally "Upper Case" */
            if (key >= 'a' && key <= 'z') {
                key += 'A' - 'a';
            }
            currentScene->onKeyPressed(key);
            key = GetKeyPressed();
        }
    }

    if (IsKeyPressed(KEY_F3)) {
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
    if (!config.isFullscreen && raylibWindow->IsFullscreen()){
        raylibWindow->ToggleFullscreen();
    }

    if (config.windowWidth != getWidth() || config.windowHeight != getHeight()) {
        if (config.windowWidth == 0 && config.windowHeight == 0) {
            raylibWindow->SetSize(GetMonitorWidth(0), GetMonitorHeight(0));
        } else {
            raylibWindow->SetSize(config.windowWidth, config.windowHeight);
        }
    }

    if (config.isFullscreen && !raylibWindow->IsFullscreen()){
        raylibWindow->ToggleFullscreen();
    }

    raylibWindow->SetPosition((GetMonitorWidth(0) - config.windowWidth) / 2,
            (GetMonitorHeight(0) - config.windowHeight) / 2);
    recalculateSizeParams();

    /* Write pbuf to preferences directory with new WindowConfiguration */
    std::fstream cfgFileStream(Util::formPrefsPath(CFG_PREFS_FILE),
            std::ios::out | std::ios::trunc | std::ios::binary);
	if (cfgFileStream.is_open()) {
		pbuf::WindowConfiguration cfgPbuf;
		cfgPbuf.set_fullscreen(config.isFullscreen);
		cfgPbuf.set_window_width(config.windowWidth);
		cfgPbuf.set_window_height(config.windowHeight);
		cfgPbuf.SerializeToOstream(&cfgFileStream);
		cfgFileStream.close();
	}
}
