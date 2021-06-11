#include "Application.h"

#include <functional>

#include "Renderer.h"

Application::Application() :
	running_(false), window_(std::make_unique<Window>(1280, 720, "Window"))
{
	window_->setCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
}

Application::Application(int width, int height, std::string title)
	 :running_(false), window_(std::make_unique<Window>(width, height, title))
{
	window_->setCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
}

void Application::onEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::onWindowClose, this, std::placeholders::_1));
	dispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::onWindowResize, this, std::placeholders::_1));

	for (auto it = layers_.rbegin(); it != layers_.rend(); ++it)
	{
		if (e.handled_)
			break;

		(*it)->onEvent(e);
	}
}

void Application::run()
{
	running_ = true;

	double delta = 0, last = 0, current = 0;

	while (running_)
	{
		current = glfwGetTime();
		delta = current - last;
		last = current;

		for (auto& layer : layers_)
			layer->onUpdate((float) delta);

		window_->onUpdate();
	}
}

void Application::pushLayer(std::shared_ptr<Layer> layer)
{
	layers_.push_back(layer);
}

bool Application::onWindowClose(WindowCloseEvent& e)
{
	running_ = false;
	return false;
}

bool Application::onWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.width_, e.height_);
	return false;
}