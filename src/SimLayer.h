#pragma once

#include "Layer.h"
#include "Renderer.h"
#include "Generation.h"

#include <glm/glm.hpp>

class SimLayer : public Layer
{

private:
	Renderer m_Renderer;
	Generation m_Generation;

	glm::vec3 m_CamPosition;
	float m_CamScale, m_TimeScale;
	bool m_MousePressed, m_FollowCam;

private:
	bool onMouseMoved(MouseMovedEvent &e);
	bool onMousePressed(MousePressedEvent &e);
	bool onMouseScrolled(MouseScrolledEvent &e);
	bool onKeyPressed(KeyPressedEvent &e);

public:
	SimLayer();
	~SimLayer();

	virtual void onUpdate(float delta) override;

	virtual void onEvent(Event &e) override;
};
