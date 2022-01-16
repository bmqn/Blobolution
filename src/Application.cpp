#include "Application.h"

#include "Log.h"
#include "Random.h"

Application::Application()
	: m_Running(false),
	  m_window(std::make_unique<Window>(1280, 720, "Blobolution"))
{
	m_window->setCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
	
	Random::Init();
}

void Application::onEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::onWindowClose, this, std::placeholders::_1));
	dispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::onWindowResize, this, std::placeholders::_1));

	for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
	{
		if (e.Handled)
			break;

		(*it)->onEvent(e);
	}
}

void Application::run()
{
	BL_LOG_INFO("Run!");

	m_Running = true;

	double delta = 0, last = 0, current = 0;

	while (m_Running)
	{
		current = glfwGetTime();
		delta = current - last;
		last = current;

		for (auto &layer : m_Layers)
			layer->onUpdate((float)delta);

		m_window->onUpdate();
	}
}

void Application::shutdown()
{
	BL_LOG_INFO("Shutdown!");

	m_Running = false;
}

void Application::pushLayer(std::shared_ptr<Layer> layer)
{
	BL_LOG_INFO("Pushing \"", layer->getName(), "\" to the layer stack.");

	m_Layers.push_back(layer);
}

bool Application::onWindowClose(WindowCloseEvent &e)
{
	BL_LOG_INFO("Window closed!");

	shutdown();

	return false;
}

bool Application::onWindowResize(WindowResizeEvent &e)
{
	BL_LOG_INFO("Window resized to ", e.Width, " x ", e.Height);

	glViewport(0, 0, e.Width, e.Height);
	return false;
}