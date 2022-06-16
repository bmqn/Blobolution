#include "Window.h"
#include "Log.h"

Window::~Window()
{
	Destroy();
}

void Window::Init(int width, int height, const std::string &title)
{
	m_Data.Width = width;
	m_Data.Height = height;
	m_Data.Title = title;

	// Create GLFW error callback
	glfwSetErrorCallback([](int error, const char *description)
	{
		BL_LOG("GLFW Error: %s", description);
	});

	// Init GLFW
	if (!glfwInit())
	{
		BL_LOG("Failed to initialize GLFW.");
		return;
	}

	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	// Configure the window
	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, &m_Data);
	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
	{
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		WindowCloseEvent e;
		data.Callback(e);
	});
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
	{
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowResizeEvent e(width, height);
		data.Callback(e);
	});
	glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		switch (action)
		{
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e(key, mods);
				data.Callback(e);
				break;
			}
			case GLFW_PRESS:
			{
				KeyPressedEvent e(key, mods);
				data.Callback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyRepeatEvent e(key, mods);
				data.Callback(e);
				break;
			}
		};
	});
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
	{
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		switch (action)
		{
			case GLFW_RELEASE:
			{
				MouseReleasedEvent e(button, mods);
				data.Callback(e);
				break;
			}
			case GLFW_PRESS:
			{
				MousePressedEvent e(button, mods);
				data.Callback(e);
				break;
			}
		};
	});
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos)
	{
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MouseMovedEvent e(xpos, ypos);
		data.Callback(e);
	});
	glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset)
	{
		WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MouseScrolledEvent e(xoffset, yoffset);
		data.Callback(e);
	});

	// Load OpenGL functions with GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		BL_LOG("Failed to initialize OpenGL context.");
		Destroy();
		return;
	}

	BL_LOG("Succesfully initialized GLFW and loaded OpenGL.");
	BL_LOG("OpenGL version %s, GLSL version %s.", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

void Window::Destroy()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}
