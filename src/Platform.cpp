#include "Platform.h"

#include <random>
#include <math.h>
#include <iostream>

Platform::~Platform()
{
    m_Body->GetWorld()->DestroyBody(m_Body);
}

void Platform::init(b2World &world, int platformCount)
{
    if (m_Body)
        return;

    m_PlatformCount = platformCount;

    b2PolygonShape shape;

    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = 1.0f;
    fixture.friction = 1.0f;
    fixture.restitution = 0.1f;

    b2BodyDef def;
    def.type = b2_staticBody;
    def.position = {-50.0f, -20.0f};

    m_Body = world.CreateBody(&def);

    float angle = 0.0f, prevAngle = 0.0f, x = 0.0f, y = 0.0f;

    for (int i = 0; i < m_PlatformCount; i++)
    {
        angle = 0.5f * ((rand() % 1000 / 1000.0f) - 0.5f) * pow(2, (float)i / (float)m_PlatformCount);

        x += 10.0f * (cos(prevAngle) + sin(3.14159f / 2.0f - angle));
        y += 10.0f * (cos(3.14159f / 2.0f - angle) + sin(prevAngle));

        b2Vec2 p1 = {-5.0f * cos(angle) - 1.0f * sin(angle), -5.0f * sin(angle) + 1.0f * cos(angle)};
        b2Vec2 p2 = {-5.0f * cos(angle) - -1.0f * sin(angle), -5.0f * sin(angle) + -1.0f * cos(angle)};
        b2Vec2 p3 = {5.0f * cos(angle) - -1.0f * sin(angle), 5.0f * sin(angle) + -1.0f * cos(angle)};
        b2Vec2 p4 = {5.0f * cos(angle) - 1.0f * sin(angle), 5.0f * sin(angle) + 1.0f * cos(angle)};

        b2Vec2 t = b2Vec2{x / 2.0f, y / 2.0f};

        b2Vec2 points[] = {p1 + t, p2 + t, p3 + t, p4 + t};

        shape.Set(points, 4);

        prevAngle = angle;

        m_Body->CreateFixture(&fixture);
    }
}

void Platform::draw(Renderer &renderer)
{
    if (!m_Body)
        return;

    b2Vec2 pos = m_Body->GetPosition();
    float rot = m_Body->GetAngle();

    for (b2Fixture *f = m_Body->GetFixtureList(); f; f = f->GetNext())
    {
        auto vertexArray = ((b2PolygonShape *)f->GetShape())->m_vertices;
        auto vertexCount = ((b2PolygonShape *)f->GetShape())->m_count;

        std::vector<glm::vec2> vertices;
        glm::vec4 colour = {0.8f, 0.2, 0.2f, 1.0f};

        for (int i = 0; i < vertexCount; i++)
        {
            float x = (vertexArray[i].x * std::cos(rot) - vertexArray[i].y * std::sin(rot)) + pos.x;
            float y = (vertexArray[i].x * std::sin(rot) + vertexArray[i].y * std::cos(rot)) + pos.y;

            vertices.push_back({x, y});
        }

        renderer.submitFilledPolygon(vertices, colour);
    }
}
