#pragma once

#include <vector>

#include "box2d/box2d.h"

#include "Renderer.h"

struct WheelData
{
	float radius;
	float wheelDensity;
	float wheelFriction;
	float wheelRestitution;
	float motorSpeed;
	int vertex;
};

struct CarData
{
	b2Vec2 vertices[8];
	float chassisDensity;
	float chassisFriction;
	float chassisRestitution;
	std::vector<WheelData> wheels;
};

class Car
{
private:

	b2Body* body_;
	std::vector<b2Body*> wheels_;
	std::vector<b2Joint*> joints_;
	
	int health_;
	int fitness_;

	CarData carData_;

public:
	Car(b2World& world, CarData data);
	~Car();

	const b2Vec2& getPosition() const { return body_->GetPosition(); }

	void draw(Renderer& renderer);
	
	static CarData randomCarData();
};

