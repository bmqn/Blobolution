#pragma once

#include "Layer.h"

#include "Generation.h"

class SimLayer : public Layer
{

private:
	Renderer renderer_;
	Generation gen_;

	glm::vec3 camPosition_;
	float camScale_;
	float timeScale_;

	double xpos_, ypos_;
	bool mousePressed_, followCam_;

private:

	bool onMouseMoved(MouseMovedEvent& e);
	bool onMousePressed(MousePressedEvent& e);
	bool onMouseScrolled(MouseScrolledEvent& e);

public:

	SimLayer();
	~SimLayer();

	virtual void onUpdate(float delta) override;

	virtual void onEvent(Event& e) override;
};

