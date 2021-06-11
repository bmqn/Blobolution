#include "Car.h"

#include <random>
#include <math.h>


Car::~Car()
{
    b2World* world = body_->GetWorld();
    
    for (b2Joint* j : joints_)
        world->DestroyJoint(j);
    
    for (b2Body* w : wheels_)
        world->DestroyBody(w);
    
    world->DestroyBody(body_);
}

void Car::draw(Renderer& renderer)
{
    b2Vec2 pos = body_->GetPosition();
    float rot = body_->GetAngle();
    
    auto vertexArray = ((b2PolygonShape*)body_->GetFixtureList()->GetShape())->m_vertices;
    auto vertexCount = ((b2PolygonShape*)body_->GetFixtureList()->GetShape())->m_count;

    std::vector<glm::vec2> vertices;
    glm::vec4 colour = { 0.8f, 0.2, 0.2f, 1.0f };

    for (int i = 0; i < vertexCount; i++)
    {
        float x = (vertexArray[i].x * std::cos(rot) - vertexArray[i].y * std::sin(rot)) + pos.x;
        float y = (vertexArray[i].x * std::sin(rot) + vertexArray[i].y * std::cos(rot)) + pos.y;

        vertices.push_back({ x, y });
    }

    renderer.submitFilledPolygon(vertices, colour);

    for (auto& wheel : wheels_)
    {
        b2Vec2 pos = wheel->GetPosition();
        float rot = wheel->GetAngle();

        auto radius = ((b2CircleShape*)wheel->GetFixtureList()->GetShape())->m_radius;

        std::vector<glm::vec2> vertices;
        glm::vec4 colour = { 0.8f, 0.2, 0.2f, 1.0f };

        constexpr float PI = 3.14159f;

        for (float a = 0.0; a <= 2.0f * PI; a += 2.0f * PI / 8.0f)
        {
            float x = radius * cos(a + rot) + pos.x;
            float y = radius * sin(a + rot) + pos.y;

            vertices.push_back({ x, y });
        }

        renderer.submitFilledPolygon(vertices, colour);
    }
}

Car::Car(b2World& world, CarData data) :
    body_(nullptr), health_(1000), fitness_(1), carData_(data)
{
    b2PolygonShape chassisShape;
    chassisShape.Set(carData_.vertices, 8);

    b2FixtureDef chassisFixture;
    chassisFixture.shape = &chassisShape;
    chassisFixture.density = carData_.chassisDensity;
    chassisFixture.restitution = carData_.chassisRestitution;
    chassisFixture.filter.groupIndex = -1;

    b2BodyDef chassisDef;
    chassisDef.type = b2_dynamicBody;
    chassisDef.position.Set(0, 0);
    chassisDef.angle = 0;

    body_ = world.CreateBody(&chassisDef);
    body_->CreateFixture(&chassisFixture);

    b2CircleShape wheelShape;

    b2BodyDef wheelDef;
    wheelDef.type = b2_dynamicBody;
    wheelDef.angle = 0;

    b2RevoluteJointDef jointDef;
    jointDef.collideConnected = false;
    jointDef.enableMotor = true;

    for (WheelData wheel : carData_.wheels)
    {
        wheelShape.m_radius = wheel.radius;

        b2FixtureDef wheel_fixture;
        wheel_fixture.shape = &wheelShape;
        wheel_fixture.density = wheel.wheelDensity;
        wheel_fixture.restitution = wheel.wheelRestitution;
        wheel_fixture.filter.groupIndex = -1;

        b2Body* wheelBody = world.CreateBody(&wheelDef);
        wheelBody->CreateFixture(&wheel_fixture);

        wheels_.push_back(wheelBody);

        jointDef.bodyA = body_;
        jointDef.bodyB = wheelBody;
        jointDef.maxMotorTorque = body_->GetMass() / wheel.radius * 100;
        jointDef.motorSpeed = wheel.motorSpeed;
        jointDef.localAnchorA = carData_.vertices[wheel.vertex];
        jointDef.localAnchorB.Set(0, 0);

        joints_.push_back(world.CreateJoint(&jointDef));
    }
}

CarData Car::randomCarData()
{
    CarData carData;

    for (int i = 0; i < 8; i++)
    {
        double angle = 3.14159 / 180.0 * 360.0 * i / 8;

        carData.vertices[i] = {
            (((float)(rand() % 1000) / 1000) * (5 - 3) + 3) * (float) cos(angle),
            (((float)(rand() % 1000) / 1000) * (5 - 3) + 3) * (float) sin(angle)
        };
    }

    carData.chassisDensity = 50.0f * (float)(rand() % 1000) / 1000.0f;
    carData.chassisFriction = 0.3f * (float)(rand() % 1000) / 1000.0f;
    carData.chassisRestitution = 0.3f * (float)(rand() % 1000) / 1000.0f;
    
    int wheelCount = rand() % 5 + 1;
    std::vector<WheelData> wheels;

    for (int i = 0; i < wheelCount; i++)
    {
        WheelData wheelData;

        wheelData.radius = ((float)(rand() % 1000) / 1000) * (2 - 1) + 1;
        wheelData.wheelDensity = 5.0f;
        wheelData.wheelFriction = 0.6f;
        wheelData.wheelRestitution = 0.3f;
        wheelData.motorSpeed = -10.0f;
        wheelData.vertex = 7 - i;

        wheels.push_back(wheelData);
    }

    carData.wheels = wheels;

    return carData;
}
