#include "Platform.h"
#include "Random.h"
#include "Log.h"

Platform::Platform()
	: m_IsCreated(false)
	, m_Body(nullptr)
	, m_PlatformCount(0)
{
}

void Platform::Create(b2World& world, int platformCount)
{
	BL_ASSERT(!m_IsCreated, "The platform has already been created !");

	if (!m_IsCreated)
	{
		m_PlatformCount = platformCount;

		b2BodyDef def;
		def.type = b2_staticBody;
		def.position = { -50.0f, -20.0f };

		m_Body = world.CreateBody(&def);

		float angle = 0.0f, prevAngle = 0.0f, x = 0.0f, y = 0.0f;

		for (int i = 0; i < m_PlatformCount; i++)
		{
			angle = 1.05f * Random::Float(-0.5f, 0.5f) * powf(
				2.0f, static_cast<float>(i) / static_cast<float>(m_PlatformCount)
			);

			x += 10.0f * (cos(prevAngle) + sin(3.14159f / 2.0f - angle));
			y += 10.0f * (cos(3.14159f / 2.0f - angle) + sin(prevAngle));

			prevAngle = angle;

			b2Vec2 p1 = { -5.0f * cos(angle) - 1.0f * sin(angle) + x / 2.0f, -5.0f * sin(angle) + 1.0f * cos(angle) + y / 2.0f };
			b2Vec2 p2 = { -5.0f * cos(angle) + 1.0f * sin(angle) + x / 2.0f, -5.0f * sin(angle) - 1.0f * cos(angle) + y / 2.0f };
			b2Vec2 p3 = { 5.0f * cos(angle) + 1.0f * sin(angle) + x / 2.0f,  5.0f * sin(angle) - 1.0f * cos(angle) + y / 2.0f };
			b2Vec2 p4 = { 5.0f * cos(angle) - 1.0f * sin(angle) + x / 2.0f,  5.0f * sin(angle) + 1.0f * cos(angle) + y / 2.0f };

			b2Vec2 points[] = { p1, p2, p3, p4 };

			b2PolygonShape shape;
			shape.Set(points, 4);

			b2FixtureDef fixture;
			fixture.shape = &shape;
			fixture.density = 1.0f;
			fixture.friction = 1.0f;
			fixture.restitution = 0.1f;

			m_Body->CreateFixture(&fixture);
		}

		m_IsCreated = true;
	}
}

void Platform::Destory()
{
	BL_ASSERT(m_IsCreated, "The platform has not been created !");

	if (m_IsCreated)
	{
		m_Body->GetWorld()->DestroyBody(m_Body);

		m_IsCreated = false;
	}
}

void Platform::Draw()
{
	BL_ASSERT(m_IsCreated, "The platform has not been created !");

	if (m_IsCreated)
	{
		b2Vec2 pos = m_Body->GetPosition();
		float rot = m_Body->GetAngle();

		for (b2Fixture* f = m_Body->GetFixtureList(); f; f = f->GetNext())
		{
			auto vertexArray = ((b2PolygonShape*)f->GetShape())->m_vertices;
			auto vertexCount = ((b2PolygonShape*)f->GetShape())->m_count;

			std::vector<glm::vec3> vertices;
			glm::vec4 colour = { 0.8f, 0.2, 0.2f, 1.0f };

			for (int i = 0; i < vertexCount; i++)
			{
				float x = (vertexArray[i].x * std::cos(rot) - vertexArray[i].y * std::sin(rot)) + pos.x;
				float y = (vertexArray[i].x * std::sin(rot) + vertexArray[i].y * std::cos(rot)) + pos.y;

				vertices.push_back({ x, y, 0.0f });
			}

			Renderer::SubmitFilledPolygon(vertices, colour);
		}
	}
}
