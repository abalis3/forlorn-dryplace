#include <iostream>

#include "Window.h"
#include "MainMenu.h"

class GameRunner {
 public:
	GameRunner();
	~GameRunner();
	void run();
 private:
	Window *window;
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

int main()
{
	GameRunner runner;
	runner.run();
    return 0;
}