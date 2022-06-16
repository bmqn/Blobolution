#pragma once

#include "Renderer.h"

#include <box2d/box2d.h>

class Platform
{

private:
	

public:
	Platform();

	void Create(b2World &world, int platformCount);
	void Destory();

	void Draw();

private:
	bool    m_IsCreated;
	b2Body* m_Body;
	int     m_PlatformCount;
};
