#pragma once

#include <string>
#include <functional>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Event.h"

struct WindowData
{
	std::string title;
	int width, height;

	std::function<void(Event& e)> callback;
};

class Window
{

private:
	GLFWwindow* window_;
	WindowData data_;

public:
	Window(int width, int height, std::string title);
	~Window();

	void setCallback(const std::function<void(Event& e)>& callback) { data_.callback = callback; }
	void setTitle(std::string title) { glfwSetWindowTitle(window_, title.c_str()); }
	void setVSync(bool state) { glfwSwapInterval(state ? 1 : 0); }

	void onUpdate();
	void onShutdown();
};

