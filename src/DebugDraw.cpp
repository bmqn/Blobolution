#include "DebugDraw.h"
#include <vector>

void Draw::DrawPolygon(const b2Vec2* vertexArray, int32 vertexCount, const b2Color& color)
{
	std::vector<glm::vec2> vertices;
	glm::vec4 colour = { color.r, color.g, color.b, color.a };

	for (int i = 0; i < vertexCount; i++)
		vertices.push_back({ vertexArray[i].x, vertexArray[i].y });

	renderer_.submitFilledPolygon(vertices, colour);

}

void Draw::DrawSolidPolygon(const b2Vec2* vertexArray, int32 vertexCount, const b2Color& color)
{
	std::vector<glm::vec2> vertices;
	glm::vec4 colour = { color.r, color.g, color.b, color.a };

	for (int i = 0; i < vertexCount; i++)
		vertices.push_back({ vertexArray[i].x, vertexArray[i].y });

	renderer_.submitFilledPolygon(vertices, colour);
}

void Draw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
}

void Draw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
}

void Draw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
}

void Draw::DrawTransform(const b2Transform& xf)
{
}

void Draw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
}
