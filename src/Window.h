#pragma once

#include "Event.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
private:
	struct WindowData
	{
		std::string Title;
		int Width, Height;

		std::function<void(Event &e)> Callback;
	};

private:
	GLFWwindow *m_Window;
	WindowData m_Data;

public:
	Window(int width, int height, std::string title);
	~Window();

	int getWidth() const { return m_Data.Width; }
	int getHeight() const { return m_Data.Height; }

	void setCallback(const std::function<void(Event &e)> &callback) { m_Data.Callback = callback; }
	void setTitle(std::string title) { glfwSetWindowTitle(m_Window, title.c_str()); }
	void setVSync(bool state) { glfwSwapInterval(state ? 1 : 0); }

	void onUpdate();
	void shutdown();
};
