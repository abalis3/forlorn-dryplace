#include <iostream>

#include "Window.h"
#include "MainMenu.h"
#include "Util.h"

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
};

GameRunner::GameRunner()
{
	window = new Window();
	mainMenu = new MainMenu();
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

		/* Render the window */
		window->renderFrame();
	}
}

/* EXECUTUION ENTRY POINT - MAIN FUNCTION */
int main(int argc, char *argv[])
{
	Util::registerRunArgs(argc, argv);
	GameRunner runner;
	runner.run();
    return 0;
}