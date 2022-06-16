#pragma once

#include "Renderer.h"

#include <box2d/box2d.h>

namespace CarConstants
{
	static constexpr size_t kNumVertices = 8;

	static constexpr float kMinSpeed = 1.0f;

	static constexpr float kMinChassisDensity = 0.5f;
	static constexpr float kMaxChassisDensity = 50.0f;

	static constexpr float kMinWheelRadius = 1.0f;
	static constexpr float kMaxWheelRadius = 5.0f; 
	
	static constexpr float kMinWheelMotorSpeed = 20.0f;
	static constexpr float kMaxWheelMotorSpeed = 50.0f;

	static constexpr float kMinWheelDensity = 0.5f;
	static constexpr float kMaxWheelDensity = 50.0f;
}

struct WheelProto
{
	float     Density     = 1.0f;
	float     Friction    = 1.0f;
	float     Restitution = 1.0f;
	float     Radius      = 1.0f;
	float     MotorSpeed  = -1.0f;
	size_t    Vertex      = 0;
	glm::vec4 Colour      = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct CarProto
{
	using VerticesArr = std::array<b2Vec2, CarConstants::kNumVertices>;
	using WheelsList  = std::vector<WheelProto>;

	float       Density     = 1.0f;
	float       Friction    = 1.0f;
	float       Restitution = 1.0f;
	glm::vec4   Colour      = { 1.0f, 1.0f, 1.0f, 1.0f };
	VerticesArr Vertices;
	WheelsList  Wheels;
};

class Car
{
private:
	uint32_t               m_CarId;

	CarProto               m_Proto;
	int                    m_Health;

	b2Body                 *m_ChassisBody;
	std::vector<b2Body*>   m_WheelBodies;
	std::vector<b2Joint*>  m_WheelJoints;

public:
	Car();
	~Car();

	inline uint32_t GetCarId() const { return m_CarId; }

	inline const CarProto &GetProto() const { return m_Proto; }
	
	inline const b2Vec2 &GetPosition() const { return m_ChassisBody ? m_ChassisBody->GetPosition() : b2Vec2_zero; }
	inline const b2Vec2 &GetVelocity() const { return m_ChassisBody ? m_ChassisBody->GetLinearVelocity() : b2Vec2_zero; }
	
	inline int GetHealth() const { return m_Health; }
	inline int GetFitness() const { return GetPosition().x <= 0.0f ? 0 : static_cast<int>(GetPosition().x); }
	
	inline bool IsDead() const { return m_Health <= 0; }

	void Create(b2World &world, const CarProto &carProto);
	void Destory();

	void Draw() const;
	
	void Update(float delta);

public:
	static CarProto RandomProto();
};
