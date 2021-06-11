#include "Window.h"

#include <iostream>
#include <stdio.h>

// #include "imgui.h"
// #include "imgui/backends/imgui_impl_glfw.h"
// #include "imgui/backends/imgui_impl_opengl3.h"

// #define IMGUI_IMPL_OPENGL_LOADER_GLAD

// #include "imgui/backends/imgui_impl_glfw.cpp"
// #include "imgui/backends/imgui_impl_opengl3.cpp"

Window::~Window()
{
	onShutdown();
}

Window::Window(int width, int height, std::string title)
{
	data_.width = width;
	data_.height = height;
	data_.title = title;

	if (!glfwInit())
		std::cout << "GLFW Init Error!" << std::endl;

	glfwSetErrorCallback([](int error, const char* description)
	{
			printf("GLFW Error! %s", description); 
	});

	window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(window_);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Glad init error!" << std::endl;

	glfwSwapInterval(0);

	glfwSetWindowUserPointer(window_, &data_);

	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		WindowCloseEvent e;
		data.callback(e);
	});

	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		WindowResizeEvent e(width, height);
		data.callback(e);
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		KeyPressedEvent e(key, mods);
		data.callback(e);
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MousePressedEvent e(button, action, mods);
		data.callback(e);
	});

	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent e(xpos, ypos);
		data.callback(e);
	});

	glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent e(xoffset, yoffset);
		data.callback(e);
	});


	// // Setup Dear ImGui context
	// IMGUI_CHECKVERSION();
	// ImGui::CreateContext();

	// // Setup Dear ImGui style
	// ImGui::StyleColorsDark();

	// // Setup Platform/Renderer bindings
	// ImGui_ImplGlfw_InitForOpenGL(window_, true);
	// ImGui_ImplOpenGL3_Init("#version 410");
}

void Window::onUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(window_);
}

void Window::onShutdown()
{
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();

	glfwDestroyWindow(window_);
	glfwTerminate();
}
