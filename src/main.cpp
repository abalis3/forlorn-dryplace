#include "raylib/raylib.hpp"
#include <iostream>

#include "Window.h"

int main()
{
	const char *welcomeText = "Welcome to my Game!";
	double position = 0;

	raylib::Color background(WHITE);
	raylib::Color barColor(BLUE);
	raylib::Color textColor(BLACK);
	raylib::Color fpsColor(GREEN);
	Window w;

	int windowWidth = w.getWidth();
	int windowHeight = w.getHeight();

	double lastTime = GetTime();
	int fontSize = windowHeight / 15;
	int textWidth = MeasureText(welcomeText, fontSize);
	int barWidth = windowWidth / 30;
	int barMax = windowWidth - barWidth;
	int velocity = windowWidth / 5;

	// Main game loop
	while (!w.shouldClose())    // Detect window close button or ESC key
	{
		//----------------------------------------------------------------------------------
		// Update
		//----------------------------------------------------------------------------------
		double elapsedTime = GetTime() - lastTime;
		lastTime += elapsedTime;
		position += velocity * elapsedTime;
		while (true) {
			if (position > barMax) {
				position = 2*barMax - position;
			} else if (position < 0) {
				position = -position;
			} else {
				break;
			}
			velocity = -velocity;
		}

		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		background.ClearBackground();
		barColor.DrawRectangle(position, 0, barWidth, windowHeight);
		textColor.DrawText(welcomeText, (windowWidth - textWidth) / 2, (windowHeight - fontSize) / 2, fontSize);
		fpsColor.DrawText(std::to_string(w.getFPS()), fontSize / 2, fontSize / 2, fontSize);

		EndDrawing();
	}

    return 0;
}