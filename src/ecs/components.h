// component.h
#pragma once

#include <src/core/common.h>
#include <src/math/math.h>
#include <src/renderer/material.h>
#include <src/ecs/scene.h>
#include <src/tiles.h>
#include <unordered_map>

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

struct TilemapComponent
{
	struct ChunkLayerData
	{
		std::vector<TileID> tiles{};
		std::vector<unsigned char> hit_points{};
	};

	struct ChunkData
	{
		unsigned short x = 0, y = 0;
		ChunkLayerData floor_layer{};
		ChunkLayerData wall_layer{};
		std::vector<EntityID> tile_entities{};
/*
		TileID get_floor_tile(unsigned char x_ltls, unsigned char y_ltls)
		{
			unsigned short tile_index = x_ltls + y_ltls * m_chunk_size_tls;
			return floor_tiles(x_lts, y_ltls);
		}*/
	};

	std::unordered_map<uint32_t, ChunkData> loaded_chunks{};

private:
	unsigned short m_size_chks = 0;
	unsigned char m_chunk_size_tls = 0;

	inline uint32_t get_chunk_key(unsigned short x_chks, unsigned short y_chks)
	{
		return (x_chks & 0xffffu) << 2 | (y_chks & 0xffffu);
	}

public:
	void clear()
	{
		loaded_chunks.clear();
	}

	void init(unsigned short size_chks, unsigned char chunk_size_tls)
	{
		m_size_chks = size_chks;
		m_chunk_size_tls = chunk_size_tls;
		
		clear();
	}

	void load(unsigned short start_x_chks, unsigned short start_y_chks, unsigned short end_x_chks, unsigned short end_y_chks)
	{
		assert(start_x_chks < m_size_chks);
		assert(start_y_chks < m_size_chks);
		assert(end_x_chks < m_size_chks);
		assert(end_y_chks < m_size_chks);

		unsigned int num_tiles = static_cast<unsigned int>(m_chunk_size_tls * m_chunk_size_tls);
		
		for (unsigned short x_chks = start_x_chks; x_chks <= end_x_chks; x_chks++)
		{
			for (unsigned short y_chks = start_y_chks; y_chks <= end_y_chks; y_chks++)
			{
				// GENERATE NEW CHUNK
				
				std::vector<TileID> floor_tiles;
				std::vector<unsigned char> floor_hit_points(num_tiles, 16);

				// fill floor with tiles
				floor_tiles.reserve(num_tiles);
				for (unsigned int i = 0; i < num_tiles; i++)
				{
					floor_tiles.push_back(static_cast<unsigned char>(Math::randomi(1, 3)));
				}
				
				ChunkLayerData floor_layer{
					floor_tiles,
					floor_hit_points
				};

				std::vector<TileID> wall_tiles(num_tiles);
				std::vector<unsigned char> wall_hit_points(num_tiles, 16);
				
				ChunkLayerData wall_layer{
					wall_tiles,
					wall_hit_points
				};
				
				std::vector<EntityID> tile_entities;

				uint32_t chunk_key = get_chunk_key(x_chks, y_chks); 
				loaded_chunks[chunk_key] = ChunkData{
					x_chks,
					y_chks,
					floor_layer,
					wall_layer,
					tile_entities
				};
			}
		}
	}

	const ChunkData* get_chunk(unsigned short x_chks, unsigned short y_chks)
	{
		uint32_t chunk_key = get_chunk_key(x_chks, y_chks); 
		
		if (loaded_chunks.find(chunk_key) != loaded_chunks.end())
		{
			return &(loaded_chunks.at(chunk_key));
		}
		
		return nullptr;
	}

	TileID get_floor_tile(unsigned int x_tls, unsigned int y_tls)
	{
		const unsigned short chunk_x_chks = static_cast<unsigned short>(x_tls / m_chunk_size_tls);
		const unsigned short chunk_y_chks = static_cast<unsigned short>(y_tls / m_chunk_size_tls);
		
		const ChunkData* chunk = get_chunk(chunk_x_chks, chunk_y_chks);

		if (chunk != nullptr)
		{
			const unsigned char x_ltls = static_cast<unsigned char>(
					x_tls - static_cast<unsigned int>(chunk_x_chks * m_chunk_size_tls)
					);
			const unsigned char y_ltls = static_cast<unsigned char>(
					y_tls - static_cast<unsigned int>(chunk_y_chks * m_chunk_size_tls)
					);

			const size_t tile_index = static_cast<size_t>(
					x_ltls + y_ltls * m_chunk_size_tls
					);

			return chunk->floor_layer.tiles[tile_index];
		}

		return 0; // chunk isn't loaded
	}
};

struct TilemapRendererComponent
{
	
};

