#pragma once

#include <iostream>
#include <vector>

#include "Car.h";
#include "Platform.h"
#include "Renderer.h"

#include "box2d/box2d.h"

class Generation
{

private:	

	int generationCount_;
	int carCount_;

	b2World* world_;

	std::vector<Car*> cars_;
	Platform* platform_;

private:
	void nextGeneration();

public:
	Generation(int carCount);
	~Generation();

	glm::vec3 getPositionOfBestCar();

	void update(float delta);
	void draw(Renderer& renderer);
};

