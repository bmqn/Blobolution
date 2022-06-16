#pragma once

#include "Window.h"
#include "Layer.h"
#include "Event.h"

class Application
{
public:
	static Application &Get()
	{
		static Application app;

		return app;
	}

public:
	void Run();
	void Close();

	void PushLayer(std::unique_ptr<Layer> layer);

	const Window& GetWindow() { return *m_Window; }

private:
	Application();

	void Create();
	void Destroy();

	void OnEvent(Event &e);

	bool OnWindowClose(WindowCloseEvent &e);
	bool OnWindowResize(WindowResizeEvent &e);

private:
	bool m_Running;
	
	std::unique_ptr<Window> m_Window;
	std::vector<std::unique_ptr<Layer>> m_Layers;
};
