#pragma once

#include "Layer.h"
#include "Renderer.h"
#include "Generation.h"

#include <glm/glm.hpp>

class SimLayer : public Layer
{
public:
	SimLayer();

	virtual void OnUpdate(float delta) override;
	virtual void OnDrawImGui() override;
	virtual void OnEvent(Event &e) override;

private:
	bool OnMouseMoved(MouseMovedEvent &e);
	bool OnMousePressed(MousePressedEvent &e);
	bool OnMouseReleased(MouseReleasedEvent &e);
	bool OnMouseScrolled(MouseScrolledEvent &e);
	bool OnKeyPressed(KeyPressedEvent &e);

private:
	Generation m_Generation;

	glm::vec3 m_CamPosition;
	float m_CamScale, m_TimeScale;
	bool m_MousePressed, m_FollowCam, m_Paused;
};
