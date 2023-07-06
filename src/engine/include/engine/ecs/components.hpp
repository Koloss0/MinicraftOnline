// component.h
#pragma once

#include "scene.hpp"
#include <engine/config.hpp>
#include <engine/core/common.hpp>
#include <engine/renderer/material.hpp>
#include <engine/tiles.hpp>

#include <array>
#include <memory>
#include <unordered_map>

struct PositionComponent
{
	int x = 0, y = 0;
};

struct SpriteComponent
{
	IntRect rect{0, 0, 0, 0};
	IntRect source_rect{0, 0, 0, 0};
	std::shared_ptr<Texture> palette_atlas = nullptr;
	unsigned char palette_index = 0;
};

struct MaterialComponent
{
	Material* material = nullptr;
};

struct SpriteAnimatorComponent
{
	double frame_duration = 1.0;
	std::vector<IntRect> frames{};
	double time = 0.0;
	std::size_t frame = 0;
};

struct PlayerComponent
{
};

struct TilemapComponent
{
	struct ChunkLayerData
	{
		TileID tiles[CHUNK_SIZE_TLS * CHUNK_SIZE_TLS];
		unsigned char hit_points[CHUNK_SIZE_TLS * CHUNK_SIZE_TLS];
	};

	struct ChunkData
	{
		unsigned short x = 0, y = 0;
		ChunkLayerData layers[2];
		std::vector<EntityID> tile_entities{};

		TileID get_tile(unsigned char lx, unsigned char ly, size_t layer) const
		{
			const size_t tile_index = static_cast<size_t>(lx + ly * CHUNK_SIZE_TLS);

			return layers[layer].tiles[tile_index];
		}
	};

	std::unordered_map<uint32_t, ChunkData> loaded_chunks{};
	
	inline static uint32_t get_chunk_key(unsigned short x_chks, unsigned short y_chks)
	{ return (x_chks & 0xffffu) << 2 | (y_chks & 0xffffu); }

	const ChunkData* get_chunk(unsigned short x, unsigned short y) const
	{
		uint32_t key = get_chunk_key(x, y);
		auto iter = loaded_chunks.find(key);

		if (iter != loaded_chunks.end())
		{
			return &(iter->second);
		}

		return nullptr; 
	}

	void set_chunk(unsigned short x, unsigned short y, const ChunkData& chunk)
	{
		uint32_t key = get_chunk_key(x, y);
		loaded_chunks[key] = chunk;
	}

	TileID get_tile(unsigned int x, unsigned int y, size_t layer)
	{
		const unsigned short chunk_x_chks = static_cast<unsigned short>(x / CHUNK_SIZE_TLS);
		const unsigned short chunk_y_chks = static_cast<unsigned short>(y / CHUNK_SIZE_TLS);
		
		const ChunkData* chunk = get_chunk(chunk_x_chks, chunk_y_chks);

		if (chunk != nullptr)
		{
			const unsigned char x_ltls = static_cast<unsigned char>(
					x - static_cast<unsigned int>(chunk_x_chks * CHUNK_SIZE_TLS)
					);
			const unsigned char y_ltls = static_cast<unsigned char>(
					y - static_cast<unsigned int>(chunk_y_chks * CHUNK_SIZE_TLS)
					);

			return chunk->get_tile(x_ltls, y_ltls, layer);
		}

		return 0; // chunk isn't loaded
	}
};

struct TilemapRendererComponent
{
	std::shared_ptr<Texture> palette_atlas = nullptr;	
};

