#pragma once

#include "Renderer.h"

#include <box2d/box2d.h>

#include <vector>
#include <memory>

const float MIN_WHEEL_RADIUS = 0.1f;
const float MAX_WHEEL_RADIUS = 5.0f; 

const float MIN_WHEEL_MOTOR_SPEED = 20.0f;
const float MAX_WHEEL_MOTOR_SPEED = 50.0f;

struct WheelData
{
	float Density = 1.f;
	float Friction = 1.f;
	float Restitution = 1.f;
	float Radius = 1.f;
	float MotorSpeed = -1.f;
	int Vertex = 0;
};

struct CarData
{
	float Density = 1.f;
	float Friction = 1.f;
	float Restitution = 1.f;
	std::vector<b2Vec2> Vertices = {};
	std::vector<WheelData> Wheels = {};
};

class Car
{
private:
	b2Body *m_Body;
	std::vector<b2Body *> m_Wheels;
	std::vector<b2Joint *> m_Joints;

	int m_Health;
	int m_Life;
	
	CarData m_CarData;

public:
	Car();
	~Car();
	Car(const Car&) = delete;
	Car(Car&& car);
	Car& operator=(Car&& car);

	const b2Vec2 &getPosition() const { return m_Body->GetPosition(); }

	bool isDead() const { return m_Health <= 0; }
	int getFitness() const { return m_Life + (int)m_Body->GetPosition().x; }

	CarData getCarData() const { return m_CarData; }

	void init(b2World &world, CarData carData);

	void draw(Renderer &renderer) const;
	void update(float delta);

public:
	static CarData randomCarData();
};
