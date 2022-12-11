#include "Car.h"
#include "Random.h"
#include "Log.h"

static uint32_t GetNewCarId()
{
	static uint32_t sCarId = 0;
	sCarId += Random::Int(0u, std::numeric_limits<uint32_t>::max() - 1);
	return sCarId;
}

Car::Car()
	: m_CarId(GetNewCarId())
	, m_Health(0)
	, m_ChassisBody(nullptr)
{
}

void Car::Create(b2World& world, const CarProto& carProto)
{
	BL_ASSERT(!m_ChassisBody, "The car has already been created !");

	if (!m_ChassisBody)
	{
		m_Proto = carProto;
		m_Health = 180;
		m_ChassisBody = nullptr;
		m_WheelJoints.clear();
		m_WheelBodies.clear();

		b2Vec2 vertices[CarConstants::kNumVertices] = { b2Vec2_zero };

		for (int i = 0; i < CarConstants::kNumVertices; i++)
		{
			vertices[i] = m_Proto.Vertices[i];
		}

		b2PolygonShape chassisShape;
		chassisShape.Set(vertices, CarConstants::kNumVertices);

		b2BodyDef chassisDef;
		chassisDef.type = b2_dynamicBody;
		chassisDef.position.Set(0, 0);
		chassisDef.angle = 0;
		chassisDef.userData.pointer = (uintptr_t)&m_Proto.Colour;

		b2FixtureDef chassisFixture;
		chassisFixture.shape = &chassisShape;
		chassisFixture.density = m_Proto.Density;
		chassisFixture.restitution = m_Proto.Restitution;
		chassisFixture.filter.groupIndex = -1;

		m_ChassisBody = world.CreateBody(&chassisDef);
		m_ChassisBody->CreateFixture(&chassisFixture);

		for (const WheelProto &wheel : m_Proto.Wheels)
		{
			b2CircleShape wheelShape;
			wheelShape.m_radius = wheel.Radius;

			b2BodyDef wheelDef;
			wheelDef.type = b2_dynamicBody;
			wheelDef.angle = 0;
			wheelDef.userData.pointer = (uintptr_t)&wheel.Colour;

			b2FixtureDef wheelFixture;
			wheelFixture.shape = &wheelShape;
			wheelFixture.density = wheel.Density;
			wheelFixture.restitution = wheel.Restitution;
			wheelFixture.filter.groupIndex = -1;

			b2Body* wheelBody = world.CreateBody(&wheelDef);
			wheelBody->CreateFixture(&wheelFixture);

			m_WheelBodies.push_back(wheelBody);

			b2RevoluteJointDef jointDef;
			jointDef.collideConnected = false;
			jointDef.enableMotor = true;

			jointDef.bodyA = m_ChassisBody;
			jointDef.bodyB = wheelBody;
			jointDef.maxMotorTorque = m_ChassisBody->GetMass() / wheel.Radius * 100.0f;
			jointDef.motorSpeed = wheel.MotorSpeed;
			jointDef.localAnchorA = vertices[wheel.Vertex];
			jointDef.localAnchorB.Set(0, 0);

			m_WheelJoints.push_back(world.CreateJoint(&jointDef));
		}
	}
}

void Car::Destory()
{
	BL_ASSERT(m_ChassisBody, "The car has not been created !");

	if (m_ChassisBody)
	{
		b2World *world = m_ChassisBody->GetWorld();

		for (b2Joint *wJoint : m_WheelJoints)
		{
			world->DestroyJoint(wJoint);
		}

		for (b2Body *wBody : m_WheelBodies)
		{
			world->DestroyBody(wBody);
		}

		world->DestroyBody(m_ChassisBody);

		m_ChassisBody = nullptr;
		m_WheelJoints.clear();
		m_WheelBodies.clear();
	}
}

void Car::Draw() const
{
	if (m_ChassisBody)
	{
		// Wheels
		for (const b2Body *wheelBody : m_WheelBodies)
		{
			b2Vec2 position = wheelBody->GetPosition();
			float  rotation = wheelBody->GetAngle();

			for (const b2Fixture *f = wheelBody->GetFixtureList(); f;
			     f = f->GetNext())
			{
				const b2PolygonShape *shape = static_cast<const b2PolygonShape*>(f->GetShape());

				float wheelRadius = shape->m_radius;

				glm::vec4 *colour = reinterpret_cast<glm::vec4*>(
					const_cast<b2Body*>(wheelBody)->GetUserData().pointer
				);

				glm::vec4 wheelColour;
				if (colour)
				{
					wheelColour = *colour;
				}
				else
				{
					wheelColour = { 0.8f, 0.4f, 0.4f, 1.0f };
				}

				glm::vec4 spokeColour = wheelColour * 0.85f;

				float wheelZOffset = 0.0f;
				float spokeZOffset = 0.1f;

				if (IsDead())
				{
					wheelColour *= 0.5f;
					spokeColour *= 0.5f;
					wheelZOffset -= 0.05f;
					spokeZOffset -= 0.05f;
				}

				// Wheel
				Renderer::SubmitFilledCircle(
					{ position.x, position.y, wheelZOffset }, wheelRadius, wheelColour
				);

				// Spokes
				{
					std::vector<glm::vec3> vertices = {
						{ position.x, position.y, spokeZOffset },
						{ wheelRadius * cos(0.00f + rotation) + position.x,
						  wheelRadius * sin(0.00f + rotation) + position.y,
						  spokeZOffset },
						{ 0.5f * wheelRadius * cos(0.79f + rotation) + position.x,
						  0.5f * wheelRadius * sin(0.79f + rotation) + position.y,
						  spokeZOffset },

						{ position.x, position.y, spokeZOffset },
						{ wheelRadius * cos(1.57f + rotation) + position.x,
						  wheelRadius * sin(1.57f + rotation) + position.y,
						  spokeZOffset },
						{ 0.5f * wheelRadius * cos(2.36f + rotation) + position.x,
						  0.5f * wheelRadius * sin(2.36f + rotation) + position.y,
						  spokeZOffset },

						{ position.x, position.y, spokeZOffset },
						{ wheelRadius * cos(3.14f + rotation) + position.x,
						  wheelRadius * sin(3.14f + rotation) + position.y,
						  spokeZOffset },
						{ 0.5f * wheelRadius * cos(3.93f + rotation) + position.x,
						  0.5f * wheelRadius * sin(3.93f + rotation) + position.y,
						  spokeZOffset },

						{ position.x, position.y, spokeZOffset },
						{ wheelRadius * cos(4.71f + rotation) + position.x,
						  wheelRadius * sin(4.71f + rotation) + position.y,
						  spokeZOffset },
						{ 0.5f * wheelRadius * cos(5.50f + rotation) + position.x,
						  0.5f * wheelRadius * sin(5.50f + rotation) + position.y,
						  spokeZOffset }
					};
					Renderer::SubmitFilledPolygon(vertices, spokeColour);
				}
			}
		}

		// Chassis
		{
			b2Vec2 position = m_ChassisBody->GetPosition();
			float  rotation = m_ChassisBody->GetAngle();

			for (const b2Fixture *f = m_ChassisBody->GetFixtureList(); f;
				f = f->GetNext())
			{
				const b2PolygonShape *shape = static_cast<const b2PolygonShape*>(f->GetShape());

				const b2Vec2 *vertexArray = shape->m_vertices;
				int32_t vertexCount = shape->m_count;

				glm::vec4 *colour = reinterpret_cast<glm::vec4*>(
					m_ChassisBody->GetUserData().pointer
				);

				glm::vec4 bodyColour;
				if (colour)
				{
					bodyColour = *colour;
				}
				else
				{
					bodyColour = { 0.8f, 0.4f, 0.4f, 1.0f };
				}
				
				float zOffset = 0.2f;

				if (IsDead())
				{
					bodyColour *= 0.5f;
					zOffset -= 0.05f;
				}

				std::vector<glm::vec3> vertices;
				for (int i = 0; i < vertexCount; ++i)
				{
					float x =  (vertexArray[i].x * std::cos(rotation)
					          - vertexArray[i].y * std::sin(rotation)) + position.x;
					float y =  (vertexArray[i].x * std::sin(rotation)
					          + vertexArray[i].y * std::cos(rotation)) + position.y;

					vertices.push_back({ x, y, zOffset });
				}
				Renderer::SubmitFilledPolygon(vertices, bodyColour);
			}
		}
	}
}

void Car::Update(float delta)
{
	if (m_ChassisBody)
	{
		if (!IsDead())
		{
			int ticker = static_cast<int>(100.0f * delta);
			ticker = ticker <= 0 ? 1 : ticker;
			if (m_ChassisBody->GetLinearVelocity().x <= CarConstants::kMinSpeed)
			{
				m_Health -= ticker;
			}
		}
		else
		{
			for (b2Joint *wheelJoint : m_WheelJoints)
			{
				if (wheelJoint->GetType() == b2JointType::e_revoluteJoint)
				{
					b2RevoluteJoint *revJoint = static_cast<b2RevoluteJoint*>(wheelJoint);
					revJoint->SetMotorSpeed(0.0f);
				}
			}
		}
	}
}

CarProto Car::RandomProto()
{
	CarProto carProto;

	for (int i = 0; i < CarConstants::kNumVertices; ++i)
	{
		float angle =   3.14159f / 180.0f * 360.0f
		              * static_cast<float>(i)
					  / static_cast<float>(CarConstants::kNumVertices);

		carProto.Vertices[i] = { Random::Float(8.0f, 15.0f) * static_cast<float>(cosf(angle)),
								 Random::Float(8.0f, 15.0f) * static_cast<float>(sinf(angle)) };
	}
	carProto.Density =  (  CarConstants::kMaxChassisDensity
	                     - CarConstants::kMinChassisDensity) * Random::Float(0.0f, 1.0f)
	                  + CarConstants::kMinChassisDensity;
	carProto.Friction = Random::Float(0.0f, 1.0f);
	carProto.Restitution = Random::Float(0.0f, 1.0f);
	{
		float r =  (carProto.Density - CarConstants::kMinChassisDensity)
		         / (CarConstants::kMaxChassisDensity - CarConstants::kMinChassisDensity);
		float g = carProto.Friction;
		float b = carProto.Restitution;
		carProto.Colour = { r, g, b, 1.0f };
	}

	int wheelCount = Random::Int(1, 5);
	for (int i = 0; i < wheelCount; i++)
	{
		WheelProto wheelProto;
		wheelProto.Density =  Random::Float(CarConstants::kMinWheelDensity, CarConstants::kMaxWheelDensity);
		wheelProto.Friction = Random::Float(0.0f, 1.0f);
		wheelProto.Restitution = Random::Float(0.0f, 1.0f);
		wheelProto.Radius = Random::Float(CarConstants::kMinWheelRadius, CarConstants::kMaxWheelRadius);
		wheelProto.MotorSpeed = -Random::Float(CarConstants::kMinWheelMotorSpeed, CarConstants::kMaxWheelMotorSpeed);
		wheelProto.Vertex = CarConstants::kNumVertices - 1 - i;
		{
			float r =  (wheelProto.Density - CarConstants::kMinChassisDensity)
			         / (CarConstants::kMaxChassisDensity - CarConstants::kMinChassisDensity);
			float g = wheelProto.Friction;
			float b = wheelProto.Restitution;
			wheelProto.Colour = { r, g, b, 1.0f };
		}
		carProto.Wheels.push_back(wheelProto);
	}

	return carProto;
}
