#include "Application.h"
#include "Log.h"
#include "Random.h"
#include "Renderer.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void Application::Run()
{
	m_Running = true;

	double prevTime = m_Window->GetTime();
	double accumulTime = 0.0;

	while (m_Running)
	{
		double currTime = m_Window->GetTime();
		double deltaTime = currTime - prevTime;
		prevTime = currTime;

		accumulTime += deltaTime;
		while (accumulTime >= k_UpdateDeltaTime)
		{
			for (std::unique_ptr<Layer> &layer : m_Layers)
			{
				layer->OnUpdate();
			}

			accumulTime -= k_UpdateDeltaTime;
		}

		for (std::unique_ptr<Layer>& layer : m_Layers)
		{
			layer->OnDraw();
		}

		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			for (std::unique_ptr<Layer> &layer : m_Layers)
			{
				layer->OnDrawImGui();
			}

			float windowWidth = static_cast<float>(GetWindow().GetWidth());
			float windowHeight = static_cast<float>(GetWindow().GetHeight());
			
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(windowWidth, windowHeight);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		m_Window->OnUpdate();

		Renderer::Clear();
	}

	Destroy();
}

void Application::Close()
{
	m_Running = false;
}

void Application::PushLayer(std::unique_ptr<Layer> layer)
{
	m_Layers.push_back(std::move(layer));
}

Application::Application()
	: m_Running(false)
	, m_Window(std::make_unique<Window>())
{
	Create();
}

void Application::Create()
{
	m_Window->Init(1280, 720, "Blobolution");
	m_Window->SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

	Random::Create();
	Renderer::Create();
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(GetWindow().GetGlfwWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

void Application::Destroy()
{
	BL_ASSERT(!m_Running, "The application is still running !");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Renderer::Destroy();
	Random::Destroy();
}

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

	// ImGui events
	{
		ImGuiIO& io = ImGui::GetIO();
	
		e.Handled |=   (e.GetType() == EventType::MouseButtonPressed
		            ||  e.GetType() == EventType::MouseButtonReleased
		            ||  e.GetType() == EventType::MouseMoved
		            ||  e.GetType() == EventType::MouseScrolled) & io.WantCaptureMouse;
		
		e.Handled |=   (e.GetType() == EventType::KeyPressed
		            ||  e.GetType() == EventType::KeyReleased
		            ||  e.GetType() == EventType::KeyRepeat
		            ||  e.GetType() == EventType::KeyTyped) & io.WantCaptureKeyboard;
	}

	for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
	{
		if (e.Handled)
			break;

		(*it)->OnEvent(e);
	}
}

bool Application::OnWindowClose(WindowCloseEvent &e)
{
	Close();
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent &e)
{
	Renderer::SetViewportSize(e.Width, e.Height);
	return false;
}