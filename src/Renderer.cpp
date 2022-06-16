#include "Renderer.h"
#include "Log.h"

#include <glm/ext.hpp>

// TODO: Get capabilities
static constexpr size_t kMaxVertices = 8 * 1024;

struct Vertex
{
	glm::vec3 WorldPosition;
	glm::vec2 LocalPosition;
	glm::vec4 Colour;
};

struct BatchRendererData
{
	GLuint Program;
	GLuint Vao, Vbo;
	Vertex *BatchDataPtr;
	GLsizei VerticesCount;

	BatchRendererData()
		: Program(0)
		, Vao(0)
		, Vbo(0)
		, BatchDataPtr(nullptr)
		, VerticesCount(0)
	{
	}
};

static glm::mat4 s_ViewProj;
static BatchRendererData s_CircleRendererData;
static BatchRendererData s_TriangleRendererData;
static BatchRendererData s_LineRendererData;

void Renderer::Create()
{
	InitCircleRenderer();
	InitTriangleRenderer();
	InitLineRenderer();

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Enable wireframe rendering
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::Destroy()
{
	CleanupCircleRenderer();
	CleanupTriangleRenderer();
	CleanupLineRenderer();
}

void Renderer::InitCircleRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_WorldPosition;\n"
		"layout (location = 1) in vec2 a_LocalPosition;\n"
		"layout (location = 2) in vec4 a_Colour;\n"
		"out vec2 v_LocalPosition;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_LocalPosition = a_LocalPosition;\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_WorldPosition, 1.0);\n"
		"}\n";

	const char *fragmentShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) out vec4 o_Colour;\n"
		"in vec4 v_Colour;\n"
		"in vec2 v_LocalPosition;\n"
		"void main()\n"
		"{\n"
		"	float distance = 1.0 - length(v_LocalPosition);\n"
		"	if (distance <= 0.0)\n"
		"		discard;\n"
		"	if (distance <= 0.05)\n"
		"		o_Colour = v_Colour;\n"
		"	else\n"
		"		o_Colour = v_Colour * 0.5;\n"
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
		BL_LOG("Circle vertex shader failed to compile! %s", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		BL_LOG("Circle fragnent shader failed to compile! %s", infoLog);
	}

	s_CircleRendererData.Program = glCreateProgram();
	glAttachShader(s_CircleRendererData.Program, vertexShader);
	glAttachShader(s_CircleRendererData.Program, fragmentShader);
	glLinkProgram(s_CircleRendererData.Program);

	glDetachShader(s_CircleRendererData.Program, vertexShader);
	glDetachShader(s_CircleRendererData.Program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &s_CircleRendererData.Vao);
	glGenBuffers(1, &s_CircleRendererData.Vbo);

	glBindVertexArray(s_CircleRendererData.Vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_CircleRendererData.Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * kMaxVertices, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(5 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::MapCircleBuffer()
{
	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_CircleRendererData.Vbo);
	s_CircleRendererData.BatchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::UnmapCircleBuffer()
{
	// Unmap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_CircleRendererData.Vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::CleanupCircleRenderer()
{
	glDeleteProgram(s_CircleRendererData.Program);
	glDeleteBuffers(1, &s_CircleRendererData.Vbo);
	glDeleteVertexArrays(1, &s_CircleRendererData.Vao);
}

void Renderer::FlushCircleVertices()
{
	UnmapCircleBuffer();

	// Check that there are actual vertices to flush
	if (s_CircleRendererData.VerticesCount == 0)
		return;

	glUseProgram(s_CircleRendererData.Program);
	glBindVertexArray(s_CircleRendererData.Vao);
	glDrawArrays(GL_TRIANGLES, 0, s_CircleRendererData.VerticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	s_CircleRendererData.VerticesCount = 0;
}

void Renderer::InitTriangleRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_WorldPosition;\n"
		"layout (location = 1) in vec2 a_LocalPosition;\n"
		"layout (location = 2) in vec4 a_Colour;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_WorldPosition, 1.0);\n"
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
		BL_LOG("Triangle vertex shader failed to compile! %s", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		BL_LOG("Triangle fragnent shader failed to compile! %s", infoLog);
	}

	s_TriangleRendererData.Program = glCreateProgram();
	glAttachShader(s_TriangleRendererData.Program, vertexShader);
	glAttachShader(s_TriangleRendererData.Program, fragmentShader);
	glLinkProgram(s_TriangleRendererData.Program);

	glDetachShader(s_TriangleRendererData.Program, vertexShader);
	glDetachShader(s_TriangleRendererData.Program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &s_TriangleRendererData.Vao);
	glGenBuffers(1, &s_TriangleRendererData.Vbo);

	glBindVertexArray(s_TriangleRendererData.Vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_TriangleRendererData.Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * kMaxVertices, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(5 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::MapTriangleBuffer()
{
	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_TriangleRendererData.Vbo);
	s_TriangleRendererData.BatchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::UnmapTriangleBuffer()
{
	// Unmap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_TriangleRendererData.Vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::CleanupTriangleRenderer()
{
	glDeleteProgram(s_TriangleRendererData.Program);
	glDeleteBuffers(1, &s_TriangleRendererData.Vbo);
	glDeleteVertexArrays(1, &s_TriangleRendererData.Vao);
}

void Renderer::FlushTriangleVertices()
{
	UnmapTriangleBuffer();

	// Check that there are actual vertices to flush
	if (s_TriangleRendererData.VerticesCount == 0)
		return;

	glUseProgram(s_TriangleRendererData.Program);
	glBindVertexArray(s_TriangleRendererData.Vao);
	glDrawArrays(GL_TRIANGLES, 0, s_TriangleRendererData.VerticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	s_TriangleRendererData.VerticesCount = 0;
}

void Renderer::InitLineRenderer()
{
	const char *vertexShaderSrc =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_WorldPosition;\n"
		"layout (location = 1) in vec2 a_LocalPosition;\n"
		"layout (location = 2) in vec4 a_Colour;\n"
		"out vec4 v_Colour;\n"
		"uniform mat4 u_ViewProj;\n"
		"void main()\n"
		"{\n"
		"	v_Colour = a_Colour;\n"
		"	gl_Position = u_ViewProj * vec4(a_WorldPosition, 1.0);\n"
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
		BL_LOG("Line vertex shader failed to compile! %s", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		BL_LOG("Line fragnent shader failed to compile! %s", infoLog);
	}

	s_LineRendererData.Program = glCreateProgram();
	glAttachShader(s_LineRendererData.Program, vertexShader);
	glAttachShader(s_LineRendererData.Program, fragmentShader);
	glLinkProgram(s_LineRendererData.Program);

	glDetachShader(s_LineRendererData.Program, vertexShader);
	glDetachShader(s_LineRendererData.Program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &s_LineRendererData.Vao);
	glGenBuffers(1, &s_LineRendererData.Vbo);

	glBindVertexArray(s_LineRendererData.Vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_LineRendererData.Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * kMaxVertices, (GLvoid *)0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(5 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::MapLineBuffer()
{
	// Remap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_LineRendererData.Vbo);
	s_LineRendererData.BatchDataPtr = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::UnmapLineBuffer()
{
	// Unmap the buffer
	glBindBuffer(GL_ARRAY_BUFFER, s_LineRendererData.Vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::CleanupLineRenderer()
{
	glDeleteProgram(s_LineRendererData.Program);
	glDeleteBuffers(1, &s_LineRendererData.Vbo);
	glDeleteVertexArrays(1, &s_LineRendererData.Vao);
}

void Renderer::FlushLineVertices()
{
	UnmapLineBuffer();

	// Check that there are actual vertices to flush
	if (s_LineRendererData.VerticesCount == 0)
		return;

	glUseProgram(s_LineRendererData.Program);
	glBindVertexArray(s_LineRendererData.Vao);
	glDrawArrays(GL_LINES, 0, s_LineRendererData.VerticesCount);
	glBindVertexArray(0);
	glUseProgram(0);

	s_LineRendererData.VerticesCount = 0;
}

void Renderer::Clear()
{
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetViewportSize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Renderer::BeginScene(const glm::mat4 &viewProj)
{
	if (s_CircleRendererData.VerticesCount > 0
		|| s_TriangleRendererData.VerticesCount > 0
		|| s_LineRendererData.VerticesCount > 0)
	{
		EndScene();
	}

	s_ViewProj = viewProj;

	GLint loc;

	glUseProgram(s_CircleRendererData.Program);
	loc = glGetUniformLocation(s_CircleRendererData.Program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(s_ViewProj));

	glUseProgram(s_TriangleRendererData.Program);
	loc = glGetUniformLocation(s_TriangleRendererData.Program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(s_ViewProj));

	glUseProgram(s_LineRendererData.Program);
	loc = glGetUniformLocation(s_LineRendererData.Program, "u_ViewProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(s_ViewProj));

	glUseProgram(0);

	MapCircleBuffer();
	MapTriangleBuffer();
	MapLineBuffer();
}

void Renderer::EndScene()
{
	// TODO: Sorting etc.

	FlushScene();
}

void Renderer::FlushScene()
{
	FlushCircleVertices();
	FlushTriangleVertices();
	FlushLineVertices();
}

void Renderer::SubmitFilledCircle(const glm::vec3 &position, float radius, const glm::vec4 &colour)
{
	// Improve this optimization.
	auto v = s_ViewProj * glm::vec4(position, 1.0f);
	if (std::abs(v.x) > 1.5f || std::abs(v.y) > 1.5f)
		return;

	// If there is insufficient room, flush the current batch.
	if (kMaxVertices - s_CircleRendererData.VerticesCount < 6)
	{
		Renderer::FlushCircleVertices();
		Renderer::MapCircleBuffer();
	}

	glm::vec3 p1 = glm::vec3(-1.0, -1.0, position.z);
	glm::vec3 p2 = glm::vec3(-1.0, 1.0, position.z);
	glm::vec3 p3 = glm::vec3(1.0, 1.0, position.z);
	glm::vec3 p4 = glm::vec3(1.0, -1.0, position.z);

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p1 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p1;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p2 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p2;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p3 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p3;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p3 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p3;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p4 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p4;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.BatchDataPtr->WorldPosition = position + p1 * radius;
	s_CircleRendererData.BatchDataPtr->LocalPosition = p1;
	s_CircleRendererData.BatchDataPtr->Colour = colour;

	s_CircleRendererData.BatchDataPtr++;

	s_CircleRendererData.VerticesCount += 6;
}

void Renderer::SubmitFilledPolygon(const std::vector<glm::vec3> &vertices, const glm::vec4 &colour)
{
	// Improve this optimization.
	auto v = s_ViewProj * glm::vec4(vertices[0], 1.0f);
	if (std::abs(v.x) > 1.5f || std::abs(v.y) > 1.5f)
		return;

	size_t vertexCount = vertices.size();

	// If there is insufficient room, flush the current batch.
	if (kMaxVertices - s_TriangleRendererData.VerticesCount < (vertexCount - 2) * 3
		|| kMaxVertices - s_LineRendererData.VerticesCount < vertexCount * 2)
	{
		Renderer::FlushTriangleVertices();
		Renderer::FlushLineVertices();
		Renderer::MapTriangleBuffer();
		Renderer::MapLineBuffer();
	}

	glm::vec4 fillColour{
		colour.r * 0.5f,
		colour.g * 0.5f,
		colour.b * 0.5f,
		colour.a};

	for (size_t i = 0; i < vertexCount - 2; i++)
	{
		s_TriangleRendererData.BatchDataPtr->WorldPosition = vertices[0];
		// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
		s_TriangleRendererData.BatchDataPtr->Colour = fillColour;

		s_TriangleRendererData.BatchDataPtr++;

		s_TriangleRendererData.BatchDataPtr->WorldPosition = vertices[i + 1];
		// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
		s_TriangleRendererData.BatchDataPtr->Colour = fillColour;

		s_TriangleRendererData.BatchDataPtr++;

		s_TriangleRendererData.BatchDataPtr->WorldPosition = vertices[i + 2];
		// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
		s_TriangleRendererData.BatchDataPtr->Colour = fillColour;

		s_TriangleRendererData.BatchDataPtr++;

		s_TriangleRendererData.VerticesCount += 3;
	}

	glm::vec3 p1 = vertices[vertexCount - 1];

	for (size_t i = 0; i < vertexCount; i++)
	{
		glm::vec3 p2 = vertices[i];

		s_LineRendererData.BatchDataPtr->WorldPosition = p1 + glm::vec3(0.0f, 0.0f, 0.01f);
		// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
		s_LineRendererData.BatchDataPtr->Colour = colour;

		s_LineRendererData.BatchDataPtr++;

		s_LineRendererData.BatchDataPtr->WorldPosition = p2 + glm::vec3(0.0f, 0.0f, 0.01f);
		;
		// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
		s_LineRendererData.BatchDataPtr->Colour = colour;

		s_LineRendererData.BatchDataPtr++;

		p1 = p2;

		s_LineRendererData.VerticesCount += 2;
	}
}

void Renderer::SubmitLine(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec4 &colour)
{
	// Improve this optimization.
	auto v = s_ViewProj * glm::vec4(p1, 1.0f);
	if (std::abs(v.x) > 1.5f || std::abs(v.y) > 1.5f)
		return;

	// If there is insufficient room, flush the current batch.
	if (kMaxVertices - s_LineRendererData.VerticesCount < 2)
	{
		Renderer::FlushLineVertices();
		Renderer::MapLineBuffer();
	}

	s_LineRendererData.BatchDataPtr->WorldPosition = p1;
	// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
	s_LineRendererData.BatchDataPtr->Colour = colour;

	s_LineRendererData.BatchDataPtr++;

	s_LineRendererData.BatchDataPtr->WorldPosition = p2;
	// s_TriangleRendererData.BatchDataPtr->LocalPosition = glm::vec2();
	s_LineRendererData.BatchDataPtr->Colour = colour;

	s_LineRendererData.BatchDataPtr++;
}