#pragma once

#include <box2d/box2d.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"


class Draw : public b2Draw
{
private:

	Renderer renderer_;

	glm::mat4 viewProj_;

public:

	Draw() : viewProj_()
	{
		auto proj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f);
		auto view = glm::mat4(1.f);

		viewProj_ = proj * view;
	}

	void begin() { renderer_.clear(); renderer_.beginScene(viewProj_); }
	void end() { renderer_.endScene(); }

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

	void DrawTransform(const b2Transform& xf) override;

	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

};
