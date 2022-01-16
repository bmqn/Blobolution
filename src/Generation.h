#pragma once

#include "Renderer.h"
#include "Car.h"
#include "Platform.h"

#include <glm/glm.hpp>
#include <box2d/box2d.h>

#include <vector>
#include <memory>


class Generation
{

private:
	using WorldPtr		= std::unique_ptr<b2World>;
	using PlatformPtr 	= std::unique_ptr<Platform>;

	WorldPtr m_World;
	PlatformPtr m_Platform;
	std::vector<Car> m_Cars;

	bool m_Simulating;

private:
	void nextGeneration();

public:
	Generation(int cars);
	~Generation();

	void init();
	void beginSimulating();

	glm::vec3 getPositionOfBestCar();

	void update(float delta);
	void draw(Renderer &renderer);
};
