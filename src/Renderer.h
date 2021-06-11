#pragma once

#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

struct RendererStats
{
	int rendererSubmissions;
	int rendererDrawCalls;
	int rendererVertices;

	RendererStats() : rendererSubmissions(0), rendererDrawCalls(0), rendererVertices(0) {}
};

struct Vertex
{
	glm::vec2 position;
	glm::vec4 colour;
};

struct BatchRendererData
{
	int verticesCount;
	GLuint program;
	GLuint vao, vbo;
	Vertex* batchDataPtr;

	BatchRendererData() : verticesCount(0), program(0), vao(0), vbo(0), batchDataPtr(nullptr) {}
};

class Renderer
{
public:
	Renderer();
	~Renderer();

private:
	// Renderer debug variables
	RendererStats rendererStats_;

	// Triangle Renderer Data
	BatchRendererData triangleRendererData_;

	// Line Renderer Data
	BatchRendererData lineRendererData_;

	glm::mat4 viewProj_;

private:
	void initTriangleRenderer();
	void cleanupTriangleRenderer();
	void flushTriangleVertices();
	void mapTriangleBuffer();

	void initLineRenderer();
	void cleanupLineRenderer();
	void flushLineVertices();
	void mapLineBuffer();

public:
	static void clear();

	void beginScene(const glm::mat4& viewProj);
	void endScene();
	void flushScene();

	void submitFilledPolygon(const std::vector<glm::vec2>& vertices, const glm::vec4& colour);

	RendererStats getRendererStats();
};

