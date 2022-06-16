#pragma once

#include "Renderer.h"
#include "Car.h"
#include "Platform.h"

#include <glm/glm.hpp>
#include <box2d/box2d.h>

class Generation
{
private:
	std::unique_ptr<b2World> m_World;

	std::unique_ptr<Platform> m_Platform;
	std::vector<Car> m_Cars;

public:
	Generation();
	~Generation();

	void Create(int numCars);
	void Update(float delta);
	void Draw() const;
	void DrawImGui() const;

	const Car &GetBestCar() const;

private:
	void NextGeneration();
};
