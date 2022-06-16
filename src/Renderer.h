#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
private:
	static void InitCircleRenderer();
	static void CleanupCircleRenderer();
	static void FlushCircleVertices();
	static void MapCircleBuffer();
	static void UnmapCircleBuffer();

	static void InitTriangleRenderer();
	static void CleanupTriangleRenderer();
	static void FlushTriangleVertices();
	static void MapTriangleBuffer();
	static void UnmapTriangleBuffer();

	static void InitLineRenderer();
	static void CleanupLineRenderer();
	static void FlushLineVertices();
	static void MapLineBuffer();
	static void UnmapLineBuffer();

	static void FlushScene();

public:
	static void Create();
	static void Destroy();

	static void Clear();
	static void SetViewportSize(int width, int height);

	static void BeginScene(const glm::mat4 &viewProj);
	static void EndScene();

	static void SubmitFilledCircle(const glm::vec3& position, float radius, const glm::vec4& colour);
	static void SubmitFilledPolygon(const std::vector<glm::vec3>& vertices, const glm::vec4& colour);

	static void SubmitLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& colour);
};
