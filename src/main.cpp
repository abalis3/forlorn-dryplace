#include <iostream>

#include "Window.h"
#include "MainMenu.h"
#include "Util.h"

using namespace std::placeholders;

/*
 * This class represents the game program as a whole and manages
 * the top-level execution of scenes, the game loop, and
 * connecting the high-level components to run.
 * NOTE: the member functions are implemented below in this same file
 */
class GameRunner {
 public:

 	/* Initialize the opening scene for the game program */
	GameRunner();

	/* Free any objects allocated for the game program */
	~GameRunner();

	/* Entrypoint for main game execution and loop */
	void run();

 private:

	/* The Window used by the program instance */
	Window *window;

	/* The main menu in use when main menu is shown */
	MainMenu *mainMenu;

	/* 
	 * The callback that gets called when an object in the program requests a change
	 * to the window configuration / graphics settings
	 */
	void onWindowRequest(const WindowConfiguration &config);
};

GameRunner::GameRunner()
{
	window = new Window();
	mainMenu = new MainMenu(*window);
	mainMenu->setWindowRequestCallback(std::bind(&GameRunner::onWindowRequest, this, _1));
	window->flipToScene(mainMenu);
}

GameRunner::~GameRunner()
{
	delete mainMenu;
	delete window;
}

void GameRunner::run()
{
	double lastTime = GetTime();
	while (!window->shouldClose()) {

		/* Update the window and scene */
		double elapsedTime = GetTime() - lastTime;
		lastTime += elapsedTime;
		window->update(elapsedTime);
		mainMenu->update(elapsedTime);
		if (mainMenu->getReturnCode() == MainMenu::ReturnCode::EXIT_PROGRAM){
			break;
		}

		/* Render the window */
		window->renderFrame();
	}
}

void GameRunner::onWindowRequest(const WindowConfiguration &config)
{
	window->updateConfiguration(config);
}

/* EXECUTUION ENTRY POINT - MAIN FUNCTION */
int main(int argc, char *argv[])
{
	Util::registerRunArgs(argc, argv);
	GameRunner runner;
	runner.run();
    return 0;
}