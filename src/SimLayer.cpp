#include "SimLayer.h"
#include "Application.h"
#include "Log.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

bool SimLayer::OnMouseMoved(MouseMovedEvent &e)
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

bool SimLayer::OnMousePressed(MousePressedEvent &e)
{
	if (e.Button == GLFW_MOUSE_BUTTON_1)
	{
		m_MousePressed = true;
	}

	return false;
}

bool SimLayer::OnMouseReleased(MouseReleasedEvent &e)
{
	if (e.Button == GLFW_MOUSE_BUTTON_1)
	{
		m_MousePressed = false;
	}

	return false;
}

bool SimLayer::OnMouseScrolled(MouseScrolledEvent &e)
{
	m_CamScale += 0.3f * m_CamScale * static_cast<float>(e.Yoffset);

	if (m_CamScale < 0.01f)
	{
		m_CamScale = 0.01f;
	}
	else if (m_CamScale > 2.0f)
	{
		m_CamScale = 2.0f;
	}

	return false;
}

bool SimLayer::OnKeyPressed(KeyPressedEvent &e)
{
	static constexpr float kEpsilon = 0.01f;

	switch (e.Key)
	{
	case GLFW_KEY_ESCAPE:
		BL_LOG("Closing application.");
		Application::Get().Close();
		break;
	case GLFW_KEY_F:
		m_FollowCam = !m_FollowCam;
		BL_LOG("Follow car %s", m_FollowCam ? "enabled" : "disabled");
		break;
	case GLFW_KEY_E:
		if (std::fabs(m_TimeScale - 10.0f) > kEpsilon)
		{
			m_TimeScale += 0.2f;
			BL_LOG("Timescale increased to %.2f", m_TimeScale);
		}
		break;
	case GLFW_KEY_Q:
		if (std::fabs(m_TimeScale - 0.2f) > kEpsilon)
		{
			m_TimeScale -= 0.2f;
			BL_LOG("Timescale decreased to %.2f", m_TimeScale);
		}
		break;
	case GLFW_KEY_SPACE:
		m_Paused = !m_Paused;
		BL_LOG("Pause %s", m_Paused ? "enabled" : "disabled");
		break;
	}

	return false;
}

SimLayer::SimLayer()
	: Layer("Simulation Layer")
	, m_CamPosition(0, 0, 0)
	, m_CamScale(0.5f)
	, m_TimeScale(1.0f)
	, m_MousePressed(false)
	, m_FollowCam(false)
	, m_Paused(false)
{
	m_Generation.Create(100);
}

void SimLayer::OnUpdate(float delta)
{
	if (!m_Paused)
	{
		m_Generation.Update(delta * m_TimeScale);
	}

	if (m_FollowCam)
	{
		b2Vec2 position = m_Generation.GetBestCar().GetPosition();
		m_CamPosition = {position.x, position.y, 0.0f};
	}

	const Window &window = Application::Get().GetWindow();
	float width = static_cast<float>(window.GetWidth());
	float height = static_cast<float>(window.GetHeight());
	float aspect = width / height;

	auto viewProj = glm::ortho(-10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f)
					* glm::scale(glm::mat4(1.0f), glm::vec3(m_CamScale))
					* glm::translate(glm::mat4(1.0f), -m_CamPosition);

	Renderer::BeginScene(viewProj);
	m_Generation.Draw();
	Renderer::EndScene();
}

void SimLayer::OnDrawImGui()
{
	m_Generation.DrawImGui();
}

void SimLayer::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseMovedEvent>(std::bind(&SimLayer::OnMouseMoved, this, std::placeholders::_1));
	dispatcher.dispatch<MouseScrolledEvent>(std::bind(&SimLayer::OnMouseScrolled, this, std::placeholders::_1));
	dispatcher.dispatch<MousePressedEvent>(std::bind(&SimLayer::OnMousePressed, this, std::placeholders::_1));
	dispatcher.dispatch<MouseReleasedEvent>(std::bind(&SimLayer::OnMouseReleased, this, std::placeholders::_1));
	dispatcher.dispatch<KeyPressedEvent>(std::bind(&SimLayer::OnKeyPressed, this, std::placeholders::_1));
}
