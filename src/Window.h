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
	Window() = default;
	~Window();

	void Init(int width, int height, const std::string &title);
	void OnUpdate();
	void Destroy();

	int GetWidth() const { return m_Data.Width; }
	int GetHeight() const { return m_Data.Height; }

	void SetCallback(const std::function<void(Event &e)> &callback) { m_Data.Callback = callback; }
	void SetTitle(const std::string &title) { glfwSetWindowTitle(m_Window, title.c_str()); }
	void SetSwapInterval(bool state) { glfwSwapInterval(state ? 1 : 0); }

	double GetTime() const { return glfwGetTime(); }

	inline GLFWwindow *GetGlfwWindow() const { return m_Window; }
};
