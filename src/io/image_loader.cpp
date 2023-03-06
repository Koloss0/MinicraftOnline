// image_loader.cpp

#include <iostream>
#include "image_loader.h"
#include <STB/stb_image.h>
#include <memory>

std::shared_ptr<Image> ImageLoader::load(const char* filepath)
{
	stbi_set_flip_vertically_on_load(true);
	
	std::shared_ptr<Image> img = std::make_shared<Image>();

	int w, h, nr_channels;
	img->data = stbi_load(filepath, &w, &h, &nr_channels, STBI_rgb_alpha);

	if (!img->data)
	{
		throw std::runtime_error(
				"ERROR: Failed to load image at '" +
				static_cast<std::string>(filepath) +
				"'"
			);
	}

	img->width = static_cast<unsigned int>(w);
	img->height = static_cast<unsigned int>(h);

	return img;
}
