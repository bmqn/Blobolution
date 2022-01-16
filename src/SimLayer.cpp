#include "SimLayer.h"
#include "Application.h"
#include "Log.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

bool SimLayer::onMouseMoved(MouseMovedEvent &e)
{
	static double xpos = 0.0;
	static double ypos = 0.0;

	if (m_MousePressed && !m_FollowCam)
	{
		m_CamPosition -= glm::vec3{e.Xpos - xpos, ypos - e.Ypos, 0.0f} * (0.05f / m_CamScale);
	}

	xpos = e.Xpos;
	ypos = e.Ypos;

	return false;
}

bool SimLayer::onMousePressed(MousePressedEvent &e)
{
	if (e.Button == GLFW_MOUSE_BUTTON_1)
	{
		if (e.Action == GLFW_PRESS)
			m_MousePressed = true;
		else
			m_MousePressed = false;
	}

	return false;
}

bool SimLayer::onMouseScrolled(MouseScrolledEvent &e)
{
	m_CamScale += e.Yoffset * (0.3f * m_CamScale);

	if (m_CamScale < 0.01f)
		m_CamScale = 0.01f;
	if (m_CamScale > 2.0f)
		m_CamScale = 2.0f;

	return false;
}

bool SimLayer::onKeyPressed(KeyPressedEvent &e)
{
	switch(e.Key)
	{
		case GLFW_KEY_ESCAPE:
			Application::get().shutdown();
			break;
		case GLFW_KEY_Q:
			m_TimeScale = m_TimeScale >= 10.0 ? m_TimeScale : m_TimeScale + 0.2;
			BL_LOG_INFO("Timescale increased to ", m_TimeScale);
			break;
		case GLFW_KEY_F:
			m_FollowCam = !m_FollowCam;
			BL_LOG_INFO("Follow camera mode ", m_FollowCam ? "enabled" : "disabled");
			break;
		case GLFW_KEY_Z:
			m_TimeScale = m_TimeScale > 0.2 ? m_TimeScale - 0.2 : m_TimeScale;
			BL_LOG_INFO("Timescale decreased to ", m_TimeScale);
			break;
	}	

	return false;
}

SimLayer::SimLayer()
	: Layer("Simulation Layer"),
	  m_CamPosition(0, 0, 0),
	  m_CamScale(0.5f),
	  m_TimeScale(1.0f),
	  m_MousePressed(false),
	  m_FollowCam(false),
	  m_Generation(25)
{
	m_Generation.init();
	m_Generation.beginSimulating();
}

SimLayer::~SimLayer()
{
}

void SimLayer::onUpdate(float delta)
{
	m_Generation.update(delta * m_TimeScale);

	if (m_FollowCam)
		m_CamPosition = m_Generation.getPositionOfBestCar();

	float width = Application::get().getWindow().getWidth();
	float height = Application::get().getWindow().getHeight();
	float aspect = width / height;

	auto viewProj =
		glm::ortho(-10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(m_CamScale)) * glm::translate(glm::mat4(1.0f), -m_CamPosition);

	m_Renderer.beginScene(viewProj);

	m_Generation.draw(m_Renderer);

	m_Renderer.endScene();

	auto stats = m_Renderer.getRendererStats();

	// ImGui_ImplOpenGL3_NewFrame();
	// ImGui_ImplGlfw_NewFrame();
	// ImGui::NewFrame();

	// ImGui::Begin("Options");

	// ImGui::BeginChild("Infomation", ImVec2(0, 80));
	// ImGui::Text("Delta (ms) %f", delta);
	// ImGui::Text("Render Submissions %d", stats.rendererSubmissions);
	// ImGui::Text("Render Vertices %d", stats.rendererVertices);
	// ImGui::Text("Render Draws %d", stats.rendererDrawCalls);
	// ImGui::EndChild();

	// ImGui::Separator();

	// ImGui::BeginChild("Settings", ImVec2(0, 60));
	// ImGui::SliderFloat("Time Scale", &timeScale_, 0.1f, 5.0f);
	// ImGui::Checkbox("Follow Camera", &followCam_);
	// ImGui::EndChild();

	// ImGui::End();

	// ImGui::Render();
	// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SimLayer::onEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseMovedEvent>(std::bind(&SimLayer::onMouseMoved, this, std::placeholders::_1));
	dispatcher.dispatch<MouseScrolledEvent>(std::bind(&SimLayer::onMouseScrolled, this, std::placeholders::_1));
	dispatcher.dispatch<MousePressedEvent>(std::bind(&SimLayer::onMousePressed, this, std::placeholders::_1));
	dispatcher.dispatch<KeyPressedEvent>(std::bind(&SimLayer::onKeyPressed, this, std::placeholders::_1));
}
