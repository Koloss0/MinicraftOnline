// image_loader.h
#pragma once

#include <engine/renderer/image.hpp>

namespace Engine
{
	class ImageLoader
	{
	public:
		static std::shared_ptr<Image> load(const char* filepath, const bool vflip = true);
	};
}
