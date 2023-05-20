// component.h
#pragma once

#include <src/core/common.h>
#include <src/renderer/material.h>
#include <src/ecs/scene.h>

struct TransformComponent
{
	glm::mat3 transform;
};

struct SpriteComponent
{
	Rect rect{0.0f, 0.0f, 0.0f, 0.0f};
	IntRect source_rect{0, 0, 0, 0};
	std::shared_ptr<Texture> palette_atlas = nullptr;
	unsigned char palette_index = 0;
};

struct MaterialComponent
{
	Material* material = nullptr;
};

struct PlayerComponent
{
	static constexpr double FRAME_DURATION = 0.25;
	static const IntRect WALK_FRAMES[];
	double animation_time = 0.0;
	std::size_t current_frame = 0;
};

const IntRect PlayerComponent::WALK_FRAMES[] = {
	{0, -16, 16, 16},
	{16, -16, -16, 16}
};

typedef uint8_t TileID; // 256 possible IDs

struct TilemapComponent
{
	struct ChunkLayerData
	{
		TileID* tiles = nullptr;
	};

	struct ChunkData
	{
		std::vector<ChunkLayerData> layers{};
		std::vector<EntityID> tile_entities{};
	};

	std::vector<ChunkData> chunks{};

	void clear()
	{
		chunks.clear();
	}

	void fill(unsigned short size_chks, unsigned char chunk_size_blks, unsigned char layers_cnt)
	{
		chunks.resize(static_cast<std::vector<ChunkData>::size_type>(size_chks * size_chks));

		for (unsigned int i = 0; i < static_cast<unsigned int>(size_chks) * size_chks; i++)
		{
			// TODO: fix this memory leak
			ChunkLayerData layer{ new TileID[chunk_size_blks * chunk_size_blks] };
			
			std::vector<ChunkLayerData> layers(layers_cnt);
			for (unsigned char j = 0; j < layers_cnt; j++)
			{
				layers[j] = layer;
			}

			chunks[i] = ChunkData{ {layer, layer}, {} };
		}
	}
};

struct TilemapRendererComponent
{
	
};

