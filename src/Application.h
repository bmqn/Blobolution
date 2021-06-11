#pragma once

#include <memory>
#include <vector>

#include "Window.h"
#include "Layer.h"

class Application
{
private:

	bool running_;
	std::unique_ptr<Window> window_;
	std::vector<std::shared_ptr<Layer>> layers_;

public:
	Application();
	Application(int width, int height, std::string title);

	void setTitle(std::string title) { window_->setTitle(title);  }

	void run();

	void pushLayer(std::shared_ptr<Layer> layer);

private:
	void onEvent(Event& e);
	bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
};

