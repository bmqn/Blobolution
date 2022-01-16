#pragma once

#include "Renderer.h"

#include <memory>

#include <box2d/box2d.h>

class Platform
{

public:
	~Platform();

	void init(b2World &world, int platformCount);
	void draw(Renderer &renderer);

private:
	int m_PlatformCount;

	b2Body *m_Body;
};
