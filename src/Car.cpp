#include "Car.h"

#include "Random.h"
#include "Assert.h"

#include <iostream>
#include <cmath>

Car::Car()
    : m_Body(nullptr),
      m_Health(180),
      m_Life(0)
{
}

Car::~Car()
{
    if (!m_Body)
        return;

    b2World *world = m_Body->GetWorld();

    for (b2Joint *j : m_Joints)
        world->DestroyJoint(j);

    for (b2Body *w : m_Wheels)
        world->DestroyBody(w);

    world->DestroyBody(m_Body);
}

Car::Car(Car &&car)
{
    m_Health = car.m_Health;
    m_Life = car.m_Life;
    m_CarData = car.m_CarData;

    m_Body = car.m_Body;
    m_Wheels = car.m_Wheels;
    m_Joints = car.m_Joints;

    car.m_Health = 0;
    car.m_Life = 0;
    car.m_CarData = {};
    car.m_Body = nullptr;
    car.m_Wheels.clear();
    car.m_Joints.clear();
}

Car &Car::operator=(Car &&car)
{
    m_Health = car.m_Health;
    m_Life = car.m_Life;
    m_CarData = car.m_CarData;

    if (m_Body && m_Body != car.m_Body)
    {
        b2World *world = m_Body->GetWorld();

        for (b2Joint *j : m_Joints)
            world->DestroyJoint(j);

        for (b2Body *w : m_Wheels)
            world->DestroyBody(w);

        world->DestroyBody(m_Body);
    }

    m_Body = car.m_Body;
    m_Wheels = car.m_Wheels;
    m_Joints = car.m_Joints;

    car.m_Health = 0;
    car.m_Life = 0;
    car.m_CarData = {};
    car.m_Body = nullptr;
    car.m_Wheels.clear();
    car.m_Joints.clear();

    return *this;
}

void Car::init(b2World &world, CarData carData)
{
    m_Health = 180;
    m_Life = 0;
    m_CarData = carData;

    if (m_Body)
    {
        b2World *wrld = m_Body->GetWorld();

        for (b2Joint *j : m_Joints)
            wrld->DestroyJoint(j);

        for (b2Body *w : m_Wheels)
            wrld->DestroyBody(w);

        wrld->DestroyBody(m_Body);

        m_Joints.clear();
        m_Wheels.clear();
        m_Body = nullptr;
    }

    b2Vec2 vertices[8];

    for (int i = 0; i < 8; i++)
    {
        vertices[i] = carData.Vertices[i];
    }

    b2PolygonShape chassisShape;
    chassisShape.Set(vertices, 8);

    b2FixtureDef chassisFixture;
    chassisFixture.shape = &chassisShape;
    chassisFixture.density = carData.Density;
    chassisFixture.restitution = carData.Restitution;
    chassisFixture.filter.groupIndex = -1;

    b2BodyDef chassisDef;
    chassisDef.type = b2_dynamicBody;
    chassisDef.position.Set(0, 0);
    chassisDef.angle = 0;

    m_Body = world.CreateBody(&chassisDef);
    m_Body->CreateFixture(&chassisFixture);

    b2CircleShape wheelShape;

    b2BodyDef wheelDef;
    wheelDef.type = b2_dynamicBody;
    wheelDef.angle = 0;

    b2RevoluteJointDef jointDef;
    jointDef.collideConnected = false;
    jointDef.enableMotor = true;

    for (WheelData wheel : carData.Wheels)
    {
        wheelShape.m_radius = wheel.Radius;

        b2FixtureDef wheel_fixture;
        wheel_fixture.shape = &wheelShape;
        wheel_fixture.density = wheel.Density;
        wheel_fixture.restitution = wheel.Restitution;
        wheel_fixture.filter.groupIndex = -1;

        b2Body *wheelBody = world.CreateBody(&wheelDef);
        wheelBody->CreateFixture(&wheel_fixture);

        m_Wheels.push_back(wheelBody);

        jointDef.bodyA = m_Body;
        jointDef.bodyB = wheelBody;
        jointDef.maxMotorTorque = m_Body->GetMass() / wheel.Radius * 100.0f;
        jointDef.motorSpeed = wheel.MotorSpeed;
        jointDef.localAnchorA = vertices[wheel.Vertex];
        jointDef.localAnchorB.Set(0, 0);

        m_Joints.push_back(world.CreateJoint(&jointDef));
    }
}

void Car::draw(Renderer &renderer) const
{
    if (!m_Body)
        return;

    {
        b2Vec2 pos = m_Body->GetPosition();
        float rot = m_Body->GetAngle();

        for (b2Fixture *f = m_Body->GetFixtureList(); f; f = f->GetNext())
        {
            b2PolygonShape *shape = (b2PolygonShape *)f->GetShape();

            if (!shape)
                continue;

            auto vertexArray = shape->m_vertices;
            auto vertexCount = shape->m_count;

            std::vector<glm::vec2> vertices;
            glm::vec4 colour{0.8f, 0.2, 0.2f, 1.0f};

            if (isDead())
                colour *= 0.5f;

            for (int i = 0; i < vertexCount; i++)
            {
                float x = (vertexArray[i].x * std::cos(rot) - vertexArray[i].y * std::sin(rot)) + pos.x;
                float y = (vertexArray[i].x * std::sin(rot) + vertexArray[i].y * std::cos(rot)) + pos.y;

                vertices.push_back({x, y});
            }

            renderer.submitFilledPolygon(vertices, colour);
        }
    }

    for (auto &wheel : m_Wheels)
    {
        if (!wheel)
            continue;

        b2Vec2 pos = wheel->GetPosition();
        float rot = wheel->GetAngle();

        for (b2Fixture *f = wheel->GetFixtureList(); f; f = f->GetNext())
        {
            b2PolygonShape *shape = (b2PolygonShape *)f->GetShape();

            if (!shape)
                continue;

            auto radius = shape->m_radius;

            std::vector<glm::vec2> vertices;
            glm::vec4 colour{0.8f, 0.2, 0.2f, 1.0f};

            if (isDead())
                colour *= 0.5f;

            constexpr float PI = 3.14159f;

            for (float a = 0.0; a <= 2.0f * PI; a += 2.0f * PI / 8.0f)
            {
                float x = radius * cos(a + rot) + pos.x;
                float y = radius * sin(a + rot) + pos.y;

                vertices.push_back({x, y});
            }

            renderer.submitFilledPolygon(vertices, colour);
        }
    }
}

void Car::update(float delta)
{
    if (!m_Body)
        return;

    if (!isDead())
    {
        m_Life++;

        if (m_Body->GetLinearVelocity().x <= 0.5f)
            m_Health--;
    }
}

CarData Car::randomCarData()
{
    CarData carData;

    for (int i = 0; i < 8; i++)
    {
        double angle = 3.14159f / 180.0f * 360.0f * (float)i / 8.0f;

        carData.Vertices.push_back({(Random::Float() * (15.f - 8.f) + 8.f) * (float)cos(angle),
                                    (Random::Float() * (15.f - 8.f) + 8.f) * (float)sin(angle)});
    }

    carData.Density = Random::Float() * (25.f - 10.f) + 10.f;
    carData.Friction = Random::Float();
    carData.Restitution = Random::Float();

    int wheelCount = Random::UInt(1, 5);

    for (int i = 0; i < wheelCount; i++)
    {
        WheelData wheelData;

        wheelData.Density = Random::Float() * (5.f - 1.f) + 1.f;
        wheelData.Friction = Random::Float();
        wheelData.Restitution = Random::Float();
        wheelData.Radius = Random::Float() * (MAX_WHEEL_RADIUS - MIN_WHEEL_RADIUS) + MIN_WHEEL_RADIUS;
        wheelData.MotorSpeed = -Random::Float() * (MAX_WHEEL_MOTOR_SPEED - MIN_WHEEL_MOTOR_SPEED) + MIN_WHEEL_MOTOR_SPEED;
        wheelData.Vertex = 7 - i;

        carData.Wheels.push_back(wheelData);
    }

    return carData;
}
