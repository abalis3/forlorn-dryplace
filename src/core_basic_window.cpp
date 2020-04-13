#include "raylib/raylib.hpp"
#include <iostream>

int main()
{
	const char *welcomeText = "Welcome to my Game!";
	double position = 0;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_FULLSCREEN_MODE);

	raylib::Color background(WHITE);
	raylib::Color barColor(BLUE);
	raylib::Color textColor(BLACK);
	raylib::Color fpsColor(GREEN);
	raylib::Window w(0, 0, "Forbidden Desert");

	int monitorWidth = GetMonitorWidth(0);
	int monitorHeight = GetMonitorHeight(0);

	double lastTime = GetTime();
	int fontSize = monitorHeight / 15;
	int textWidth = MeasureText(welcomeText, fontSize);
	int barWidth = monitorWidth / 30;
	int barMax = monitorWidth - barWidth;
	int velocity = monitorWidth / 5;


	// Main game loop
	while (!w.ShouldClose())    // Detect window close button or ESC key
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
		barColor.DrawRectangle(position, 0, barWidth, monitorHeight);
		textColor.DrawText(welcomeText, (monitorWidth - textWidth) / 2, (monitorHeight - fontSize) / 2, fontSize);
		fpsColor.DrawText(std::to_string(GetFPS()), fontSize / 2, fontSize / 2, fontSize);

		EndDrawing();
	}

	return 1;
}
