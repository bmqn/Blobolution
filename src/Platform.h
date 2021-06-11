#pragma once

#include "box2d/box2d.h"
#include "Renderer.h"

#include <vector>

class Platform
{

public:
	~Platform();
	Platform(b2World& world, int platformCount);

	void draw(Renderer& renderer);

private:
	b2Body* platform_;
};

