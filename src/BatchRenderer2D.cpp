#include "BatchRenderer2D.h"

#include <iostream>
#include <glad/glad.h>

namespace renderer
{
	static const int MAX_VERTICES = 10 * 2048;

	// Renderer debug variables
	static int renderer_draw_calls = 0;
	static int renderer_submissions = 0;
	static int renderer_vertices = 0;

	static b2Vec2 camera_position;
	static float camera_zoom;

#pragma region triangle_batch_renderer

	struct TriangleVertex
	{
		b2Vec2 position;
		b2Color colour;
	};

	// Triangle batch renderer data
	struct TriangleBatchRendererData
	{
		int vertices_count;
		GLuint program;
		GLuint vao, vbo;
		TriangleVertex* batch_data_ptr;
	};

	static TriangleBatchRendererData triangle_renderer_data;

	void init_triangle_renderer()
	{
		const char* vertex_shader_text =
			"#version 330 core\n"
			"layout (location = 0) in vec2 a_Position;\n"
			"layout (location = 1) in vec3 a_Colour;\n"
			"out vec3 v_Colour;\n"
			"uniform vec2 u_CamPos;\n"
			"uniform float u_CamZoom;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4((u_CamPos + a_Position) * u_CamZoom, 0.0, 1.0);\n"
			"	v_Colour = a_Colour;\n"
			"}\n";
		const char* fragment_shader_text =
			"#version 330 core\n"
			"layout (location = 0) out vec4 o_Colour;\n"
			"in vec3 v_Colour;\n"
			"void main()\n"
			"{\n"
			"	o_Colour = vec4(v_Colour, 1.0f);\n"
			"}\n";

		int success;
		char infoLog[512];

		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
		glCompileShader(vertex_shader);
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
			std::cout << "Vertex shader failed to compile!\n" << infoLog << std::endl;
		}

		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
			std::cout << "Fragment shader failed to compile!\n" << infoLog << std::endl;
		}

		triangle_renderer_data.program = glCreateProgram();
		glAttachShader(triangle_renderer_data.program, vertex_shader);
		glAttachShader(triangle_renderer_data.program, fragment_shader);
		glLinkProgram(triangle_renderer_data.program);

		glDetachShader(triangle_renderer_data.program, vertex_shader);
		glDetachShader(triangle_renderer_data.program, fragment_shader);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		glGenVertexArrays(1, &triangle_renderer_data.vao);
		glGenBuffers(1, &triangle_renderer_data.vbo);

		glBindVertexArray(triangle_renderer_data.vao);

		glBindBuffer(GL_ARRAY_BUFFER, triangle_renderer_data.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertex) * MAX_VERTICES, (GLvoid*)0, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), (GLvoid*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	void cleanup_triangle_renderer()
	{
		glDeleteProgram(triangle_renderer_data.program);
		glDeleteBuffers(1, &triangle_renderer_data.vbo);
		glDeleteVertexArrays(1, &triangle_renderer_data.vao);
	}

	static void triangle_vertices_flush()
	{
		// Check that there are actual vertices to flush
		if (triangle_renderer_data.vertices_count == 0) return;

		glBindBuffer(GL_ARRAY_BUFFER, triangle_renderer_data.vbo);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(triangle_renderer_data.program);
		glBindVertexArray(triangle_renderer_data.vao);
		glDrawArrays(GL_TRIANGLES, 0, triangle_renderer_data.vertices_count);
		glBindVertexArray(0);
		glUseProgram(0);

		renderer_vertices += triangle_renderer_data.vertices_count;
		triangle_renderer_data.vertices_count = 0;
		renderer_draw_calls++;
	}

	static void map_triangle_buffer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, triangle_renderer_data.vbo);
		triangle_renderer_data.batch_data_ptr = (TriangleVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#pragma endregion

#pragma region line_batch_renderer

	struct LineVertex
	{
		b2Vec2 position;
		b2Color colour;
	};

	// Triangle batch renderer data
	struct LineBatchRendererData
	{
		int vertices_count;
		GLuint program;
		GLuint vao, vbo;
		LineVertex* batch_data_ptr;
	};

	static LineBatchRendererData line_renderer_data;

	void init_line_renderer()
	{
		const char* vertex_shader_text =
			"#version 330 core\n"
			"layout (location = 0) in vec2 a_Position;\n"
			"layout (location = 1) in vec3 a_Colour;\n"
			"out vec3 v_Colour;\n"
			"uniform vec2 u_CamPos;\n"
			"uniform float u_CamZoom;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4((u_CamPos + a_Position) * u_CamZoom, 0.0, 1.0);\n"
			"	v_Colour = a_Colour;\n"
			"}\n";
		const char* fragment_shader_text =
			"#version 330 core\n"
			"layout (location = 0) out vec4 o_Colour;\n"
			"in vec3 v_Colour;\n"
			"void main()\n"
			"{\n"
			"	o_Colour = vec4(v_Colour, 1.0f);\n"
			"}\n";

		int success;
		char infoLog[512];

		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
		glCompileShader(vertex_shader);
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
			std::cout << "Vertex shader failed to compile!\n" << infoLog << std::endl;
		}

		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
			std::cout << "Fragment shader failed to compile!\n" << infoLog << std::endl;
		}

		line_renderer_data.program = glCreateProgram();
		glAttachShader(line_renderer_data.program, vertex_shader);
		glAttachShader(line_renderer_data.program, fragment_shader);
		glLinkProgram(line_renderer_data.program);

		glDetachShader(line_renderer_data.program, vertex_shader);
		glDetachShader(line_renderer_data.program, fragment_shader);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		glGenVertexArrays(1, &line_renderer_data.vao);
		glGenBuffers(1, &line_renderer_data.vbo);

		glBindVertexArray(line_renderer_data.vao);

		glBindBuffer(GL_ARRAY_BUFFER, line_renderer_data.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * MAX_VERTICES, (GLvoid*)0, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	void cleanup_line_renderer()
	{
		glDeleteProgram(line_renderer_data.program);
		glDeleteBuffers(1, &line_renderer_data.vbo);
		glDeleteVertexArrays(1, &line_renderer_data.vao);
	}

	static void line_vertices_flush()
	{
		// Check that there are actual vertices to flush
		if (line_renderer_data.vertices_count == 0) return;

		glBindBuffer(GL_ARRAY_BUFFER, line_renderer_data.vbo);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(line_renderer_data.program);
		glBindVertexArray(line_renderer_data.vao);
		glDrawArrays(GL_LINES, 0, line_renderer_data.vertices_count);
		glBindVertexArray(0);
		glUseProgram(0);

		renderer_vertices += line_renderer_data.vertices_count;

		line_renderer_data.vertices_count = 0;

		renderer_draw_calls++;
	}

	static void map_line_buffer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, line_renderer_data.vbo);
		line_renderer_data.batch_data_ptr = (LineVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#pragma endregion

	void submit_filled_polygon(const b2Vec2 vertices[], int vertex_count, const b2Color& colour)
	{
		if (vertices->x < camera_position.x - 1.5f / camera_zoom ||
			vertices->x > camera_position.x + 1.5f / camera_zoom ||
			vertices->y < camera_position.y - 1.0f / camera_zoom ||
			vertices->y > camera_position.y + 1.0f / camera_zoom)
			return;

		// Check that there is enough room to submit these vertices.
		int triangle_vertices_remaining = MAX_VERTICES - triangle_renderer_data.vertices_count;
		int line_vertices_remaining = MAX_VERTICES - line_renderer_data.vertices_count;
		if (triangle_vertices_remaining < (vertex_count - 2) * 3)
		{
			// If there is insufficient room, flush the current batch.
			triangle_vertices_flush();
			map_triangle_buffer();
		}
		if (line_vertices_remaining < vertex_count * 2)
		{
			// If there is insufficient room, flush the current batch.
			line_vertices_flush();
			map_line_buffer();
		}

		b2Color fill_colour
		{
			colour.r * 0.5f,
			colour.g * 0.5f,
			colour.b * 0.5f,
		};

		for (int i = 0; i < vertex_count - 2; i++)
		{
			triangle_renderer_data.batch_data_ptr->position = vertices[0];
			triangle_renderer_data.batch_data_ptr->colour = fill_colour;

			triangle_renderer_data.batch_data_ptr++;

			triangle_renderer_data.batch_data_ptr->position = vertices[i + 1];
			triangle_renderer_data.batch_data_ptr->colour = fill_colour;

			triangle_renderer_data.batch_data_ptr++;

			triangle_renderer_data.batch_data_ptr->position = vertices[i + 2];
			triangle_renderer_data.batch_data_ptr->colour = fill_colour;

			triangle_renderer_data.batch_data_ptr++;

			triangle_renderer_data.vertices_count += 3;
		}

		b2Vec2 p1 = vertices[vertex_count - 1];
		for (int i = 0; i < vertex_count; i++)
		{
			b2Vec2 p2 = vertices[i];

			line_renderer_data.batch_data_ptr->position = p1;
			line_renderer_data.batch_data_ptr->colour = colour;

			line_renderer_data.batch_data_ptr++;

			line_renderer_data.batch_data_ptr->position = p2;
			line_renderer_data.batch_data_ptr->colour = colour;

			line_renderer_data.batch_data_ptr++;

			p1 = p2;

			line_renderer_data.vertices_count += 2;
		}

		renderer_submissions++;
	}

	void submit_polygon(const b2Vec2 vertices[], int vertex_count, const b2Color& colour)
	{
		if (vertices->x < camera_position.x - 1.5f / camera_zoom ||
			vertices->x > camera_position.x + 1.5f / camera_zoom ||
			vertices->y < camera_position.y - 1.0f / camera_zoom ||
			vertices->y > camera_position.y + 1.0f / camera_zoom)
			return;

		// Check that there is enough room to submit these vertices.
		int line_vertices_remaining = MAX_VERTICES - line_renderer_data.vertices_count;
		if (line_vertices_remaining < vertex_count * 2)
		{
			// If there is insufficient room, flush the current batch.
			line_vertices_flush();
			map_line_buffer();
		}

		b2Vec2 p1 = vertices[vertex_count - 1];
		for (int i = 0; i < vertex_count; i++)
		{
			b2Vec2 p2 = vertices[i];

			line_renderer_data.batch_data_ptr->position = p1;
			line_renderer_data.batch_data_ptr->colour = colour;

			line_renderer_data.batch_data_ptr++;

			line_renderer_data.batch_data_ptr->position = p2;
			line_renderer_data.batch_data_ptr->colour = colour;

			line_renderer_data.batch_data_ptr++;

			p1 = p2;

			line_renderer_data.vertices_count += 2;
		}

		renderer_submissions++;
	}

	void submit_line(const b2Vec2& p1, const b2Vec2& p2, const b2Color& colour)
	{
		if (p1.x < camera_position.x - 1.5f / camera_zoom ||
			p1.x > camera_position.x + 1.5f / camera_zoom ||
			p1.y < camera_position.y - 1.0f / camera_zoom ||
			p1.y > camera_position.y + 1.0f / camera_zoom)
			return;

		// Check that there is enough room to submit these vertices.
		int line_vertices_remaining = MAX_VERTICES - line_renderer_data.vertices_count;
		if (line_vertices_remaining <  2)
		{
			// If there is insufficient room, flush the current batch.
			line_vertices_flush();
			map_line_buffer();
		}

		line_renderer_data.batch_data_ptr->position = p1;
		line_renderer_data.batch_data_ptr->colour = colour;

		line_renderer_data.batch_data_ptr++;

		line_renderer_data.batch_data_ptr->position = p2;
		line_renderer_data.batch_data_ptr->colour = colour;

		line_renderer_data.batch_data_ptr++;

		line_renderer_data.vertices_count += 2;
	}

	void begin_drawing(b2Vec2 cam_pos, float cam_zoom)
	{
		camera_position = cam_pos;
		camera_zoom = cam_zoom;

		glUseProgram(triangle_renderer_data.program);

		GLint loc = glGetUniformLocation(triangle_renderer_data.program, "u_CamPos");
		glUniform2f(loc, -cam_pos.x, -cam_pos.y);

		loc = glGetUniformLocation(triangle_renderer_data.program, "u_CamZoom");
		glUniform1f(loc, cam_zoom);

		glUseProgram(0);

		glUseProgram(line_renderer_data.program);

		loc = glGetUniformLocation(line_renderer_data.program, "u_CamPos");
		glUniform2f(loc, -cam_pos.x, -cam_pos.y);

		loc = glGetUniformLocation(line_renderer_data.program, "u_CamZoom");
		glUniform1f(loc, cam_zoom);

		glUseProgram(0);

		map_triangle_buffer();
		map_line_buffer();

		renderer_draw_calls = 0;
		renderer_submissions = 0;
		renderer_vertices = 0;
	}

	void end_drawing()
	{
		triangle_vertices_flush();
		line_vertices_flush();
	}

	RenderStats get_render_stats()
	{
		RenderStats stats;
		stats.renderer_draw_calls = renderer_draw_calls;
		stats.renderer_submissions = renderer_submissions;
		stats.renderer_vertices = renderer_vertices;

		return stats;
	}

	void init_renderer()
	{
		init_triangle_renderer();
		init_line_renderer();
	}

	void cleanup_renderer()
	{
		cleanup_triangle_renderer();
		cleanup_line_renderer();
	}
}

