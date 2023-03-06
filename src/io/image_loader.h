// image_loader.h
#pragma once

#include "../renderer/image.h"
#include <memory>

class ImageLoader
{
public:
	static std::shared_ptr<Image> load(const char* filepath);
};
