#include "Renderer.h"

#include <glm/ext.hpp>

#include <iostream>

// TODO: Get capabilities
constexpr int MAX_VERTICES = 8 * 1024;

Renderer::Renderer()
{
	initTriangleRenderer();
	initLineRenderer();
}

Renderer::~Renderer()
{
	cleanupTriangleRenderer();
	cleanupLineRenderer();
}

void Renderer::initTriangleRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec2 a_Position;\n"
		"layout (location = 1) in vec4 a_Colour;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_Position, 0, 1);\n"
		"}\n";
	const char *fragmentShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) out vec4 o_Colour;\n"
		"in vec4 v_Colour;\n"
		"void main()\n"
		"{\n"
		"	o_Colour = v_Colour;\n"
		"}\n";

	int success;
	char infoLog[512];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Vertex shader failed to compile!\n"
				  << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader failed to compile!\n"
				  << infoLog << std::endl;
	}

	m_TriangleRendererData.program = glCreateProgram();
	glAttachShader(m_TriangleRendererData.program, vertexShader);
	glAttachShader(m_TriangleRendererData.program, fragmentShader);
	glLinkProgram(m_TriangleRendererData.program);

	glDetachShader(m_TriangleRendererData.program, vertexShader);
	glDetachShader(m_TriangleRendererData.program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &m_TriangleRendererData.vao);
	glGenBuffers(1, &m_TriangleRendererData.vbo);

	glBindVertexArray(m_TriangleRendererData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_TriangleRendererData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::mapTriangleBuffer()
{
	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_TriangleRendererData.vbo);
	m_TriangleRendererData.batchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::unmapTriangleBuffer()
{
	// Unmap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_TriangleRendererData.vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::cleanupTriangleRenderer()
{
	glDeleteProgram(m_TriangleRendererData.program);
	glDeleteBuffers(1, &m_TriangleRendererData.vbo);
	glDeleteVertexArrays(1, &m_TriangleRendererData.vao);
}

void Renderer::flushTriangleVertices()
{
	unmapTriangleBuffer();

	// Check that there are actual vertices to flush
	if (m_TriangleRendererData.verticesCount == 0)
		return;

	glUseProgram(m_TriangleRendererData.program);
	glBindVertexArray(m_TriangleRendererData.vao);
	glDrawArrays(GL_TRIANGLES, 0, m_TriangleRendererData.verticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	m_RendererStats.rendererVertices += m_TriangleRendererData.verticesCount;
	m_RendererStats.rendererDrawCalls += 1;

	m_TriangleRendererData.verticesCount = 0;
}

void Renderer::initLineRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec2 a_Position;\n"
		"layout (location = 1) in vec4 a_Colour;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_Position, 0, 1);\n"
		"}\n";
	const char *fragmentShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) out vec4 o_Colour;\n"
		"in vec4 v_Colour;\n"
		"void main()\n"
		"{\n"
		"	o_Colour = v_Colour;\n"
		"}\n";

	int success;
	char infoLog[512];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Vertex shader failed to compile!\n"
				  << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader failed to compile!\n"
				  << infoLog << std::endl;
	}

	m_LineRendererData.program = glCreateProgram();
	glAttachShader(m_LineRendererData.program, vertexShader);
	glAttachShader(m_LineRendererData.program, fragmentShader);
	glLinkProgram(m_LineRendererData.program);

	glDetachShader(m_LineRendererData.program, vertexShader);
	glDetachShader(m_LineRendererData.program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &m_LineRendererData.vao);
	glGenBuffers(1, &m_LineRendererData.vbo);

	glBindVertexArray(m_LineRendererData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_LineRendererData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::mapLineBuffer()
{
	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_LineRendererData.vbo);
	m_LineRendererData.batchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::unmapLineBuffer()
{
	// Unmap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_LineRendererData.vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::cleanupLineRenderer()
{
	glDeleteProgram(m_LineRendererData.program);
	glDeleteBuffers(1, &m_LineRendererData.vbo);
	glDeleteVertexArrays(1, &m_LineRendererData.vao);
}

void Renderer::flushLineVertices()
{
	unmapLineBuffer();

	// Check that there are actual vertices to flush
	if (m_LineRendererData.verticesCount == 0)
		return;

	glUseProgram(m_LineRendererData.program);
	glBindVertexArray(m_LineRendererData.vao);
	glDrawArrays(GL_LINES, 0, m_LineRendererData.verticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	m_RendererStats.rendererVertices += m_LineRendererData.verticesCount;
	m_RendererStats.rendererDrawCalls += 1;

	m_LineRendererData.verticesCount = 0;
}

void Renderer::submitFilledPolygon(const std::vector<glm::vec2> &vertices, const glm::vec4 &colour)
{
	// Improve this optimization.
	auto v = m_ViewProj * glm::vec4(vertices[0], 0.0f, 1.0f);
	if (std::abs(v.x) > 1.5f || std::abs(v.y) > 1.5f)
		return;

	int vertexCount = vertices.size();

	// If there is insufficient room, flush the current batch.
	if (MAX_VERTICES - m_TriangleRendererData.verticesCount < (vertexCount - 2) * 3 || MAX_VERTICES - m_LineRendererData.verticesCount < vertexCount * 2)
	{
		flushScene();

		mapTriangleBuffer();
		mapLineBuffer();
	}

	glm::vec4 fillColour{
		colour.r * 0.5f,
		colour.g * 0.5f,
		colour.b * 0.5f,
		colour.a};

	for (int i = 0; i < vertexCount - 2; i++)
	{
		m_TriangleRendererData.batchDataPtr->position = vertices[0];
		m_TriangleRendererData.batchDataPtr->colour = fillColour;

		m_TriangleRendererData.batchDataPtr++;

		m_TriangleRendererData.batchDataPtr->position = vertices[i + 1];
		m_TriangleRendererData.batchDataPtr->colour = fillColour;

		m_TriangleRendererData.batchDataPtr++;

		m_TriangleRendererData.batchDataPtr->position = vertices[i + 2];
		m_TriangleRendererData.batchDataPtr->colour = fillColour;

		m_TriangleRendererData.batchDataPtr++;

		m_TriangleRendererData.verticesCount += 3;
	}

	glm::vec2 p1 = vertices[vertexCount - 1];

	for (int i = 0; i < vertexCount; i++)
	{
		glm::vec2 p2 = vertices[i];

		m_LineRendererData.batchDataPtr->position = p1;
		m_LineRendererData.batchDataPtr->colour = colour;

		m_LineRendererData.batchDataPtr++;

		m_LineRendererData.batchDataPtr->position = p2;
		m_LineRendererData.batchDataPtr->colour = colour;

		m_LineRendererData.batchDataPtr++;

		p1 = p2;

		m_LineRendererData.verticesCount += 2;
	}

	m_RendererStats.rendererSubmissions++;
}

void Renderer::beginScene(const glm::mat4 &viewProj)
{
	if (m_TriangleRendererData.verticesCount > 0 || m_LineRendererData.verticesCount > 0)
		endScene();

	m_ViewProj = viewProj;

	GLint loc;

	glUseProgram(m_TriangleRendererData.program);
	loc = glGetUniformLocation(m_TriangleRendererData.program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_ViewProj));

	glUseProgram(m_LineRendererData.program);
	loc = glGetUniformLocation(m_LineRendererData.program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_ViewProj));

	glUseProgram(0);

	mapTriangleBuffer();
	mapLineBuffer();

	m_RendererStats.rendererDrawCalls = 0;
	m_RendererStats.rendererSubmissions = 0;
	m_RendererStats.rendererVertices = 0;
}

void Renderer::endScene()
{
	// TODO: Sorting etc.

	flushScene();
}

void Renderer::flushScene()
{
	// Clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	flushTriangleVertices();
	flushLineVertices();
}

RendererStats Renderer::getRendererStats()
{
	return m_RendererStats;
}