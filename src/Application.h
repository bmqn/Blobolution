#pragma once

#include "Window.h"
#include "Layer.h"
#include "Event.h"

#include <memory>
#include <vector>

class Application
{
public:
	static Application &get()
	{
		static Application app;

		return app;
	}

public:
	void setTitle(std::string title) { m_window->setTitle(title); }

	void pushLayer(std::shared_ptr<Layer> layer);
	void run();
	void shutdown();

	Window& getWindow() { return *m_window; }

private:
	Application();

	void onEvent(Event &e);

	bool onWindowClose(WindowCloseEvent &e);
	bool onWindowResize(WindowResizeEvent &e);

private:
	bool m_Running;
	std::unique_ptr<Window> m_window;
	std::vector<std::shared_ptr<Layer>> m_Layers;
};
