#pragma once

#include "Renderer.h"

#include <box2d/box2d.h>

class Platform
{
public:
	Platform();

	void Create(b2World &world, int platformCount);
	void Destory();

	void Draw();

private:
	b2Body *m_PlatformBody;
	int m_PlatformCount;
};
