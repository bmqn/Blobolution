#include "Renderer.h"

#include "glm/ext.hpp"

#include <iostream>

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
	const char* vertexShaderSrc =
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
	const char* fragmentShaderSrc =
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
		std::cout << "Vertex shader failed to compile!\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader failed to compile!\n" << infoLog << std::endl;
	}

	triangleRendererData_.program = glCreateProgram();
	glAttachShader(triangleRendererData_.program, vertexShader);
	glAttachShader(triangleRendererData_.program, fragmentShader);
	glLinkProgram(triangleRendererData_.program);

	glDetachShader(triangleRendererData_.program, vertexShader);
	glDetachShader(triangleRendererData_.program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &triangleRendererData_.vao);
	glGenBuffers(1, &triangleRendererData_.vbo);

	glBindVertexArray(triangleRendererData_.vao);

	glBindBuffer(GL_ARRAY_BUFFER, triangleRendererData_.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, (GLvoid*)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::mapTriangleBuffer()
{
	if (triangleRendererData_.verticesCount != 0) return;

	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, triangleRendererData_.vbo);
	triangleRendererData_.batchDataPtr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::cleanupTriangleRenderer()
{
	glDeleteProgram(triangleRendererData_.program);
	glDeleteBuffers(1, &triangleRendererData_.vbo);
	glDeleteVertexArrays(1, &triangleRendererData_.vao);
}

void Renderer::flushTriangleVertices()
{
	// Check that there are actual vertices to flush
	if (triangleRendererData_.verticesCount == 0) return;

	glBindBuffer(GL_ARRAY_BUFFER, triangleRendererData_.vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(triangleRendererData_.program);
	glBindVertexArray(triangleRendererData_.vao);
	glDrawArrays(GL_TRIANGLES, 0, triangleRendererData_.verticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	rendererStats_.rendererVertices += triangleRendererData_.verticesCount;
	rendererStats_.rendererDrawCalls += 1;

	triangleRendererData_.verticesCount = 0;
}

void Renderer::initLineRenderer()
{
	const char* vertexShaderSrc =
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
	const char* fragmentShaderSrc =
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
		std::cout << "Vertex shader failed to compile!\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader failed to compile!\n" << infoLog << std::endl;
	}

	lineRendererData_.program = glCreateProgram();
	glAttachShader(lineRendererData_.program, vertexShader);
	glAttachShader(lineRendererData_.program, fragmentShader);
	glLinkProgram(lineRendererData_.program);

	glDetachShader(lineRendererData_.program, vertexShader);
	glDetachShader(lineRendererData_.program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &lineRendererData_.vao);
	glGenBuffers(1, &lineRendererData_.vbo);

	glBindVertexArray(lineRendererData_.vao);

	glBindBuffer(GL_ARRAY_BUFFER, lineRendererData_.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, (GLvoid*)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::mapLineBuffer()
{
	if (lineRendererData_.verticesCount != 0) return;

	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, lineRendererData_.vbo);
	lineRendererData_.batchDataPtr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::cleanupLineRenderer()
{
	glDeleteProgram(lineRendererData_.program);
	glDeleteBuffers(1, &lineRendererData_.vbo);
	glDeleteVertexArrays(1, &lineRendererData_.vao);
}

void Renderer::flushLineVertices()
{
	// Check that there are actual vertices to flush
	if (lineRendererData_.verticesCount == 0) return;

	glBindBuffer(GL_ARRAY_BUFFER, lineRendererData_.vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(lineRendererData_.program);
	glBindVertexArray(lineRendererData_.vao);
	glDrawArrays(GL_LINES, 0, lineRendererData_.verticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	rendererStats_.rendererVertices += lineRendererData_.verticesCount;
	rendererStats_.rendererDrawCalls += 1;

	lineRendererData_.verticesCount = 0;
}

void Renderer::submitFilledPolygon(const std::vector<glm::vec2>& vertices, const glm::vec4& colour)
{
	// Crude optimization... expensive.
	auto v = viewProj_ * glm::vec4(vertices[0], 0.0f, 1.0f);
	if (std::abs(v.x) > 1.5f || std::abs(v.y) > 1.5f)
		return;

	int vertexCount = vertices.size();

	// If there is insufficient room, flush the current batch.
	if (MAX_VERTICES - triangleRendererData_.verticesCount < (vertexCount - 2) * 3
		|| MAX_VERTICES - lineRendererData_.verticesCount < vertexCount * 2)
	{
		flushScene();

		mapTriangleBuffer();
		mapLineBuffer();
	}

	glm::vec4 fillColour
	{
		colour.r * 0.5f,
		colour.g * 0.5f,
		colour.b * 0.5f,
		colour.a
	};

	for (int i = 0; i < vertexCount - 2; i++)
	{
		triangleRendererData_.batchDataPtr->position = vertices[0];
		triangleRendererData_.batchDataPtr->colour = fillColour;
								   
		triangleRendererData_.batchDataPtr++;
								   
		triangleRendererData_.batchDataPtr->position = vertices[i + 1];
		triangleRendererData_.batchDataPtr->colour = fillColour;
								   
		triangleRendererData_.batchDataPtr++;
								   
		triangleRendererData_.batchDataPtr->position = vertices[i + 2];
		triangleRendererData_.batchDataPtr->colour = fillColour;
								   
		triangleRendererData_.batchDataPtr++;

		triangleRendererData_.verticesCount += 3;
	}

	glm::vec2 p1 = vertices[vertexCount - 1];

	for (int i = 0; i < vertexCount; i++)
	{
		glm::vec2 p2 = vertices[i];

		lineRendererData_.batchDataPtr->position = p1;
		lineRendererData_.batchDataPtr->colour = colour;

		lineRendererData_.batchDataPtr++;

		lineRendererData_.batchDataPtr->position = p2;
		lineRendererData_.batchDataPtr->colour = colour;

		lineRendererData_.batchDataPtr++;

		p1 = p2;

		lineRendererData_.verticesCount += 2;
	}

	rendererStats_.rendererSubmissions++;
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::beginScene(const glm::mat4& viewProj)
{
	viewProj_ = viewProj;

	GLint loc;

	glUseProgram(triangleRendererData_.program);
	loc = glGetUniformLocation(triangleRendererData_.program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewProj_));

	glUseProgram(lineRendererData_.program);
	loc = glGetUniformLocation(lineRendererData_.program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewProj_));

	glUseProgram(0);

	mapTriangleBuffer();
	mapLineBuffer();

	rendererStats_.rendererDrawCalls = 0;
	rendererStats_.rendererSubmissions = 0;
	rendererStats_.rendererVertices = 0;
}

void Renderer::endScene()
{
	// TODO: Sorting etc.

	flushScene();
}

void Renderer::flushScene()
{
	flushTriangleVertices();
	flushLineVertices();
}

RendererStats Renderer::getRendererStats()
{
	return rendererStats_;
}