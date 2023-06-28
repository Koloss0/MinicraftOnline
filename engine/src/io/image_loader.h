// image_loader.h
#pragma once

#include <renderer/image.h>

class ImageLoader
{
public:
	static std::shared_ptr<Image> load(const char* filepath, const bool vflip = true);
};
