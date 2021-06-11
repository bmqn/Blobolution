#pragma once

#include <box2d/box2d.h>

namespace renderer
{
	struct RenderStats
	{
		int renderer_submissions;
		int renderer_draw_calls;
		int renderer_vertices;
	};

	void init_renderer();
	void cleanup_renderer();

	void begin_drawing(b2Vec2 cam_pos, float cam_zoom);
	void end_drawing();

	void submit_filled_polygon(const b2Vec2 vertices[], int vertex_count, const b2Color& colour);
	void submit_polygon(const b2Vec2 vertices[], int vertex_count, const b2Color& colour);
	void submit_line(const b2Vec2& p1, const b2Vec2& p2, const b2Color& colour);

	RenderStats get_render_stats();
}