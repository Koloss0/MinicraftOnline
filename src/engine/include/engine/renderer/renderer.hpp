#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "texture.hpp"

namespace Engine
{
	namespace Renderer
	{
		const int VIEWPORT_WIDTH = 256;
		const int VIEWPORT_HEIGHT = 144;

		void init(unsigned int window_width, unsigned int window_height);
		void draw_rect(const glm::ivec2& position, const glm::ivec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, const std::shared_ptr<Texture>& palette_atlas, const unsigned char palette = 0, const glm::vec3& tint = glm::vec3(1.0f)); 
		void draw_rect(const int x, const int y, const int width, const int height, const int source_x, const int source_y, const int source_width, const int source_height, const std::shared_ptr<Texture>& palette_atlas, const unsigned char palette = 0, const glm::vec3& tint = glm::vec3(1.0f)); 
		void begin();
		void end();
		void flush();
		void shutdown();
		void update_viewport_size(unsigned int width, unsigned int height);
	}
}
