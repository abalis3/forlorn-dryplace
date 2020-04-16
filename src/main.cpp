#include <iostream>

#include "Window.h"

class TestScene : public Scene {
 
 public:

	TestScene() {
		position = 0;
		currentFPS = 0;
		textMeasured = false;
	}

	void render(Renderer *renderer) override {

		if (!textMeasured) {
			textWidth = renderer->measureText(welcomeText, fontSize);
			textMeasured = true;
		}

		renderer->setColor(GRAY);
		renderer->clearBackground();

		renderer->setColor(BLUE);
		renderer->drawRectangle(position, 0, barWidth, getHeight());

		renderer->setColor(BLACK);
		renderer->drawText(welcomeText, (getWidth() - textWidth) / 2, (getHeight() - fontSize) / 2, fontSize);
		
		renderer->setColor(GREEN);
		renderer->drawText(std::to_string(currentFPS), fontSize / 2, fontSize / 2, fontSize);
	}

	void update(double secs) override {
		position += velocity * secs;
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
	}

	void onSizeChangedFrom(int oldWidth, int oldHeight) override {
		fontSize = getHeight() / 15;
		barWidth = getWidth() / 30;
		barMax = getWidth() - barWidth;
		if (velocity < 0) {
			velocity = getWidth() / -5;
		} else {
			velocity = getWidth() / 5;
		}

		position = (position / oldWidth) * getWidth();

	}

	void updateFPS(int fps) {
		currentFPS = fps;
	}

private:

	const char *welcomeText = "Welcome to my Game!";
	int fontSize;
	int textWidth;
	int barWidth;
	int barMax;
	int velocity;
	double position;
	int currentFPS;
	bool textMeasured;

};

int main()
{
	Window window;
	TestScene scene;

	window.flipToScene(&scene);

	double lastTime = GetTime();
	while (!window.shouldClose()) {

		/* Update the scene */
		double elapsedTime = GetTime() - lastTime;
		lastTime += elapsedTime;
		scene.update(elapsedTime);
		scene.updateFPS(window.getFPS());

		/* Render the window */
		window.renderFrame();
	}

    return 0;
}