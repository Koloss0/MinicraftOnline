// image.h
#pragma once

#include <memory>
#include <vector>
#include <glm/vec4.hpp>

struct Image
{
	unsigned int width, height;
	unsigned char* data;

	static std::shared_ptr<Image> create_palette(const std::vector<glm::ivec4>& colours)
	{
		std::shared_ptr<Image> img = std::make_shared<Image>();
		img->width = static_cast<unsigned int>(colours.size());
		img->height = 1;
		img->data = new unsigned char[colours.size()];

		for (std::size_t i = 0; i < colours.size(); i++)
		{
			const glm::vec4 col = colours[i];
			img->data[i*4] = static_cast<unsigned char>(col.r);
			img->data[i*4+1] = static_cast<unsigned char>(col.g);
			img->data[i*4+2] = static_cast<unsigned char>(col.b);
			img->data[i*4+3] = static_cast<unsigned char>(col.a);
		}

		return img;
	}
};
