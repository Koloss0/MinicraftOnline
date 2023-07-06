// image.h
#pragma once

#include <memory>
#include <cstring>
#include <vector>

#include "colour.hpp"

struct Image
{
	unsigned int width, height;
	unsigned char* data;

	static std::shared_ptr<Image> create_palette(const std::vector<colour>& colours)
	{
		std::shared_ptr<Image> img = std::make_shared<Image>();
		img->width = static_cast<unsigned int>(colours.size());
		img->height = 1;
		img->data = new unsigned char[colours.size() * sizeof(colour)];

		for (std::size_t i = 0; i < colours.size(); i++)
		{
			memcpy(&(img->data[4*i]), &colours[i], sizeof(colour));
		}

		return img;
	}
};
