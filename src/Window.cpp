#include "Window.h"

#include "Log.h"

// #include "imgui.h"
// #include "imgui/backends/imgui_impl_glfw.h"
// #include "imgui/backends/imgui_impl_opengl3.h"

// #define IMGUI_IMPL_OPENGL_LOADER_GLAD

// #include "imgui/backends/imgui_impl_glfw.cpp"
// #include "imgui/backends/imgui_impl_opengl3.cpp"

Window::~Window()
{
	shutdown();
}

Window::Window(int width, int height, std::string title)
{
	m_Data.Width = width;
	m_Data.Height = height;
	m_Data.Title = title;

	int init = glfwInit();

	glfwSetErrorCallback([](int error, const char *description)
						 { BL_LOG_ERROR("GLFW Error. ", description); });

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(m_Window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowUserPointer(m_Window, &m_Data);

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
							   {
								   WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

								   WindowCloseEvent e;
								   data.Callback(e);
							   });

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
							  {
								  WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

								  WindowResizeEvent e(width, height);
								  data.Callback(e);
							  });

	glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
					   {
						   WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

							switch (action)
							{
								case GLFW_PRESS:
									KeyPressedEvent e(key, mods);
						   			data.Callback(e);
									break;
							}
					   });

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
							   {
								   WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

								   MousePressedEvent e(button, action, mods);
								   data.Callback(e);
							   });

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos)
							 {
								 WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

								 MouseMovedEvent e(xpos, ypos);
								 data.Callback(e);
							 });

	glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset)
						  {
							  WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

							  MouseScrolledEvent e(xoffset, yoffset);
							  data.Callback(e);
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
	glfwSwapBuffers(m_Window);
}

void Window::shutdown()
{
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}
